/*
 * Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
 * Contact: Marko Srebre <starstuffn9@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */


#include "StelSensors.hpp"
#include "StelQuaternion.hpp"
#include "StelMovementMgr.hpp"
#include "StelApp.hpp"
#include "StelModuleMgr.hpp"

#include <QMagnetometer>

#include <QDebug>>


struct MagFilter : public QMagnetometerFilter
{
    virtual bool filter(QMagnetometerReading *r)
    {
        if (C == 0.)
            return true;

        r->setX(prevReading.x * C + r->x() * (1. - C));
        r->setY(prevReading.y * C + r->y() * (1. - C));
        r->setZ(prevReading.z * C + r->z() * (1. - C));
        prevReading.set(r->x(), r->y(), r->z());
        return true;
    }
    Vec3 prevReading;
    double C;
};



struct AccelFilter : public QAccelerometerFilter
{
    virtual bool filter(QAccelerometerReading *r)
    {
        if (C == 0.)
            return true;

        r->setX(prevReading.x * C + r->x() * (1. - C));
        r->setY(prevReading.y * C + r->y() * (1. - C));
        r->setZ(prevReading.z * C + r->z() * (1. - C));
        prevReading.set(r->x(), r->y(), r->z());
        return true;
    }
    Vec3 prevReading;
    double C;
};


/** Gyro fitler for tracking current rotation from last know rotation */
struct GyroFilter : public QGyroscopeFilter
{
    virtual bool filter(QGyroscopeReading *r)
    {
        if (!initialRotationIsSet)
            return true;

        const double now = StelSensors::elapsedTime();
        const double dt = now - t0;
        if (dt < 0.001)
            return true;

        Vec3 axis(r->x(), r->y(), r->z());
        double angle = axis.normalize() * dt * M_PI / 180.;
        if (angle < 1e-7)
            return true;

        q0 = Quat(axis, angle) * q0;
        t0 = now;
        Q_ASSERT(fabs(q0.normalize() - 1.0) < 1e6);
        return true;
    }

    bool initialRotationIsSet;
    Quat q0;
    double t0;
};



QElapsedTimer StelSensors::elapsedTimer;


StelSensors::StelSensors(QObject *parent) :
    QObject(parent), enabled(false), firstMeasurement(true), frameNo(0)
{
    gyroFilter = new GyroFilter;
    magFilter = new MagFilter;
    accelFilter = new AccelFilter;

    magSensor = new QMagnetometer(this);
    magSensor->addFilter(magFilter);
    //magSensor->setReturnGeoValues(true); // this doesn't work on jolla phones (!)

    accelSensor = new QAccelerometer(this);
    accelSensor->addFilter(accelFilter);

    gyroSensor = new QGyroscope(this);
    gyroSensor->addFilter(gyroFilter);

}


StelSensors::~StelSensors()
{
    stopSensors();
    delete gyroFilter;
    delete magFilter;
    delete accelFilter;
}


bool StelSensors::advance(double dt)
{
    if (!enabled)
        return false;

    QAccelerometerReading* accelReading = accelSensor->reading();
    QMagnetometerReading* magReading = magSensor->reading();

    if (!accelReading || !magReading)
        return false;


    // Gravity acceleration; normalized to equal alt/az up in device frame.
    Vec3 u(accelReading->x(),
           accelReading->y(),
           accelReading->z());
    if (u.normalize() < 1e-8)
        return false;

    // Magentic field direction, points north and down
    Vec3 b(magReading->x(),
           magReading->y(),
           magReading->z());
    if (b.normalize() < 1e-8)
        return false;

    Vec3 e(b^u);	// east
    e.normalize();

    Vec3 n(u^e);	// north
    n.normalize();

    Matrix3 m;
    m.set(-n.x, e.x, u.x,
          -n.y, e.y, u.y,
          -n.z, e.z, u.z);

    Quat qt(m); // target rotation

    if (firstMeasurement)
    {
        firstMeasurement = false;
        rotation = qt;
        angVel = Vec3(0., 0., 0.);
        gyroFilter->q0 = qt;
        gyroFilter->t0 = elapsedTime();
        gyroFilter->initialRotationIsSet = true;
        return true;
    }

    frameNo++;

    StelMovementMgr* mmgr = GETSTELMODULE(StelMovementMgr);
    double fov = mmgr->getCurrentFov();
    double smooth = fov > 60. ? 0. : (60. - fov) / 60.;


    if (gyroSensor->isActive())
    {
        gyroMethod(qt, smooth);
    }
    else
    {
        inertialMethod(qt, smooth, dt);
    }

    return true;
}


/** Accurate gyro method used in Jolla which is equipped with gyroscope sensor */
void StelSensors::gyroMethod(const Quat& qt, double smooth)
{
    Quat q;
    q.slerp(0.02, gyroFilter->q0, qt);
    if (frameNo % 50 == 0)
        q.normalize();

    gyroFilter->q0 = q;

    rotation.slerp(pow(smooth, 0.6), q, rotation);
    if (frameNo % 50 == 0)
        rotation.normalize();
}



/** Inertial method used in Nokia N9 which lacks gyroscope sensor */
void StelSensors::inertialMethod(const Quat& qt, double smooth, double dt)
{
    double Kp = 23.0 * (1.0 - smooth);
    double Kd = 8.;

    Quat& q0 = rotation;
    Vec3& w0 = angVel;

    // calc rotation error in device frame
    Vec3 error;
    {
        double angle;
        (q0.conj() * qt).getAngleAxis(angle, error);
        error *= angle;
    }

    // integrate to next rotation state
    //
    // see Rigid body dynamics using Euler’s equations, Runge-Kutta and quaternions.
    // by Indrek Mandre <indrek@mare.ee> http://www.mare.ee/indrek/


    Quat w_dot(error * Kp - w0 * Kd);

    Quat q0_dot = q0 * Quat(w0) * 0.5;

    Quat k1 = q_dot2(q0, q0_dot, w_dot);
    Quat C = q0 + q0_dot*dt*0.5 + k1*(dt*dt*0.125);
    Quat k2 = q_dot2(C, q0_dot + k1*dt*0.5, w_dot);
    Quat k3 = q_dot2(C, q0_dot + k2*dt*0.5, w_dot);
    Quat k4 = q_dot2(q0 + q0_dot*dt + k3*(dt*dt*0.5), q0_dot + k3*dt, w_dot);

    Quat q1 = q0 + q0_dot * dt + (k1 + k2 + k3)*(dt*dt/6.0);
    Quat q1_dot = q0_dot + (k1 + k2*2. + k3*2. + k4)*(dt/6.0);

    Quat w1 = q1.conj() * q1_dot * 2.;

    q0 = q1;

    if (frameNo++ % 100 == 0)
    {
        q0.normalize();
    }

    w0 = w1.getVec3();

}

void StelSensors::startSensors()
{
    qDebug() << "Starting sensors";
    if (enabled)
        return;

    elapsedTimer.start();


    firstMeasurement = true;
    if (!magSensor->isActive())
        magSensor->start();

    if (!magSensor->isActive())
    {
        qWarning() << "Magnetometer Sensor didn't start!" << endl;
    }

    if (!accelSensor->isActive())
        accelSensor->start();

    if (!accelSensor->isActive())
    {
        qWarning() << "Accelerometer Sensor didn't start!" << endl;
    }

    gyroFilter->initialRotationIsSet = false;

    if (!gyroSensor->isActive())
        gyroSensor->start();

    if (gyroSensor->isActive())
    {
        accelFilter->prevReading.set(0., 0., 0.);
        magFilter->prevReading.set(0., 0., 0.);
        accelFilter->C = 0.7;
        magFilter->C = 0.7;
    }
    else
    {
        accelFilter->C = 0.;
        magFilter->C = 0.;
        qWarning() << "Could not start gyrometer. Trying to do without." << endl;
    }

    enabled = accelSensor->isActive() && magSensor->isActive();
    if (!enabled)
    {
        qWarning() << "StelSensors are not enabled.";
    }


}

void StelSensors::stopSensors()
{
    qDebug() << "Stopping sensors";
    if (magSensor->isActive())
        magSensor->stop();

    if (accelSensor->isActive())
        accelSensor->stop();

    if (gyroSensor->isActive())
        gyroSensor->stop();

    enabled = false;
}

Mat4d StelSensors::calcMatrix()
{
    Matrix3 m;
    rotation.getMatrix(m);

//    return Mat4d(m.v[0][0], m.v[0][1], m.v[0][2], 0.,
//                 m.v[1][0], m.v[1][1], m.v[1][2], 0.,
//                 m.v[2][0], m.v[2][1], m.v[2][2], 0.,
//                 0., 0., 0., 1.);

    return Mat4d(m.v[0][0], m.v[1][0], m.v[2][0], 0.,
                 m.v[0][1], m.v[1][1], m.v[2][1], 0.,
                 m.v[0][2], m.v[1][2], m.v[2][2], 0.,
                 0., 0., 0., 1.);
}
