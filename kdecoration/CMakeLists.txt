add_definitions(-DTRANSLATION_DOMAIN="lightly_kwin_deco")

find_package(KF5 REQUIRED COMPONENTS CoreAddons GuiAddons ConfigWidgets WindowSystem I18n IconThemes)
find_package(Qt5 CONFIG REQUIRED COMPONENTS DBus)

### XCB
find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
  DESCRIPTION "X protocol C-language Binding"
  URL "https://xcb.freedesktop.org"
  TYPE OPTIONAL
  PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

if(UNIX AND NOT APPLE)

  set(LIGHTLY_HAVE_X11 ${XCB_XCB_FOUND})
  if (XCB_XCB_FOUND)
    find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
  endif()

else()

  set(LIGHTLY_HAVE_X11 FALSE)

endif()

################# configuration #################
configure_file(config-lightly.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-lightly.h )

################# includes #################
include_directories(${CMAKE_SOURCE_DIR}/liblightlycommon)
include_directories(${CMAKE_BINARY_DIR}/liblightlycommon)

################# newt target #################
### plugin classes
set(lightlydecoration_SRCS
    lightlybutton.cpp
    lightlydecoration.cpp
    lightlyexceptionlist.cpp
    lightlysettingsprovider.cpp
    lightlysizegrip.cpp)

kconfig_add_kcfg_files(lightlydecoration_SRCS lightlysettings.kcfgc)

### config classes
### they are kept separately because they might move in a separate library in the future
set(lightlydecoration_config_SRCS
    config/lightlyconfigwidget.cpp
    config/lightlydetectwidget.cpp
    config/lightlyexceptiondialog.cpp
    config/lightlyexceptionlistwidget.cpp
    config/lightlyexceptionmodel.cpp
    config/lightlyitemmodel.cpp
)

set(lightlydecoration_config_PART_FORMS
   config/ui/lightlyconfigurationui.ui
   config/ui/lightlydetectwidget.ui
   config/ui/lightlyexceptiondialog.ui
   config/ui/lightlyexceptionlistwidget.ui
)

ki18n_wrap_ui(lightlydecoration_config_PART_FORMS_HEADERS ${lightlydecoration_config_PART_FORMS})

### build library
add_library(lightlydecoration MODULE
    ${lightlydecoration_SRCS}
    ${lightlydecoration_config_SRCS}
    ${lightlydecoration_config_PART_FORMS_HEADERS})

target_link_libraries(lightlydecoration
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::DBus
    PRIVATE
        lightlycommon5
        KDecoration2::KDecoration
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::ConfigWidgets
        KF5::GuiAddons
        KF5::I18n
        KF5::IconThemes
        KF5::WindowSystem)

if(LIGHTLY_HAVE_X11)
  target_link_libraries(lightlydecoration
    PUBLIC
      Qt5::X11Extras
      XCB::XCB)
endif()


install(TARGETS lightlydecoration DESTINATION ${PLUGIN_INSTALL_DIR}/org.kde.kdecoration2)
install(FILES config/lightlydecorationconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
