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

#include "StelMainSkyItem.hpp"
#include "StelSearch.hpp"
#include "StelSensors.hpp"

#include "StelApp.hpp"
#include "StelQGL2Renderer.hpp"
#include "StelMovementMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelLocationMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelFileMgr.hpp"

#include "MeteorMgr.hpp"
#include "MilkyWay.hpp"
#include "StelSkyCultureMgr.hpp"

#include "ConstellationMgr.hpp"
#include "GridLinesMgr.hpp"
#include "LandscapeMgr.hpp"
#include "SolarSystem.hpp"
#include "StarMgr.hpp"
#include "NebulaMgr.hpp"


#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLPaintDevice>

#include <QQuickView>
#include <QQmlContext>
#include <QStringListModel>
#include <QSGNode>


class StelMainSkyItemRenderClient : public StelRenderClient
{
public:

    explicit StelMainSkyItemRenderClient(StelMainSkyItem* item, QPainter* painter) :
        StelRenderClient(),
        item(item),
        painter(painter)

    {}
    virtual bool drawPartial() { return item->paintPartial(); }
    virtual QPainter* getPainter() { return painter; }
    virtual StelViewportEffect* getViewportEffect() { return 0; } //TODO? }

private:

    StelMainSkyItem* const item;
    QPainter* const painter;
};


// Animation forces redraw every frame
struct Animation : public QAbstractAnimation
{
    Animation(StelMainSkyItem* c) : QAbstractAnimation(c) {}
    virtual int duration() const { return -1; }
    virtual void updateCurrentTime(int currentTime)
    {
        QQuickWindow* win = static_cast<StelMainSkyItem*>(parent())->window();
        if (win)
            win->update();
    }
};


StelMainSkyItem::StelMainSkyItem(QQuickItem *parent) :
    QQuickItem(parent),
    stelSearch(0),
    stelApp(0),
    renderer(0),
    paintDevice(0),
    paintState(0),
    firstFrame(true),
    lastEventTimeSec(0),
    frameNo(0),
    _fps(0),
    _minFps(30), // obsolete
    _maxFps(60), // obsolete
    _fov(0),
    _updateObjectSearchResults(false),
    _updateLocationSearchResults(false),
    _addLocationToList(false),
    _removeLocationFromList(false),
    _applyCurrentLocation(false),
    _objectSearchResultsModel(0),
    _locationSearchResultsModel(0),
    _gotoObject(false),
    _gotoRecheck(false),
    _selectNamedLocation(false),
    _saveSettings(false)
{
    _objectSearchResultsModel = new QStringListModel;
    _locationSearchResultsModel = new QStringListModel;
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);

    loadInfoTextFiltersConfig();

    sensors = new StelSensors(this);

    Animation* animation = new Animation(this);
    animation->start();
}

StelMainSkyItem::~StelMainSkyItem()
{
    delete _objectSearchResultsModel;
    delete _locationSearchResultsModel;
    sensors->stopSensors();
    cleanup();
}



void StelMainSkyItem::itemChange(ItemChange change, const ItemChangeData &)
{
    // The ItemSceneChange event is sent when we are first attached to a window.
    if (change == ItemSceneChange) {
        QQuickWindow *win = window();
        if (!win)
            return;

        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
        connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.

        window()->setClearBeforeRendering(false);

        connect(this, &StelMainSkyItem::objectSearchResultsUpdated, this, &StelMainSkyItem::setObjectSearchResults);
        connect(this, &StelMainSkyItem::locationSearchResultsUpdated, this, &StelMainSkyItem::setLocationSearchResults);
    }
}


void StelMainSkyItem::paint()
{
    // init stellarium app/renderer but not on the first frame; allow the first frame for init screen
    if (renderer == 0)
    {
        if (firstFrame)
        {
            firstFrame = false;
            return;
        }

        // TODO: not being called, why?
        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);

        Q_ASSERT(paintDevice == 0);
        Q_ASSERT(stelApp == 0);

        renderer = new StelQGL2Renderer(0, false);
        renderer->setQGLContext(QGLContext::fromOpenGLContext(QOpenGLContext::currentContext()));
        if(!renderer->init())
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Failed to initialize renderer");
        }

        previousPaintTime = StelApp::getTotalRunTime();
        stelApp = new StelApp();
        stelApp->init(renderer);

        stelSearch = new StelSearch;

        //Q_ASSERT(viewportEffect==NULL);
        //setViewportEffect(conf->value("video/viewport_effect", "none").toString());

        stelApp->windowHasBeenResized(window()->x(), window()->y(), window()->width(), window()->height());
        renderer->viewportHasBeenResized(window()->size());

        previousPaintTime = StelApp::getTotalRunTime();
        previousFrameTime = StelApp::getTotalRunTime();

        paintDevice = new QOpenGLPaintDevice;
        paintDevice->setSize(window()->size());

        initConnections();

        emit loaded();
    }


    QPainter painter(paintDevice);
    StelMainSkyItemRenderClient client(this, &painter);

    if (renderer->getSkipNumFrames() != _skipNumFrames)
    {
        renderer->setSkipNumFrames(_skipNumFrames);
    }

    renderer->renderFrame(client);

    frameNo++;
}

void StelMainSkyItem::cleanup()
{
    qDebug() << "StelMainSkyItem: clean up.";
    if (stelApp)
    {
        delete stelApp;
        stelApp = 0;
    }
    if (renderer)
    {
        delete renderer;
        renderer = 0;
    }
    if (stelSearch)
    {
        delete stelSearch;
        stelSearch = 0;
    }
    if (paintDevice)
    {
        delete paintDevice;
        paintDevice = 0;
    }

}

void StelMainSkyItem::gotoNamedObject(const QString &text, bool recheck)
{
    _gotoObject = true;
    _gotoName = text;
    _gotoRecheck = recheck;
}


void StelMainSkyItem::selectNamedLocation(const QString &text, bool recheck)
{
    _selectNamedLocation = true;
    _selectLocationName = text;
    _selectLocationRecheck = recheck;
}


void StelMainSkyItem::sync()
{
    if (!stelApp || !stelApp->getCore())
        return;

    StelCore* core = stelApp->getCore();

    _fps = stelApp->getFps();
    _timerate = (int) (core->getTimeRate() / StelCore::JD_SECOND);
    _fov = core->getMovementMgr()->getCurrentFov();
    _location = core->getCurrentLocation().getID();

    double jd = core->getJDay();
    _prettyDatetime = stelApp->getLocaleMgr().getPrintableDateLocal(jd) + " " +
            stelApp->getLocaleMgr().getPrintableTimeLocal(jd);


    if (_saveSettings)
    {
        _saveSettings = false;
        saveCurrentViewOptions();
    }

    if (_updateObjectSearchResults)
    {
        _updateObjectSearchResults = false;
        QStringList matches;
        stelSearch->updateSearchResults(_objectSearchString, matches);
        emit objectSearchResultsUpdated(matches);
    }


    if (_updateLocationSearchResults)
    {
        _updateLocationSearchResults = false;
        StelLocationMgr& locmgr = StelApp::getInstance().getLocationMgr();
        QStringList list;
        QString text = _locationSearchString;
        if (text.size() ==  0)
        {
            //list += locmgr.getModelAll()->stringList();
        }
        else
        {
            QRegExp rx("\\b" + text.toLower());
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            list += locmgr.getModelAll()->stringList().filter(rx);
        }
        emit locationSearchResultsUpdated(list);
    }

    if (_applyCurrentLocation)
    {
        _applyCurrentLocation=  false;
        StelLocation loc = getCurrentLocation();
        StelApp::getInstance().getCore()->moveObserverTo(loc, 0.);
        setLocationCanBeDeleted(stelApp->getLocationMgr().canDeleteUserLocation(loc.getID()));
    }


    if (_gotoObject)
    {
       _gotoObject = false;
       bool result = stelSearch->gotoNamedObject(_gotoName, _gotoRecheck);
       emit gotoCompleted(result);
    }


    if (_selectNamedLocation)
    {
        _selectNamedLocation = false;
        if (_selectLocationName.size() > 0)
        {
            StelLocationMgr& locmgr = stelApp->getLocationMgr();
            QString locName;
            if (_selectLocationRecheck)
            {
                QStringList list = locmgr.getModelAll()->stringList().filter(_selectLocationName, Qt::CaseInsensitive);
                if (list.count() == 0)
                    return;
                locName = list[0];
            }
            else
            {
                locName = _selectLocationName;
            }

            StelLocation loc = locmgr.locationForSmallString(locName);
            setCurrentLocation(loc);
            stelApp->getCore()->moveObserverTo(loc, 0.);
            SolarSystem* ssm = GETSTELMODULE(SolarSystem);
            PlanetP p = ssm->searchByEnglishName(loc.planetName);
            LandscapeMgr* ls = GETSTELMODULE(LandscapeMgr);
            ls->setFlagAtmosphere(p->hasAtmosphere());
            ls->setFlagFog(p->hasAtmosphere());
            emit locationFound();
        }
    }

    if (_addLocationToList)
    {
        _addLocationToList = false;
        stelApp->getLocationMgr().saveUserLocation(getCurrentLocation());
    }

    if (_removeLocationFromList)
    {
        _removeLocationFromList = false;
        stelApp->getLocationMgr().deleteUserLocation(getCurrentLocation().getID());
    }

    if (_freeLook)
    {
        if (!sensors->getEnabled())
        {
            sensors->startSensors();
            if (!sensors->getEnabled())
            {
                qWarning() << "Cannot start StelSensors, disabling freelook.";
                setFreeLook(false);
            }
        }

        sensors->advance();
        core->setFreeLookMatrix(sensors->calcMatrix());
        thereWasAnEvent();
    }
    else
    {
        if (sensors->getEnabled())
            sensors->stopSensors();
    }

    if (frameNo % 30 == 0)
        updateInfoText();

    emit synced();
}


bool StelMainSkyItem::paintPartial()
{
    if (paintState == 0)
    {
        const double now = StelApp::getTotalRunTime();
        double dt = now-previousPaintTime;
        previousPaintTime = now;
        if (dt<0)       // This fix the star scale bug!!
            return false;

        // Update the core and all modules
        stelApp->update(dt);
        paintState = 1;
        return true;
    }
    if (paintState == 1)
    {
        // And draw them
        if (stelApp->drawPartial(renderer))
            return true;

        paintState = 0;
        return false;
    }
    Q_ASSERT(false);
    return false;
}


void StelMainSkyItem::mouseMoveEvent(QMouseEvent *event)
{
    thereWasAnEvent();
    QPointF pos = event->pos();
    flipMousePos(pos);
    stelApp->handleMove(pos.x(), pos.y(), event->buttons());
}

void StelMainSkyItem::flipMousePos(QPointF& pos)
{
    //qDebug() << "x " << pos.x() << " y " << pos.y() << " w " << window()->width();
    if (_orientation == 1) // portrait
    {
        pos.setY(window()->height() - 1 - pos.y());
    }
    else // landscape
    {
        float tmp = pos.y();
        pos.setY(window()->height() - 1 - pos.x());
        pos.setX(window()->width() - 1 - tmp);
    }
    //qDebug() << "after x " << pos.x() << " y " << pos.y();
}

void StelMainSkyItem::mousePressEvent(QMouseEvent *event)
{
    thereWasAnEvent();
    QPointF pos = event->pos();
    flipMousePos(pos);
    QMouseEvent newEvent(QEvent::MouseButtonPress, QPoint(pos.x(),pos.y()), event->button(), event->buttons(), event->modifiers());
    stelApp->handleClick(&newEvent);
}


void StelMainSkyItem::mouseReleaseEvent(QMouseEvent *event)
{
    thereWasAnEvent();
    QPointF pos = event->pos();
    flipMousePos(pos);
    QMouseEvent newEvent(QEvent::MouseButtonRelease, QPoint(pos.x(),pos.y()), event->button(), event->buttons(), event->modifiers());
    stelApp->handleClick(&newEvent);
}

void StelMainSkyItem::touchEvent(QTouchEvent* event)
{
    if (event->touchPoints().count() == 2)
        stelApp->handleTouch(event);
    else
        QQuickItem::touchEvent(event);
}


void StelMainSkyItem::thereWasAnEvent()
{
    lastEventTimeSec = StelApp::getTotalRunTime();
}


void StelMainSkyItem::updateInfoText()
{
    const QList<StelObjectP>& selected = GETSTELMODULE(StelObjectMgr)->getSelectedObject();
    if (selected.isEmpty())
    {
        setNameText("");
        setInfoText("");
    }
    else
    {
        StelCore* core = stelApp->getCore();

        QString s = selected[0]->getInfoString(core, infoTextFilters);

        QString name, infoText;
        int h0 = s.indexOf("<h2>");
        int h1 = s.indexOf("</h2>");
        if (h0 >= 0 && h1 > 0)
        {
            name = s.mid(h0+4, h1-h0-4);
            infoText = s.mid(h1+5);
        }
        else
        {
            infoText = s;
        }

        setInfoText(infoText);
        setInfoColor(StelUtils::vec3fToHtmlColor(selected[0]->getInfoColor()));

        setNameText(name);
        setNameColor(StelUtils::vec3fToHtmlColor(selected[0]->getInfoColor()));
    }
}


void StelMainSkyItem::loadInfoTextFiltersConfig()
{
    QSettings* conf = StelApp::getConfig();
    QString objectInfo = conf->value("gui/selected_object_info", "all").toString();
    if (objectInfo == "all")
    {
        infoTextFilters = StelObject::InfoStringGroup(StelObject::AllInfo);
    }
    else if (objectInfo == "short")
    {
        infoTextFilters = StelObject::InfoStringGroup(StelObject::ShortInfo);
    }
    else if (objectInfo == "none")
    {
        infoTextFilters = StelObject::InfoStringGroup(0);
    }
    else if (objectInfo == "custom")
    {
        infoTextFilters = StelObject::InfoStringGroup(0);

        conf->beginGroup("custom_selected_info");
        if (conf->value("flag_show_name", false).toBool())
            infoTextFilters |= StelObject::Name;
        if (conf->value("flag_show_catalognumber", false).toBool())
            infoTextFilters |= StelObject::CatalogNumber;
        if (conf->value("flag_show_magnitude", false).toBool())
            infoTextFilters |= StelObject::Magnitude;
        if (conf->value("flag_show_absolutemagnitude", false).toBool())
            infoTextFilters |= StelObject::AbsoluteMagnitude;
        if (conf->value("flag_show_radecj2000", false).toBool())
            infoTextFilters |= StelObject::RaDecJ2000;
        if (conf->value("flag_show_radecofdate", false).toBool())
            infoTextFilters |= StelObject::RaDecOfDate;
        if (conf->value("flag_show_hourangle", false).toBool())
            infoTextFilters |= StelObject::HourAngle;
        if (conf->value("flag_show_altaz", false).toBool())
            infoTextFilters |= StelObject::AltAzi;
        if (conf->value("flag_show_distance", false).toBool())
            infoTextFilters |= StelObject::Distance;
        if (conf->value("flag_show_size", false).toBool())
            infoTextFilters |= StelObject::Size;
        if (conf->value("flag_show_extra1", false).toBool())
            infoTextFilters |= StelObject::Extra1;
        if (conf->value("flag_show_extra2", false).toBool())
            infoTextFilters |= StelObject::Extra2;
        if (conf->value("flag_show_extra3", false).toBool())
            infoTextFilters |= StelObject::Extra3;

        conf->endGroup();
    }
    else
    {
        qWarning() << "config.ini option gui/selected_object_info is invalid, using \"all\"";
        infoTextFilters = StelObject::InfoStringGroup(StelObject::AllInfo);
    }
}

void StelMainSkyItem::setCurrentLocation(const StelLocation& loc)
{

    setLocationName(loc.name);
    setLocationCountry(loc.country);
    setLocationPlanet(loc.planetName);
    setLocationLongitude(loc.longitude);
    setLocationLatitude(loc.latitude);
    setLocationAltitude(loc.altitude);
    setLocationCanBeDeleted(stelApp->getLocationMgr().canDeleteUserLocation(loc.getID()));
    setLocationIsNew(false);

    if (false && loc.planetName=="Earth")
    {
        setTerrainImageSource("/data/gui/world.png");
    }
    else
    {
        SolarSystem* ssm = GETSTELMODULE(SolarSystem);
        PlanetP p = ssm->searchByEnglishName(loc.planetName);
        QString path;
        if (p)
        {
            try
            {
                path = StelFileMgr::findFile("textures/"+p->getTextMapName());
            }
            catch (std::runtime_error& e)
            {
                qWarning() << "ERROR - could not find planet map for " << loc.planetName << e.what();
                return;
            }
            setTerrainImageSource(path);
        }
    }

}



StelLocation StelMainSkyItem::getCurrentLocation()
{
    StelLocation loc;
    loc.name = _locationName;
    loc.country = _locationCountry;
    loc.planetName = _locationPlanet;
    loc.altitude = _locationAltitude;
    loc.longitude = _locationLongitude;
    loc.latitude = _locationLatitude;
    return loc;
}



void StelMainSkyItem::initConnections()
{
    ConstellationMgr* cmgr = GETSTELMODULE(ConstellationMgr);

    setConstLines(cmgr->getFlagLines());
    connect(cmgr, &ConstellationMgr::linesDisplayedChanged, this, &StelMainSkyItem::setConstLines);
    connect(this, &StelMainSkyItem::constLinesChanged, cmgr, &ConstellationMgr::setFlagLines);

    setConstLabels(cmgr->getFlagLabels());
    connect(cmgr, &ConstellationMgr::namesDisplayedChanged, this, &StelMainSkyItem::setConstLabels);
    connect(this, &StelMainSkyItem::constLabelsChanged, cmgr, &ConstellationMgr::setFlagLabels);

    setConstBoundaries(cmgr->getFlagBoundaries());
    connect(cmgr, &ConstellationMgr::boundariesDisplayedChanged, this, &StelMainSkyItem::setConstBoundaries);
    connect(this, &StelMainSkyItem::constBoundariesChanged, cmgr, &ConstellationMgr::setFlagBoundaries);

    setConstArt(cmgr->getFlagArt());
    //connect(cmgr, &ConstellationMgr::artDisplayedChanged, this, &StelMainSkyItem::setConstArt);
    connect(this, &StelMainSkyItem::constArtChanged, cmgr, &ConstellationMgr::setFlagArt);



    GridLinesMgr* g = GETSTELMODULE(GridLinesMgr);

    setEquatorialGrid(g->getFlagEquatorGrid());
    connect(g, &GridLinesMgr::equatorGridDisplayedChanged, this, &StelMainSkyItem::setEquatorialGrid);
    connect(this, &StelMainSkyItem::equatorialGridChanged, g, &GridLinesMgr::setFlagEquatorGrid);

    setAzimuthalGrid(g->getFlagAzimuthalGrid());
    connect(g, &GridLinesMgr::azimuthalGridDisplayedChanged, this, &StelMainSkyItem::setAzimuthalGrid);
    connect(this, &StelMainSkyItem::azimuthalGridChanged, g, &GridLinesMgr::setFlagAzimuthalGrid);

    setGalacticGrid(g->getFlagGalacticGrid());
    connect(g, &GridLinesMgr::galacticGridDisplayedChanged, this, &StelMainSkyItem::setGalacticGrid);
    connect(this, &StelMainSkyItem::galacticGridChanged, g, &GridLinesMgr::setFlagGalacticGrid);

    setMeridianLine(g->getFlagMeridianLine());
    connect(g, &GridLinesMgr::meridianLineDisplayedChanged, this, &StelMainSkyItem::setMeridianLine);
    connect(this, &StelMainSkyItem::meridianLineChanged, g, &GridLinesMgr::setFlagMeridianLine);

    setEcliptic(g->getFlagEclipticLine());
    connect(g, &GridLinesMgr::eclipticLineDisplayedChanged, this, &StelMainSkyItem::setEcliptic);
    connect(this, &StelMainSkyItem::eclipticChanged, g, &GridLinesMgr::setFlagEclipticLine);

    setEcliptic(g->getFlagEclipticLine());
    connect(g, &GridLinesMgr::eclipticLineDisplayedChanged, this, &StelMainSkyItem::setEcliptic);
    connect(this, &StelMainSkyItem::eclipticChanged, g, &GridLinesMgr::setFlagEclipticLine);


    LandscapeMgr* lmgr = GETSTELMODULE(LandscapeMgr);

    setGround(lmgr->getFlagLandscape());
    connect(lmgr, &LandscapeMgr::landscapeDisplayedChanged, this, &StelMainSkyItem::setGround);
    connect(this, &StelMainSkyItem::groundChanged, lmgr, &LandscapeMgr::setFlagLandscape);
    connect(this, &StelMainSkyItem::groundChanged, lmgr, &LandscapeMgr::setFlagFog);

    setFog(lmgr->getFlagFog());
    connect(lmgr, &LandscapeMgr::fogDisplayedChanged, this, &StelMainSkyItem::setFog);
    connect(this, &StelMainSkyItem::fogChanged, lmgr, &LandscapeMgr::setFlagFog);

    setAtmosphere(lmgr->getFlagAtmosphere());
    connect(lmgr, &LandscapeMgr::atmosphereDisplayedChanged, this, &StelMainSkyItem::setAtmosphere);
    connect(this, &StelMainSkyItem::atmosphereChanged, lmgr, &LandscapeMgr::setFlagAtmosphere);

    setCardinalPoints(lmgr->getFlagCardinalsPoints());
    connect(lmgr, &LandscapeMgr::cardinalsPointsDisplayedChanged, this, &StelMainSkyItem::setCardinalPoints);
    connect(this, &StelMainSkyItem::cardinalPointsChanged, lmgr, &LandscapeMgr::setFlagCardinalsPoints);


    StarMgr* smgr = GETSTELMODULE(StarMgr);
    setStarLabels(smgr->getFlagLabels());
    setStarLabelsAmount(smgr->getLabelsAmount());
    connect(this, &StelMainSkyItem::starLabelsChanged, smgr, &StarMgr::setFlagLabels);
    connect(this, &StelMainSkyItem::starLabelsAmountChanged, smgr, &StarMgr::setLabelsAmount);

    SolarSystem* ss = GETSTELMODULE(SolarSystem);
    setPlanetLabels(ss->getFlagLabels());
    setPlanetLabelsAmount(ss->getLabelsAmount());
    connect(this, &StelMainSkyItem::planetLabelsChanged, ss, &SolarSystem::setFlagLabels);
    connect(this, &StelMainSkyItem::planetLabelsAmountChanged, ss, &SolarSystem::setLabelsAmount);

    NebulaMgr* nmgr = GETSTELMODULE(NebulaMgr);
    setNebulaLabels(nmgr->getFlagHints());
    setNebulaLabelsAmount(nmgr->getLabelsAmount());
    connect(this, &StelMainSkyItem::nebulaLabelsChanged, nmgr, &NebulaMgr::setFlagHints);
    connect(this, &StelMainSkyItem::nebulaLabelsAmountChanged, nmgr, &NebulaMgr::setLabelsAmount);
    connect(this, &StelMainSkyItem::nebulaLabelsAmountChanged, nmgr, &NebulaMgr::setHintsAmount);

    _nightMode = stelApp->getVisionModeNight();
    emit nightModeChanged(_nightMode);
    connect(this, &StelMainSkyItem::nightModeChanged, stelApp, &StelApp::setVisionModeNight);


    StelObjectMgr* objMgr = GETSTELMODULE(StelObjectMgr);
    StelMovementMgr* moveMgr = GETSTELMODULE(StelMovementMgr);
    StelCore* core = stelApp->getCore();

    connect(this, &StelMainSkyItem::timeSetToNow, core, &StelCore::setTimeNow);
    connect(this, &StelMainSkyItem::realtimeSpeedToggled, core, &StelCore::toggleRealTimeSpeed);
    connect(this, &StelMainSkyItem::timeSpeedIncreased, core, &StelCore::increaseTimeSpeed);
    connect(this, &StelMainSkyItem::timeSpeedDecreased, core, &StelCore::decreaseTimeSpeed);
    connect(this, &StelMainSkyItem::selectionCentered, moveMgr, &StelMovementMgr::setFlagTracking);
    connect(this, &StelMainSkyItem::selectionCleared, objMgr, &StelObjectMgr::unSelect);

    connect(this, &StelMainSkyItem::freeLookChanged, core, &StelCore::setFreeLook);
    connect(this, &StelMainSkyItem::orientationChanged, core, &StelCore::setOrientation);
    connect(objMgr, &StelObjectMgr::selectedObjectChanged, this, &StelMainSkyItem::updateInfoText);
    //updateInfoText();

    setOrientation(1); // 1 - portrait; 2 - landscape

    StelSkyDrawer* skyDrawer = core->getSkyDrawer();

    setDynamicEyeAdaptation(skyDrawer->getFlagLuminanceAdaptation());
    connect(this, &StelMainSkyItem::dynamicEyeAdaptationChanged, skyDrawer, &StelSkyDrawer::setFlagLuminanceAdaptation);

    setExtinction(skyDrawer->getExtinctionCoefficient());
    connect(this, &StelMainSkyItem::extinctionChanged, skyDrawer, &StelSkyDrawer::setExtinctionCoefficient);

    setAtmospherePressure(skyDrawer->getAtmospherePressure());
    connect(this, &StelMainSkyItem::atmospherePressureChanged, skyDrawer, &StelSkyDrawer::setAtmospherePressure);

    setAtmosphereTemp(skyDrawer->getAtmosphereTemperature());
    connect(this, &StelMainSkyItem::atmosphereTempChanged, skyDrawer, &StelSkyDrawer::setAtmosphereTemperature);

    setLightPollution(lmgr->getAtmosphereBortleLightPollution());
    connect(this, &StelMainSkyItem::lightPollutionChanged, lmgr, &LandscapeMgr::setAtmosphereBortleLightPollution);
    connect(this, &StelMainSkyItem::lightPollutionChanged, skyDrawer, &StelSkyDrawer::setBortleScale);

    setStarAbsScale(skyDrawer->getAbsoluteStarScale());
    connect(this, &StelMainSkyItem::starAbsScaleChanged, skyDrawer, &StelSkyDrawer::setAbsoluteStarScale);

    setStarRelScale(skyDrawer->getRelativeStarScale());
    connect(this, &StelMainSkyItem::starRelScaleChanged, skyDrawer, &StelSkyDrawer::setRelativeStarScale);

    skyDrawer->setFlagTwinkle(true);
    setTwinkleAmount(skyDrawer->getTwinkleAmount());
    connect(this, &StelMainSkyItem::twinkleAmountChanged, skyDrawer, &StelSkyDrawer::setTwinkleAmount);

    MeteorMgr* meteorMgr = GETSTELMODULE(MeteorMgr);
    setMeteorZHR(meteorMgr->getZHR());
    connect(this, &StelMainSkyItem::meteorZHRChanged, meteorMgr, &MeteorMgr::setZHR);
    connect(meteorMgr, &MeteorMgr::zhrChanged, this, &StelMainSkyItem::setMeteorZHR);


    setCurrentLocation(core->getCurrentLocation());

}



void StelMainSkyItem::saveCurrentViewOptions()
{
    QSettings* conf = StelApp::getInstance().getSettings();
    Q_ASSERT(conf);

    LandscapeMgr* lmgr = GETSTELMODULE(LandscapeMgr);
    Q_ASSERT(lmgr);
    SolarSystem* ssmgr = GETSTELMODULE(SolarSystem);
    Q_ASSERT(ssmgr);
    MeteorMgr* mmgr = GETSTELMODULE(MeteorMgr);
    Q_ASSERT(mmgr);
    StelSkyDrawer* skyd = StelApp::getInstance().getCore()->getSkyDrawer();
    Q_ASSERT(skyd);
    ConstellationMgr* cmgr = GETSTELMODULE(ConstellationMgr);
    Q_ASSERT(cmgr);
    StarMgr* smgr = GETSTELMODULE(StarMgr);
    Q_ASSERT(smgr);
    NebulaMgr* nmgr = GETSTELMODULE(NebulaMgr);
    Q_ASSERT(nmgr);
    GridLinesMgr* glmgr = GETSTELMODULE(GridLinesMgr);
    Q_ASSERT(glmgr);
    StelMovementMgr* mvmgr = GETSTELMODULE(StelMovementMgr);
    Q_ASSERT(mvmgr);

    StelCore* core = StelApp::getInstance().getCore();
    const StelProjectorP proj = core->getProjection(StelCore::FrameJ2000);
    Q_ASSERT(proj);

    // view dialog / sky tab settings
    conf->setValue("stars/absolute_scale", skyd->getAbsoluteStarScale());
    conf->setValue("stars/relative_scale", skyd->getRelativeStarScale());
    conf->setValue("stars/flag_star_twinkle", skyd->getFlagTwinkle());
    conf->setValue("stars/star_twinkle_amount", skyd->getTwinkleAmount());
    conf->setValue("astro/flag_star_magnitude_limit",
                   skyd->getFlagStarMagnitudeLimit());
    conf->setValue("astro/star_magnitude_limit",
                   skyd->getCustomStarMagnitudeLimit());
    conf->setValue("astro/flag_nebula_magnitude_limit",
                   skyd->getFlagNebulaMagnitudeLimit());
    conf->setValue("astro/nebula_magnitude_limit",
                   skyd->getCustomNebulaMagnitudeLimit());
    conf->setValue("viewing/use_luminance_adaptation", skyd->getFlagLuminanceAdaptation());
    conf->setValue("astro/flag_planets", ssmgr->getFlagPlanets());
    conf->setValue("astro/flag_planets_hints", ssmgr->getFlagHints());
    conf->setValue("astro/flag_planets_orbits", ssmgr->getFlagOrbits());
    conf->setValue("astro/flag_light_travel_time", ssmgr->getFlagLightTravelTime());
    conf->setValue("viewing/flag_moon_scaled", ssmgr->getFlagMoonScale());
    conf->setValue("astro/meteor_rate", mmgr->getZHR());
    conf->setValue("astro/milky_way_intensity", GETSTELMODULE(MilkyWay)->getIntensity());

    // view dialog / markings tab settings
    conf->setValue("viewing/flag_azimuthal_grid", glmgr->getFlagAzimuthalGrid());
    conf->setValue("viewing/flag_equatorial_grid", glmgr->getFlagEquatorGrid());
    conf->setValue("viewing/flag_equator_line", glmgr->getFlagEquatorLine());
    conf->setValue("viewing/flag_ecliptic_line", glmgr->getFlagEclipticLine());
    conf->setValue("viewing/flag_ecliptic_J2000_grid", glmgr->getFlagEclipticJ2000Grid());
    conf->setValue("viewing/flag_meridian_line", glmgr->getFlagMeridianLine());
    conf->setValue("viewing/flag_horizon_line", glmgr->getFlagHorizonLine());
    conf->setValue("viewing/flag_equatorial_J2000_grid", glmgr->getFlagEquatorJ2000Grid());
    conf->setValue("viewing/flag_galactic_grid", glmgr->getFlagGalacticGrid());
    conf->setValue("viewing/flag_galactic_plane_line", glmgr->getFlagGalacticPlaneLine());
    conf->setValue("viewing/flag_cardinal_points", lmgr->getFlagCardinalsPoints());
    conf->setValue("viewing/flag_constellation_drawing", cmgr->getFlagLines());
    conf->setValue("viewing/flag_constellation_name", cmgr->getFlagLabels());
    conf->setValue("viewing/flag_constellation_boundaries", cmgr->getFlagBoundaries());
    conf->setValue("viewing/flag_constellation_art", cmgr->getFlagArt());
    conf->setValue("viewing/flag_constellation_isolate_selected", cmgr->getFlagIsolateSelected());
    conf->setValue("viewing/flag_landscape_autoselection", lmgr->getFlagLandscapeAutoSelection());
    conf->setValue("viewing/constellation_art_intensity", cmgr->getArtIntensity());
    conf->setValue("viewing/flag_night", StelApp::getInstance().getVisionModeNight());
    conf->setValue("astro/flag_star_name", smgr->getFlagLabels());
    conf->setValue("stars/labels_amount", smgr->getLabelsAmount());
    conf->setValue("astro/flag_planets_labels", ssmgr->getFlagLabels());
    conf->setValue("astro/labels_amount", ssmgr->getLabelsAmount());
    conf->setValue("astro/nebula_hints_amount", nmgr->getHintsAmount());
    conf->setValue("astro/flag_nebula_name", nmgr->getFlagHints());
    conf->setValue("projection/type", core->getCurrentProjectionTypeKey());

    // view dialog / landscape tab settings
    lmgr->setDefaultLandscapeID(lmgr->getCurrentLandscapeID());
    conf->setValue("landscape/flag_landscape_sets_location", lmgr->getFlagLandscapeSetsLocation());
    conf->setValue("landscape/flag_landscape", lmgr->getFlagLandscape());
    conf->setValue("landscape/flag_atmosphere", lmgr->getFlagAtmosphere());
    conf->setValue("landscape/flag_brightness", lmgr->getFlagLandscapeNightBrightness());
    conf->setValue("landscape/flag_fog", lmgr->getFlagFog());
    conf->setValue("stars/init_bortle_scale", core->getSkyDrawer()->getBortleScale());
        conf->setValue("landscape/atmospheric_extinction_coefficient", core->getSkyDrawer()->getExtinctionCoefficient());
        conf->setValue("landscape/pressure_mbar", core->getSkyDrawer()->getAtmospherePressure());
        conf->setValue("landscape/temperature_C", core->getSkyDrawer()->getAtmosphereTemperature());

    // view dialog / starlore tab
    StelApp::getInstance().getSkyCultureMgr().setDefaultSkyCultureID(StelApp::getInstance().getSkyCultureMgr().getCurrentSkyCultureID());

    // Save default location
    StelApp::getInstance().getCore()->setDefaultLocationID(core->getCurrentLocation().getID());

    // configuration dialog / main tab
    QString langName = StelApp::getInstance().getLocaleMgr().getAppLanguage();
    conf->setValue("localization/app_locale", StelTranslator::nativeNameToIso639_1Code(langName));
    langName = StelApp::getInstance().getLocaleMgr().getSkyLanguage();
    conf->setValue("localization/sky_locale", StelTranslator::nativeNameToIso639_1Code(langName));

    // configuration dialog / selected object info tab
    const StelObject::InfoStringGroup& flags = infoTextFilters;
    if (flags == StelObject::InfoStringGroup(0))
        conf->setValue("gui/selected_object_info", "none");
    else if (flags == StelObject::InfoStringGroup(StelObject::ShortInfo))
        conf->setValue("gui/selected_object_info", "short");
    else if (flags == StelObject::InfoStringGroup(StelObject::AllInfo))
        conf->setValue("gui/selected_object_info", "all");
    else
    {
        conf->setValue("gui/selected_object_info", "custom");

        conf->beginGroup("custom_selected_info");
        conf->setValue("flag_show_name", (bool) (flags & StelObject::Name));
        conf->setValue("flag_show_catalognumber",
                       (bool) (flags & StelObject::CatalogNumber));
        conf->setValue("flag_show_magnitude",
                       (bool) (flags & StelObject::Magnitude));
        conf->setValue("flag_show_absolutemagnitude",
                       (bool) (flags & StelObject::AbsoluteMagnitude));
        conf->setValue("flag_show_radecj2000",
                       (bool) (flags & StelObject::RaDecJ2000));
        conf->setValue("flag_show_radecofdate",
                       (bool) (flags & StelObject::RaDecOfDate));
        conf->setValue("flag_show_hourangle",
                       (bool) (flags & StelObject::HourAngle));
        conf->setValue("flag_show_altaz",
                       (bool) (flags &  StelObject::AltAzi));
        conf->setValue("flag_show_distance",
                       (bool) (flags & StelObject::Distance));
        conf->setValue("flag_show_size",
                       (bool) (flags & StelObject::Size));
        conf->setValue("flag_show_extra1",
                       (bool) (flags & StelObject::Extra1));
        conf->setValue("flag_show_extra2",
                       (bool) (flags & StelObject::Extra2));
        conf->setValue("flag_show_extra3",
                       (bool) (flags & StelObject::Extra3));
        conf->setValue("flag_show_galcoordj2000",
                   (bool) (flags & StelObject::GalCoordJ2000));
        conf->endGroup();
    }

    // toolbar auto-hide status
    //conf->setValue("gui/auto_hide_horizontal_toolbar", gui->getAutoHideHorizontalButtonBar());
    //conf->setValue("gui/auto_hide_vertical_toolbar", gui->getAutoHideVerticalButtonBar());
    //conf->setValue("gui/flag_show_nebulae_background_button", gui->getFlagShowNebulaBackgroundButton());

    mvmgr->setInitFov(mvmgr->getCurrentFov());
    mvmgr->setInitViewDirectionToCurrent();

    // configuration dialog / navigation tab
    conf->setValue("navigation/flag_enable_zoom_keys", mvmgr->getFlagEnableZoomKeys());
    conf->setValue("navigation/flag_enable_mouse_navigation", mvmgr->getFlagEnableMouseNavigation());
    conf->setValue("navigation/flag_enable_move_keys", mvmgr->getFlagEnableMoveKeys());
    conf->setValue("navigation/startup_time_mode", core->getStartupTimeMode());
    conf->setValue("navigation/today_time", core->getInitTodayTime());
    conf->setValue("navigation/preset_sky_time", core->getPresetSkyTime());
    conf->setValue("navigation/time_correction_algorithm", core->getCurrentDeltaTAlgorithmKey());
    conf->setValue("navigation/init_fov", mvmgr->getInitFov());
    if (mvmgr->getMountMode() == StelMovementMgr::MountAltAzimuthal)
        conf->setValue("navigation/viewing_mode", "horizon");
    else
        conf->setValue("navigation/viewing_mode", "equator");


    // configuration dialog / tools tab
    //conf->setValue("gui/flag_show_flip_buttons", gui->getFlagShowFlipButtons());
    //conf->setValue("video/viewport_effect", StelMainGraphicsView::getInstance().getStelAppGraphicsWidget()->getViewportEffect());
    conf->setValue("projection/viewport", StelProjector::maskTypeToString(proj->getMaskType()));
    conf->setValue("viewing/flag_gravity_labels", proj->getFlagGravityLabels());
    conf->setValue("viewing/flag_render_solar_shadows", StelApp::getInstance().getRenderSolarShadows());
    conf->setValue("navigation/auto_zoom_out_resets_direction", mvmgr->getFlagAutoZoomOutResetsDirection());
    //conf->setValue("gui/flag_mouse_cursor_timeout", StelMainGraphicsView::getInstance().getFlagCursorTimeout());
    //conf->setValue("gui/mouse_cursor_timeout", StelMainGraphicsView::getInstance().getCursorTimeout());

    conf->setValue("main/screenshot_dir", StelFileMgr::getScreenshotDir());
    //conf->setValue("main/invert_screenshots_colors", StelMainGraphicsView::getInstance().getFlagInvertScreenShotColors());


    // save free look
    conf->setValue("projection/free_look", stelApp->getCore()->getFreeLook());

    // clear the restore defaults flag if it is set.
    conf->setValue("main/restore_defaults", false);

}
