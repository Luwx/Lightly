################# Qt/KDE #################
find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets DBus)
find_package(KF5 REQUIRED COMPONENTS
    I18n
    Config
    GuiAddons
    IconThemes
    ConfigWidgets
    WindowSystem)

find_package(Qt5 COMPONENTS Quick)
set(LIGHTLY_HAVE_QTQUICK ${Qt5Quick_FOUND})

find_package( KF5FrameworkIntegration CONFIG )
set_package_properties(KF5FrameworkIntegration PROPERTIES
    DESCRIPTION "KF5 Framework Integration"
    URL "https://projects.kde.org/projects/frameworks/frameworkintegration"
    TYPE OPTIONAL
    PURPOSE "Required to use KStyle convenience functionalities in style")

set(LIGHTLY_HAVE_KSTYLE ${KF5FrameworkIntegration_FOUND})

find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
    DESCRIPTION "X protocol C-language Binding"
    URL "https://xcb.freedesktop.org"
    TYPE OPTIONAL
    PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

find_package(KF5Wayland CONFIG)
set(LIGHTLY_HAVE_KWAYLAND ${KF5Wayland_FOUND})

if(UNIX AND NOT APPLE)
    set(LIGHTLY_HAVE_X11 ${XCB_XCB_FOUND})
    if (XCB_XCB_FOUND)
        find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
    endif()
else()
    set(LIGHTLY_HAVE_X11 FALSE)
endif()


################# includes #################
include_directories(
  animations
  debug
)

include_directories(${CMAKE_SOURCE_DIR}/liblightlycommon)
include_directories(${CMAKE_BINARY_DIR}/liblightlycommon)

################# configuration #################
configure_file(config-lightly.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-lightly.h )

########### next target ###############
set(lightly_PART_SRCS
    animations/lightlyanimation.cpp
    animations/lightlyanimations.cpp
    animations/lightlyanimationdata.cpp
    animations/lightlybaseengine.cpp
    animations/lightlybusyindicatordata.cpp
    animations/lightlybusyindicatorengine.cpp
    animations/lightlydialdata.cpp
    animations/lightlydialengine.cpp
    animations/lightlyenabledata.cpp
    animations/lightlygenericdata.cpp
    animations/lightlyheaderviewdata.cpp
    animations/lightlyheaderviewengine.cpp
    animations/lightlyscrollbardata.cpp
    animations/lightlyscrollbarengine.cpp
    animations/lightlyspinboxengine.cpp
    animations/lightlyspinboxdata.cpp
    animations/lightlystackedwidgetdata.cpp
    animations/lightlystackedwidgetengine.cpp
    animations/lightlytabbarengine.cpp
    animations/lightlytabbardata.cpp
    animations/lightlytoolboxengine.cpp
    animations/lightlytransitiondata.cpp
    animations/lightlytransitionwidget.cpp
    animations/lightlywidgetstateengine.cpp
    animations/lightlywidgetstatedata.cpp
    debug/lightlywidgetexplorer.cpp
    lightlyaddeventfilter.cpp
    lightlyblurhelper.cpp
    lightlyframeshadow.cpp
    lightlyhelper.cpp
    lightlymdiwindowshadow.cpp
    lightlymnemonics.cpp
    lightlypropertynames.cpp
    lightlyshadowhelper.cpp
    lightlysplitterproxy.cpp
    lightlystyle.cpp
    lightlystyleplugin.cpp
    lightlytileset.cpp
    lightlywindowmanager.cpp
)

kconfig_add_kcfg_files(lightly_PART_SRCS lightlystyleconfigdata.kcfgc)
add_library(lightly MODULE ${lightly_PART_SRCS})
target_link_libraries(lightly Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
if( LIGHTLY_HAVE_QTQUICK )
    target_link_libraries(lightly Qt5::Quick)
endif()
target_link_libraries(lightly KF5::ConfigCore KF5::ConfigWidgets KF5::GuiAddons KF5::IconThemes KF5::WindowSystem)
target_link_libraries(lightly lightlycommon5)

if(KF5FrameworkIntegration_FOUND)
    target_link_libraries(lightly KF5::Style)
endif()

if (WIN32)
    # As stated in https://docs.microsoft.com/en-us/cpp/c-runtime-library/math-constants M_PI only gets defined
    # when if _USE_MATH_DEFINES is defined
    target_compile_definitions(lightly PRIVATE _USE_MATH_DEFINES _BSD_SOURCE)
endif()

if(LIGHTLY_HAVE_X11)
    target_link_libraries(lightly ${XCB_LIBRARIES})
    target_link_libraries(lightly Qt5::X11Extras)
endif()

if(LIGHTLY_HAVE_KWAYLAND)
    target_link_libraries(lightly KF5::WaylandClient)
endif()


########### install files ###############
install(TARGETS lightly DESTINATION ${QT_PLUGIN_INSTALL_DIR}/styles/)
install(FILES lightly.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes)

########### subdirectories ###############
add_subdirectory(config)
