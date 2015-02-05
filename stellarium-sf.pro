# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = stellarium-sf

CONFIG += sailfishapp
QT += core gui opengl network widgets sensors svg location

LIBS += -lz

#installss

data1.path = /usr/share/stellarium-sf/data
data1.files = stellarium-0.12.4-base/data/ssystem.ini \
    stellarium-0.12.4-base/data/default_config.ini \
    stellarium-0.12.4-base/data/iso639-1.utf8 \
    stellarium-0.12.4-base/data/stellarium.ico \
    stellarium-0.12.4-base/data/base_locations.bin.gz \
    stellarium-0.12.4-base/data/DejaVuSans.ttf \
    stellarium-0.12.4-base/data/DejaVuSansMono.ttf \
    stellarium-0.12.4-base/data/constellations_boundaries.dat \
    stellarium-0.12.4-base/data/iso639-1.utf8 \
    stellarium-0.12.4-base/data/iso3166-1-alpha-2.utf8 \
    stellarium-0.12.4-base/data/countryCodes.dat

data2.path = /usr/share/stellarium-sf
data2.files = stellarium-0.12.4-base/data/shaders \
    stellarium-0.12.4-base/textures \
    stellarium-0.12.4-base/landscapes \
    stellarium-0.12.4-base/skycultures \
    stellarium-0.12.4-base/nebulae \
    stellarium-0.12.4-base/data/gui \
    locale \

data3.path = /usr/share/stellarium-sf/stars
data3.files = stellarium-0.12.4-base/stars/default

data4.path = /usr/share/stellarium-sf/qml
data4.files = stellarium-n9256.png


INSTALLS += data1 data2 data3 data4


DEFINES += PACKAGE_VERSION=\\\"0.9\\\"
DEFINES += USE_OPENGL_ES2
DEFINES += STELSF_VERSION=\\\"0.4.0\\\"
DEFINES += ENABLE_NLS
DEFINES += DISABLE_SCRIPTING

INCLUDEPATH += stellarium-0.12.4-base/src \
    stellarium-0.12.4-base/src/core \
    stellarium-0.12.4-base/src/core/external \
    stellarium-0.12.4-base/src/core/external/kfilter \
    stellarium-0.12.4-base/src/core/external/glues_stel/source \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess \
    stellarium-0.12.4-base/src/core/modules \
    stellarium-0.12.4-base/src/core/planetsephems \
    stellarium-0.12.4-base/src/core/renderer \
    stellarium-0.12.4-base/src/gui \
    stellarium-0.12.4-base/src/scripting \
    stellarium-0.12.4-base/src/ \
    src




SOURCES += \
    src/StelMainSkyItem.cpp \
    stellarium-0.12.4-base/src/StelLogger.cpp \
    stellarium-0.12.4-base/src/CLIProcessor.cpp \
    stellarium-0.12.4-base/src/core/TrailGroup.cpp \
    stellarium-0.12.4-base/src/core/StelViewportEffect.cpp \
    stellarium-0.12.4-base/src/core/StelUtils.cpp \
    stellarium-0.12.4-base/src/core/StelTranslator.cpp \
    stellarium-0.12.4-base/src/core/StelToneReproducer.cpp \
    stellarium-0.12.4-base/src/core/StelSphericalIndexMultiRes.cpp \
    stellarium-0.12.4-base/src/core/StelSphericalIndex.cpp \
    stellarium-0.12.4-base/src/core/StelSphereGeometry.cpp \
    stellarium-0.12.4-base/src/core/StelSkyPolygon.cpp \
    stellarium-0.12.4-base/src/core/StelSkyLayerMgr.cpp \
    stellarium-0.12.4-base/src/core/StelSkyLayer.cpp \
    stellarium-0.12.4-base/src/core/StelSkyImageTile.cpp \
    stellarium-0.12.4-base/src/core/StelSkyDrawer.cpp \
    stellarium-0.12.4-base/src/core/StelSkyCultureMgr.cpp \
    stellarium-0.12.4-base/src/core/StelProjectorClasses.cpp \
    stellarium-0.12.4-base/src/core/StelProjector.cpp \
    stellarium-0.12.4-base/src/core/StelObserver.cpp \
    stellarium-0.12.4-base/src/core/StelObjectModule.cpp \
    stellarium-0.12.4-base/src/core/StelObjectMgr.cpp \
    stellarium-0.12.4-base/src/core/StelObject.cpp \
    stellarium-0.12.4-base/src/core/StelMovementMgr.cpp \
    stellarium-0.12.4-base/src/core/StelModuleMgr.cpp \
    stellarium-0.12.4-base/src/core/StelModule.cpp \
    stellarium-0.12.4-base/src/core/StelLocationMgr.cpp \
    stellarium-0.12.4-base/src/core/StelLocation.cpp \
    stellarium-0.12.4-base/src/core/StelLocaleMgr.cpp \
    stellarium-0.12.4-base/src/core/StelLoadingBar.cpp \
    stellarium-0.12.4-base/src/core/StelJsonParser.cpp \
    stellarium-0.12.4-base/src/core/StelIniParser.cpp \
    stellarium-0.12.4-base/src/core/StelGuiBase.cpp \
    stellarium-0.12.4-base/src/core/StelGeodesicGrid.cpp \
    stellarium-0.12.4-base/src/core/StelFileMgr.cpp \
    stellarium-0.12.4-base/src/core/StelCore.cpp \
    stellarium-0.12.4-base/src/core/StelAudioMgr.cpp \
    stellarium-0.12.4-base/src/core/StelAppGraphicsWidget.cpp \
    stellarium-0.12.4-base/src/core/StelApp.cpp \
    stellarium-0.12.4-base/src/core/SphericMirrorCalculator.cpp \
    stellarium-0.12.4-base/src/core/SimbadSearcher.cpp \
    stellarium-0.12.4-base/src/core/RefractionExtinction.cpp \
    stellarium-0.12.4-base/src/core/OctahedronPolygon.cpp \
    stellarium-0.12.4-base/src/core/MultiLevelJsonBase.cpp \
    stellarium-0.12.4-base/src/core/modules/ZoneArray.cpp \
    stellarium-0.12.4-base/src/core/modules/StarWrapper.cpp \
    stellarium-0.12.4-base/src/core/modules/StarMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/Star.cpp \
    stellarium-0.12.4-base/src/core/modules/SolarSystem.cpp \
    stellarium-0.12.4-base/src/core/modules/Skylight.cpp \
    stellarium-0.12.4-base/src/core/modules/Skybright.cpp \
    stellarium-0.12.4-base/src/core/modules/Planet.cpp \
    stellarium-0.12.4-base/src/core/modules/Orbit.cpp \
    stellarium-0.12.4-base/src/core/modules/NebulaMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/Nebula.cpp \
    stellarium-0.12.4-base/src/core/modules/MinorPlanet.cpp \
    stellarium-0.12.4-base/src/core/modules/MilkyWay.cpp \
    stellarium-0.12.4-base/src/core/modules/MeteorMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/Meteor.cpp \
    stellarium-0.12.4-base/src/core/modules/LandscapeMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/Landscape.cpp \
    stellarium-0.12.4-base/src/core/modules/LabelMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/GridLinesMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/ConstellationMgr.cpp \
    stellarium-0.12.4-base/src/core/modules/Constellation.cpp \
    stellarium-0.12.4-base/src/core/modules/Comet.cpp \
    stellarium-0.12.4-base/src/core/modules/Atmosphere.cpp \
    stellarium-0.12.4-base/src/core/planetsephems/vsop87.c \
    stellarium-0.12.4-base/src/core/planetsephems/tass17.c \
    stellarium-0.12.4-base/src/core/planetsephems/stellplanet.c \
    stellarium-0.12.4-base/src/core/planetsephems/sideral_time.c \
    stellarium-0.12.4-base/src/core/planetsephems/pluto.c \
    stellarium-0.12.4-base/src/core/planetsephems/marssat.c \
    stellarium-0.12.4-base/src/core/planetsephems/l1.c \
    stellarium-0.12.4-base/src/core/planetsephems/gust86.c \
    stellarium-0.12.4-base/src/core/planetsephems/elp82b.c \
    stellarium-0.12.4-base/src/core/planetsephems/elliptic_to_rectangular.c \
    stellarium-0.12.4-base/src/core/planetsephems/calc_interpolated_elements.c \
    stellarium-0.12.4-base/src/core/renderer/StelTextureNew.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelRenderer.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLViewport.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLTextureBackend.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLRenderer.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLInterleavedArrayVertexBufferBackend.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLGLSLShader.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLArrayVertexBufferBackend.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGL2InterleavedArrayVertexBufferBackend.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGL2ArrayVertexBufferBackend.cpp \
    stellarium-0.12.4-base/src/core/renderer/StelGeometryBuilder.cpp \
    stellarium-0.12.4-base/src/core/StelVideoMgr.cpp \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/glues_error.c \
    stellarium-0.12.4-base/src/core/renderer/StelGLUtilityFunctions.cpp \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/tessmono.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/tess.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/sweep.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/render.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/priorityq.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/normal.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/mesh.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/memalloc.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/geom.c \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/dict.c \
    stellarium-0.12.4-base/src/core/external/kfilter/kzip.cpp \
    stellarium-0.12.4-base/src/core/external/kfilter/klimitediodevice.cpp \
    stellarium-0.12.4-base/src/core/external/kfilter/kgzipfilter.cpp \
    stellarium-0.12.4-base/src/core/external/kfilter/kfilterdev.cpp \
    stellarium-0.12.4-base/src/core/external/kfilter/kfilterbase.cpp \
    stellarium-0.12.4-base/src/core/external/kfilter/karchive.cpp \
    stellarium-0.12.4-base/src/core/StelShortcutGroup.cpp \
    stellarium-0.12.4-base/src/core/StelShortcutMgr.cpp \
    stellarium-0.12.4-base/src/main.cpp \
    src/StelSearch.cpp \
    src/StelSensors.cpp


OTHER_FILES += qml/stellarium-sf.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    stellarium-sf.desktop \
    qml/pages/TestPage.qml \
    stellarium-0.12.4-base/data/ssystem.ini \
    qml/pages/Hud.qml \
    qml/pages/QuickSettings.qml \
    qml/pages/QuickSettingsGridItem.qml \
    qml/pages/DimmedPage.qml \
    qml/pages/Labels.qml \
    rpm/stellarium-sf.yaml \
    rpm/stellarium-sf.spec \
    qml/pages/AtmosphereSettings.qml \
    qml/pages/LocationSettings.qml \
    qml/pages/ListRow.qml \
    qml/pages/LocationSearch.qml \
    qml/pages/ObjectSearch.qml \
    qml/pages/SearchPage.qml \
    qml/pages/StarSettings.qml \
    qml/pages/AboutPage.qml \
    qml/pages/DimmedBackground.qml \
    stellarium-n986.png \
    stellarium-sf.svg \
    stellarium-sf.png


HEADERS += \
    src/StelMainSkyItem.hpp \
    stellarium-0.12.4-base/src/StelLogger.hpp \
    stellarium-0.12.4-base/src/CLIProcessor.hpp \
    stellarium-0.12.4-base/src/translations.h \
    stellarium-0.12.4-base/src/core/VecMath.hpp \
    stellarium-0.12.4-base/src/core/Triplet.hpp \
    stellarium-0.12.4-base/src/core/TriangleIterator.hpp \
    stellarium-0.12.4-base/src/core/TrailGroup.hpp \
    stellarium-0.12.4-base/src/core/StelViewportEffect.hpp \
    stellarium-0.12.4-base/src/core/StelUtils.hpp \
    stellarium-0.12.4-base/src/core/StelTranslator.hpp \
    stellarium-0.12.4-base/src/core/StelToneReproducer.hpp \
    stellarium-0.12.4-base/src/core/StelSphericalIndexMultiRes.hpp \
    stellarium-0.12.4-base/src/core/StelSphericalIndex.hpp \
    stellarium-0.12.4-base/src/core/StelSphereGeometry.hpp \
    stellarium-0.12.4-base/src/core/StelSkyPolygon.hpp \
    stellarium-0.12.4-base/src/core/StelSkyLayerMgr.hpp \
    stellarium-0.12.4-base/src/core/StelSkyLayer.hpp \
    stellarium-0.12.4-base/src/core/StelSkyImageTile.hpp \
    stellarium-0.12.4-base/src/core/StelSkyDrawer.hpp \
    stellarium-0.12.4-base/src/core/StelSkyCultureMgr.hpp \
    stellarium-0.12.4-base/src/core/StelRegionObject.hpp \
    stellarium-0.12.4-base/src/core/StelProjectorType.hpp \
    stellarium-0.12.4-base/src/core/StelProjectorClasses.hpp \
    stellarium-0.12.4-base/src/core/StelProjector.hpp \
    stellarium-0.12.4-base/src/core/StelPluginInterface.hpp \
    stellarium-0.12.4-base/src/core/StelObserver.hpp \
    stellarium-0.12.4-base/src/core/StelObjectType.hpp \
    stellarium-0.12.4-base/src/core/StelObjectModule.hpp \
    stellarium-0.12.4-base/src/core/StelObjectMgr.hpp \
    stellarium-0.12.4-base/src/core/StelObject.hpp \
    stellarium-0.12.4-base/src/core/StelMovementMgr.hpp \
    stellarium-0.12.4-base/src/core/StelModuleMgr.hpp \
    stellarium-0.12.4-base/src/core/StelModule.hpp \
    stellarium-0.12.4-base/src/core/StelLocationMgr.hpp \
    stellarium-0.12.4-base/src/core/StelLocation.hpp \
    stellarium-0.12.4-base/src/core/StelLocaleMgr.hpp \
    stellarium-0.12.4-base/src/core/StelLoadingBar.hpp \
    stellarium-0.12.4-base/src/core/StelJsonParser.hpp \
    stellarium-0.12.4-base/src/core/StelIniParser.hpp \
    stellarium-0.12.4-base/src/core/StelGuiBase.hpp \
    stellarium-0.12.4-base/src/core/StelGeodesicGrid.hpp \
    stellarium-0.12.4-base/src/core/StelFileMgr.hpp \
    stellarium-0.12.4-base/src/core/StelFader.hpp \
    stellarium-0.12.4-base/src/core/StelCore.hpp \
    stellarium-0.12.4-base/src/core/StelAudioMgr.hpp \
    stellarium-0.12.4-base/src/core/StelAppGraphicsWidget.hpp \
    stellarium-0.12.4-base/src/core/StelApp.hpp \
    stellarium-0.12.4-base/src/core/SphericMirrorCalculator.hpp \
    stellarium-0.12.4-base/src/core/SimbadSearcher.hpp \
    stellarium-0.12.4-base/src/core/RefractionExtinction.hpp \
    stellarium-0.12.4-base/src/core/OctahedronPolygon.hpp \
    stellarium-0.12.4-base/src/core/MultiLevelJsonBase.hpp \
    stellarium-0.12.4-base/src/core/modules/ZoneData.hpp \
    stellarium-0.12.4-base/src/core/modules/ZoneArray.hpp \
    stellarium-0.12.4-base/src/core/modules/StarWrapper.hpp \
    stellarium-0.12.4-base/src/core/modules/StarMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/Star.hpp \
    stellarium-0.12.4-base/src/core/modules/Solve.hpp \
    stellarium-0.12.4-base/src/core/modules/SolarSystem.hpp \
    stellarium-0.12.4-base/src/core/modules/Skylight.hpp \
    stellarium-0.12.4-base/src/core/modules/Skybright.hpp \
    stellarium-0.12.4-base/src/core/modules/Planet.hpp \
    stellarium-0.12.4-base/src/core/modules/Orbit.hpp \
    stellarium-0.12.4-base/src/core/modules/NebulaMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/Nebula.hpp \
    stellarium-0.12.4-base/src/core/modules/MinorPlanet.hpp \
    stellarium-0.12.4-base/src/core/modules/MilkyWay.hpp \
    stellarium-0.12.4-base/src/core/modules/MeteorMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/Meteor.hpp \
    stellarium-0.12.4-base/src/core/modules/LandscapeMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/Landscape.hpp \
    stellarium-0.12.4-base/src/core/modules/LabelMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/GridLinesMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/ConstellationMgr.hpp \
    stellarium-0.12.4-base/src/core/modules/Constellation.hpp \
    stellarium-0.12.4-base/src/core/modules/Comet.hpp \
    stellarium-0.12.4-base/src/core/modules/Atmosphere.hpp \
    stellarium-0.12.4-base/src/core/planetsephems/vsop87.h \
    stellarium-0.12.4-base/src/core/planetsephems/tass17.h \
    stellarium-0.12.4-base/src/core/planetsephems/stellplanet.h \
    stellarium-0.12.4-base/src/core/planetsephems/sideral_time.h \
    stellarium-0.12.4-base/src/core/planetsephems/pluto.h \
    stellarium-0.12.4-base/src/core/planetsephems/marssat.h \
    stellarium-0.12.4-base/src/core/planetsephems/l1.h \
    stellarium-0.12.4-base/src/core/planetsephems/gust86.h \
    stellarium-0.12.4-base/src/core/planetsephems/elp82b.h \
    stellarium-0.12.4-base/src/core/planetsephems/elliptic_to_rectangular.h \
    stellarium-0.12.4-base/src/core/planetsephems/calc_interpolated_elements.h \
    stellarium-0.12.4-base/src/core/renderer/StelVertexBufferBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelVertexBuffer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelVertexAttribute.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelTextureParams.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelTextureNew.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelTextureLoader.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelTextureCache.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelTextureBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelRendererStatistics.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelRenderer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLViewport.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLTextureBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLRenderer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLInterleavedArrayVertexBufferBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLIndexBuffer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLGLSLShader.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGLArrayVertexBufferBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGL2Renderer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGL2InterleavedArrayVertexBufferBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelQGL2ArrayVertexBufferBackend.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelLight.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelIndexBuffer.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelGLSLShader.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelGLCompatibility.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelGeometryBuilder.hpp \
    stellarium-0.12.4-base/src/core/renderer/StelCircleArcRenderer.hpp \
    stellarium-0.12.4-base/src/core/renderer/GenericVertexTypes.hpp \
    stellarium-0.12.4-base/src/core/StelVideoMgr.hpp \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/glues.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/glues_error.h \
    stellarium-0.12.4-base/src/core/renderer/StelGLUtilityFunctions.hpp \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/tessmono.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/tess.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/sweep.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/render.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/priorityq.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/priorityq-sort.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/priorityq-heap.i \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/priorityq-heap.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/normal.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/mesh.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/memalloc.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/geom.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/dict.h \
    stellarium-0.12.4-base/src/core/external/glues_stel/source/libtess/dict-list.h \
    stellarium-0.12.4-base/src/core/external/kfilter/kzip.h \
    stellarium-0.12.4-base/src/core/external/kfilter/klimitediodevice.h \
    stellarium-0.12.4-base/src/core/external/kfilter/kgzipfilter.h \
    stellarium-0.12.4-base/src/core/external/kfilter/kfilterdev.h \
    stellarium-0.12.4-base/src/core/external/kfilter/kfilterbase.h \
    stellarium-0.12.4-base/src/core/external/kfilter/karchive.h \
    stellarium-0.12.4-base/src/core/StelShortcutMgr.hpp \
    stellarium-0.12.4-base/src/core/StelShortcutGroup.hpp \
    src/StelSearch.hpp \
    src/StelQuaternion.hpp \
    src/StelSensors.hpp

QMAKE_CXXFLAGS_RELEASE += -ftree-vectorize -funroll-loops -ffast-math
