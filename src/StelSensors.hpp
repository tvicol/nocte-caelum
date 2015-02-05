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


#ifndef STELSENSORS_HPP
#define STELSENSORS_HPP

#include <QAccelerometer>
#include <QMagnetometer>
#include <QGyroscope>
#include <QCompass>
#include <QElapsedTimer>

#include "StelQuaternion.hpp"
#include "VecMath.hpp"

class StelSensors : public QObject
{
    Q_OBJECT

public:
    explicit StelSensors(QObject *parent = 0);
    ~StelSensors();

    Mat4d calcMatrix();

    void startSensors();
    void stopSensors();

    bool advance(double dt = 1./40.);

    bool getEnabled() const { return enabled; }

    static inline Quat q_dot2(const Quat& q, const Quat& q_dot, const Quat& w_dot)
    {
        return q_dot * q.conj() * q_dot + q * w_dot * 0.5;
    }

    static double elapsedTime()
    {
        return elapsedTimer.nsecsElapsed() * 1e-9;
    }

private:

    void gyroMethod(const Quat&, double);
    void inertialMethod(const Quat&, double, double);

    QAccelerometer* accelSensor;
    QMagnetometer* magSensor;
    QGyroscope* gyroSensor;
    QCompass* compassSensor;

    class GyroFilter* gyroFilter;
    class MagFilter* magFilter;
    class AccelFilter* accelFilter;

    Quat rotation;
    Vec3 angVel;

    friend class AccelFilter;

    bool firstMeasurement;
    bool enabled;

    unsigned int frameNo;

    static QElapsedTimer elapsedTimer;
};





#endif
