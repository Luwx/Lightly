### configuration plugin
add_definitions(-DTRANSLATION_DOMAIN=\"lightly_style_config\")
set(kstyle_lightly_config_PART_SRCS lightlystyleconfig.cpp)

set(kstyle_lightly_config_PART_SRCS ${kstyle_lightly_config_PART_SRCS} lightlystyleconfigmodule.cpp)

kconfig_add_kcfg_files(kstyle_lightly_config_PART_SRCS ../lightlystyleconfigdata.kcfgc)
set(kstyle_lightly_config_PART_FORMS ui/lightlystyleconfig.ui)
ki18n_wrap_ui(kstyle_lightly_config_PART_FORMS_HEADERS ${kstyle_lightly_config_PART_FORMS})

add_library(kstyle_lightly_config MODULE
    ${kstyle_lightly_config_PART_SRCS}
    ${kstyle_lightly_config_PART_FORMS_HEADERS}
)

target_link_libraries(kstyle_lightly_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
target_link_libraries(kstyle_lightly_config KF5::ConfigCore KF5::ConfigWidgets KF5::I18n)
install(TARGETS kstyle_lightly_config DESTINATION ${QT_PLUGIN_INSTALL_DIR})
install(FILES lightlystyleconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})

########### next target ###############
find_package(KF5KCMUtils CONFIG)
set_package_properties(KF5KCMUtils PROPERTIES
    DESCRIPTION "KF5 KCM utils Framework"
    URL "https://projects.kde.org/projects/frameworks/kcmutils"
    TYPE OPTIONAL
    PURPOSE "Required for lightly-settings5 application to build")

if( KF5KCMUtils_FOUND )
    set(lightly_settings_SOURCES main.cpp)
    add_executable(lightly-settings5 ${lightly_settings_SOURCES} )
    target_link_libraries(lightly-settings5 Qt5::Core Qt5::Gui Qt5::Widgets )
    target_link_libraries(lightly-settings5 KF5::I18n KF5::KCMUtils)

    install(TARGETS lightly-settings5 ${INSTALL_TARGETS_DEFAULT_ARGS})

    # icon
    ecm_install_icons(ICONS sc-apps-lightly-settings.svgz
        DESTINATION ${ICON_INSTALL_DIR}
        THEME hicolor )
endif()
