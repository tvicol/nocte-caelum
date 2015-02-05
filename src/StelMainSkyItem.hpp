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


#ifndef STELMAINSKYITEM_HPP
#define STELMAINSKYITEM_HPP

#include "StelObject.hpp"
#include "StelLocation.hpp"

#include <QQuickItem>

#include <QtGui/QOpenGLShaderProgram>
#include <QStringListModel>



#define BOOL_PROPERTY
class StelMainSkyItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal fps READ fps NOTIFY synced)
    Q_PROPERTY(qreal fov READ fov NOTIFY synced)
    Q_PROPERTY(int timerate READ timerate NOTIFY synced)
    Q_PROPERTY(QString prettyDatetime READ prettyDatetime NOTIFY synced)
    Q_PROPERTY(QString location READ location NOTIFY synced)

    Q_PROPERTY(qreal minFps MEMBER _minFps NOTIFY minFpsChanged)
    Q_PROPERTY(qreal maxFps MEMBER _maxFps NOTIFY maxFpsChanged)

    Q_PROPERTY(QObject* objectSearchResultsModel READ objectSearchResultsModel CONSTANT)
    Q_PROPERTY(QObject* locationSearchResultsModel READ locationSearchResultsModel CONSTANT)

    Q_PROPERTY(QString objectSearchString MEMBER _objectSearchString NOTIFY objectSearchStringChanged)
    Q_PROPERTY(QString locationSearchString MEMBER _locationSearchString NOTIFY locationSearchStringChanged)

    Q_PROPERTY(bool freeLook MEMBER _freeLook NOTIFY freeLookChanged)
    Q_PROPERTY(int orientation MEMBER _orientation NOTIFY orientationChanged)
    Q_PROPERTY(bool constLines MEMBER _constLines NOTIFY constLinesChanged)
    Q_PROPERTY(bool constLabels MEMBER _constLabels NOTIFY constLabelsChanged)
    Q_PROPERTY(bool constBoundaries MEMBER _constBoundaries NOTIFY constBoundariesChanged)
    Q_PROPERTY(bool constArt MEMBER _constArt NOTIFY constArtChanged)
    Q_PROPERTY(bool equatorialGrid MEMBER _equatorialGrid NOTIFY equatorialGridChanged)
    Q_PROPERTY(bool azimuthalGrid MEMBER _azimuthalGrid NOTIFY azimuthalGridChanged)
    Q_PROPERTY(bool galacticGrid MEMBER _galacticGrid NOTIFY galacticGridChanged)
    Q_PROPERTY(bool meridianLine MEMBER _meridianLine NOTIFY meridianLineChanged)
    Q_PROPERTY(bool ecliptic MEMBER _ecliptic NOTIFY eclipticChanged)
    Q_PROPERTY(bool ground MEMBER _ground NOTIFY groundChanged)
    Q_PROPERTY(bool fog MEMBER _fog NOTIFY fogChanged)
    Q_PROPERTY(bool atmosphere MEMBER _atmosphere NOTIFY atmosphereChanged)
    Q_PROPERTY(bool cardinalPoints MEMBER _cardinalPoints NOTIFY cardinalPointsChanged)

    Q_PROPERTY(bool starLabels MEMBER _starLabels NOTIFY starLabelsChanged)
    Q_PROPERTY(bool planetLabels MEMBER _planetLabels NOTIFY planetLabelsChanged)
    Q_PROPERTY(bool nebulaLabels MEMBER _nebulaLabels NOTIFY nebulaLabelsChanged)
    Q_PROPERTY(qreal starLabelsAmount MEMBER _starLabelsAmount NOTIFY starLabelsAmountChanged)
    Q_PROPERTY(qreal planetLabelsAmount MEMBER _planetLabelsAmount NOTIFY planetLabelsAmountChanged)
    Q_PROPERTY(qreal nebulaLabelsAmount MEMBER _nebulaLabelsAmount NOTIFY nebulaLabelsAmountChanged)

    Q_PROPERTY(bool nightMode MEMBER _nightMode NOTIFY nightModeChanged)
    Q_PROPERTY(qreal extinction MEMBER _extinction NOTIFY extinctionChanged)

    Q_PROPERTY(QString nameText MEMBER _nameText NOTIFY nameTextChanged)
    Q_PROPERTY(QString nameColor MEMBER _nameColor NOTIFY nameColorChanged)
    Q_PROPERTY(QString infoText MEMBER _infoText NOTIFY infoTextChanged)
    Q_PROPERTY(QString infoColor MEMBER _infoColor NOTIFY infoColorChanged)

    Q_PROPERTY(QString locationName MEMBER _locationName NOTIFY locationNameChanged)
    Q_PROPERTY(QString locationCountry MEMBER _locationCountry NOTIFY locationCountryChanged)
    Q_PROPERTY(QString locationPlanet MEMBER _locationPlanet NOTIFY locationPlanetChanged)
    Q_PROPERTY(qreal locationLatitude MEMBER _locationLatitude NOTIFY locationLatitudeChanged)
    Q_PROPERTY(qreal locationLongitude MEMBER _locationLongitude NOTIFY locationLongitudeChanged)
    Q_PROPERTY(qreal locationAltitude MEMBER _locationAltitude NOTIFY locationAltitudeChanged)
    Q_PROPERTY(QString terrainImageSource MEMBER _terrainImageSource NOTIFY terrainImageSourceChanged)
    Q_PROPERTY(bool locationCanBeDeleted MEMBER _locationCanBeDeleted NOTIFY locationCanBeDeletedChanged)
    Q_PROPERTY(bool locationIsNew MEMBER _locationIsNew NOTIFY locationIsNewChanged)

    Q_PROPERTY(int skipNumFrames MEMBER _skipNumFrames NOTIFY skipNumFramesChanged)

    Q_PROPERTY(bool dynamicEyeAdaptation MEMBER _dynamicEyeAdaptation NOTIFY dynamicEyeAdaptationChanged)
    Q_PROPERTY(double atmospherePressure MEMBER _atmospherePressure NOTIFY atmospherePressureChanged)
    Q_PROPERTY(double atmosphereTemp MEMBER _atmosphereTemp NOTIFY atmosphereTempChanged)
    Q_PROPERTY(int lightPollution MEMBER _lightPollution NOTIFY lightPollutionChanged)
    Q_PROPERTY(double starAbsScale MEMBER _starAbsScale NOTIFY starAbsScaleChanged)
    Q_PROPERTY(double starRelScale MEMBER _starRelScale NOTIFY starRelScaleChanged)
    Q_PROPERTY(double twinkleAmount MEMBER _twinkleAmount NOTIFY twinkleAmountChanged)
    Q_PROPERTY(int meteorZHR MEMBER _meteorZHR NOTIFY meteorZHRChanged)

public:
    explicit StelMainSkyItem(QQuickItem *parent = 0);
    virtual ~StelMainSkyItem();

    qreal fps() const { return _fps; }
    int timerate() const { return _timerate; }
    qreal fov() const { return _fov; }
    QString prettyDatetime() const { return _prettyDatetime; }
    QString location() const { return _location; }


    Q_INVOKABLE void gotoNamedObject(const QString& text, bool recheck);
    Q_INVOKABLE void selectNamedLocation(const QString& text, bool recheck);

    Q_INVOKABLE void setTimeToNow() { emit timeSetToNow(); }
    Q_INVOKABLE void toggleRealtimeSpeed() { emit realtimeSpeedToggled(); }
    Q_INVOKABLE void increaseTimeSpeed() { emit timeSpeedIncreased(); }
    Q_INVOKABLE void decreaseTimeSpeed() { emit timeSpeedDecreased(); }
    Q_INVOKABLE void centerSelection() { emit selectionCentered(true); }
    Q_INVOKABLE void clearSelection() { emit selectionCleared(); }

    Q_INVOKABLE void updateObjectSearchResults() { _updateObjectSearchResults = true; }
    Q_INVOKABLE void updateLocationSearchResults() { _updateLocationSearchResults = true; }
    Q_INVOKABLE void addLocationToList() { _addLocationToList = true; }
    Q_INVOKABLE void removeLocationFromList() { _removeLocationFromList = true; }
    Q_INVOKABLE void applyCurrentLocation() { _applyCurrentLocation = true; }
    Q_INVOKABLE void saveSettings() { _saveSettings = true; }


    QStringListModel* objectSearchResultsModel() { return _objectSearchResultsModel; }
    QStringListModel* locationSearchResultsModel() { return _locationSearchResultsModel; }

signals:

    void synced();
    void loaded();

    void gotoCompleted(bool);
    void locationFound();

    void minFpsChanged(qreal);
    void maxFpsChanged(qreal);
    void freeLookChanged(bool);
    void orientationChanged(int);
    void objectSearchStringChanged(QString);
    void objectSearchResultsUpdated(const QStringList&);
    void locationSearchStringChanged(QString);
    void locationSearchResultsUpdated(const QStringList&);
    void constLinesChanged(bool);
    void constLabelsChanged(bool);
    void constBoundariesChanged(bool);
    void constArtChanged(bool);
    void equatorialGridChanged(bool);
    void azimuthalGridChanged(bool);
    void galacticGridChanged(bool);
    void meridianLineChanged(bool);
    void eclipticChanged(bool);
    void groundChanged(bool);
    void fogChanged(bool);
    void atmosphereChanged(bool);
    void cardinalPointsChanged(bool);
    void starLabelsChanged(bool);
    void planetLabelsChanged(bool);
    void nebulaLabelsChanged(bool);
    void starLabelsAmountChanged(qreal);
    void planetLabelsAmountChanged(qreal);
    void nebulaLabelsAmountChanged(qreal);
    void nightModeChanged(bool);
    void extinctionChanged(qreal);

    void nameTextChanged(QString);
    void nameColorChanged(QString);
    void infoTextChanged(QString);
    void infoColorChanged(QString);

    void locationNameChanged(QString);
    void locationCountryChanged(QString);
    void locationPlanetChanged(QString);
    void locationLatitudeChanged(qreal);
    void locationLongitudeChanged(qreal);
    void locationAltitudeChanged(qreal);
    void terrainImageSourceChanged(QString);
    void locationCanBeDeletedChanged(bool);
    void locationIsNewChanged(bool);
    void skipNumFramesChanged(int);
    void dynamicEyeAdaptationChanged(bool);
    void atmospherePressureChanged(qreal);
    void atmosphereTempChanged(double);
    void lightPollutionChanged(int);
    void starAbsScaleChanged(double);
    void starRelScaleChanged(double);
    void twinkleAmountChanged(double);
    void meteorZHRChanged(int);


    void timeSetToNow();
    void timeSpeedIncreased();
    void timeSpeedDecreased();
    void realtimeSpeedToggled();
    void selectionCentered(bool);
    void selectionCleared();

protected:
    virtual void itemChange(ItemChange change, const ItemChangeData &);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);


    friend class StelMainSkyItemRenderClient;
    bool paintPartial();
    void thereWasAnEvent();

public slots:
    void paint();
    void cleanup();
    void sync();

    void setMinFps(qreal v) { _minFps = v; minFpsChanged(v); }
    void setMaxFps(qreal v) { _maxFps = v; maxFpsChanged(v); }
    void setFreeLook(bool v) { _freeLook = v; freeLookChanged(v); }
    void setOrientation(int v) { _orientation = v; orientationChanged(v); }
    void setObjectSearchResults(const QStringList& l) { _objectSearchResultsModel->setStringList(l); }
    void setLocationSearchResults(const QStringList& l) { _locationSearchResultsModel->setStringList(l); }
    void setConstLines(bool v) { _constLines = v; constLinesChanged(v); }
    void setConstLabels(bool v) { _constLabels = v; constLabelsChanged(v); }
    void setConstBoundaries(bool v) { _constBoundaries = v; constBoundariesChanged(v); }
    void setConstArt(bool v) { _constArt = v; constArtChanged(v); }

    void setEquatorialGrid(bool v) { _equatorialGrid = v; equatorialGridChanged(v);}
    void setAzimuthalGrid(bool v) { _azimuthalGrid = v; azimuthalGridChanged(v);}
    void setGalacticGrid(bool v) { _galacticGrid = v; galacticGridChanged(v);}

    void setMeridianLine(bool v) { _meridianLine = v; meridianLineChanged(v);}
    void setEcliptic(bool v) { _ecliptic = v; eclipticChanged(v);}

    void setGround(bool v) { _ground = v; groundChanged(v);}
    void setFog(bool v) { _fog = v; fogChanged(v);}
    void setAtmosphere(bool v) { _atmosphere = v; atmosphereChanged(v);}
    void setCardinalPoints(bool v) { _cardinalPoints = v; cardinalPointsChanged(v);}

    void setStarLabels(bool v) { _starLabels = v; starLabelsChanged(v);}
    void setPlanetLabels(bool v) { _planetLabels = v; planetLabelsChanged(v);}
    void setNebulaLabels(bool v) { _nebulaLabels = v; nebulaLabelsChanged(v);}
    void setStarLabelsAmount(qreal v) { _starLabelsAmount = v; starLabelsAmountChanged(v);}
    void setPlanetLabelsAmount(qreal v) { _planetLabelsAmount = v; planetLabelsAmountChanged(v);}
    void setNebulaLabelsAmount(qreal v) { _nebulaLabelsAmount = v; nebulaLabelsAmountChanged(v);}
    void setExtinction(qreal v) { _extinction = v; extinctionChanged(v);}

    void setNameText(const QString& v) { _nameText = v; nameTextChanged(v); }
    void setNameColor(const QString& v) { _nameColor = v; nameColorChanged(v); }
    void setInfoText(const QString& v) { _infoText = v; infoTextChanged(v); }
    void setInfoColor(const QString& v) { _infoColor = v; infoColorChanged(v); }

    void setLocationName(const QString& v) { _locationName = v; locationNameChanged(v); }
    void setLocationCountry(const QString& v) { _locationCountry = v; locationCountryChanged(v); }
    void setLocationPlanet(const QString& v) { _locationPlanet = v; locationPlanetChanged(v); }
    void setLocationLatitude(qreal v) { _locationLatitude = v; locationLatitudeChanged(v); }
    void setLocationLongitude(qreal v) { _locationLongitude = v; locationLongitudeChanged(v); }
    void setLocationAltitude(qreal v) { _locationAltitude = v; locationAltitudeChanged(v); }
    void setTerrainImageSource(const QString& v) { _terrainImageSource = v; terrainImageSourceChanged(v); }
    void setLocationCanBeDeleted(bool v) { _locationCanBeDeleted = v; locationCanBeDeletedChanged(v); }
    void setLocationIsNew(bool v) { _locationIsNew = v; locationIsNewChanged(v); }
    void setSkipNumFrames(bool v) { _skipNumFrames = v; skipNumFramesChanged(v); }
    void setDynamicEyeAdaptation(bool v) { _dynamicEyeAdaptation = v; dynamicEyeAdaptationChanged(v); }
    void setAtmospherePressure(double v) { _atmospherePressure = v; atmospherePressureChanged(v); }
    void setAtmosphereTemp(double v) { _atmosphereTemp = v; atmosphereTempChanged(v); }
    void setLightPollution(int v) { _lightPollution = v; lightPollutionChanged(v); }
    void setStarAbsScale(double v) { _starAbsScale = v; starAbsScaleChanged(v); }
    void setStarRelScale(double v) { _starRelScale = v; starRelScaleChanged(v); }
    void setTwinkleAmount(double v) { _twinkleAmount = v; twinkleAmountChanged(v); }
    void setMeteorZHR(int v) { _meteorZHR = v; meteorZHRChanged(v); }

    void updateInfoText();



private:

    void flipMousePos(QPointF& pos);
    void initConnections();
    void loadInfoTextFiltersConfig();
    StelLocation getCurrentLocation();
    void setCurrentLocation(const StelLocation&);
    void saveCurrentViewOptions();


    double previousPaintTime;

    class StelSearch* stelSearch;
    class StelApp* stelApp;
    class StelQGL2Renderer* renderer;
    class QOpenGLPaintDevice* paintDevice;
    int paintState;
    class QTimer* minFpsTimer;
    bool firstFrame;
    double lastEventTimeSec;
    double previousFrameTime;
    unsigned int frameNo;
    StelObject::InfoStringGroup infoTextFilters;

    qreal _fps;
    qreal _minFps; // TODO: remove, obsolete
    qreal _maxFps; // TODO: remove, obsolete
    int _timerate;
    qreal _fov;
    QString _prettyDatetime;
    QString _location;

    QString _objectSearchString;
    QString _locationSearchString;
    bool _updateObjectSearchResults;
    bool _updateLocationSearchResults;
    bool _addLocationToList;
    bool _removeLocationFromList;
    bool _applyCurrentLocation;

    bool _freeLook;
    int _orientation;
    bool _constLines;
    bool _constLabels;
    bool _constBoundaries;
    bool _constArt;
    bool _equatorialGrid;
    bool _azimuthalGrid;
    bool _galacticGrid;
    bool _meridianLine;
    bool _ecliptic;
    bool _ground;
    bool _fog;
    bool _atmosphere;
    bool _cardinalPoints;
    bool _starLabels;
    bool _nebulaLabels;
    bool _planetLabels;
    qreal _starLabelsAmount;
    qreal _nebulaLabelsAmount;
    qreal _planetLabelsAmount;
    QStringListModel* _objectSearchResultsModel;
    QStringListModel* _locationSearchResultsModel;
    bool _gotoObject;
    QString _gotoName;
    bool _gotoRecheck;
    bool _nightMode;
    qreal _extinction;
    QString _nameText;
    QString _nameColor;
    QString _infoText;
    QString _infoColor;
    QString _locationName;
    QString _locationCountry;
    QString _locationPlanet;
    qreal _locationLatitude;
    qreal _locationLongitude;
    qreal _locationAltitude;
    QString _terrainImageSource;
    bool _locationCanBeDeleted;
    bool _locationIsNew;
    bool _selectNamedLocation;
    QString _selectLocationName;
    bool _selectLocationRecheck;
    bool _saveSettings;
    int _skipNumFrames;
    bool _dynamicEyeAdaptation;

    double _atmospherePressure;
    double _atmosphereTemp;
    int _lightPollution;
    double _starAbsScale;
    double _starRelScale;
    double _twinkleAmount;
    int _meteorZHR;

    class StelSensors* sensors;

};

#endif // STELMAINSKYITEM_HPP


