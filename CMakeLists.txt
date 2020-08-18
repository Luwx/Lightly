cmake_minimum_required(VERSION 3.0)
project(lightly)
set(PROJECT_VERSION "5.19.4")
set(PROJECT_VERSION_MAJOR 5)

set(KF5_MIN_VERSION "5.66.0")

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)

include(GtkUpdateIconCache)

option(WITH_DECORATIONS "Build Lightly window decorations for KWin" ON)
if(WITH_DECORATIONS)
    find_package(KDecoration2 REQUIRED)
    add_subdirectory(kdecoration)
endif()

add_subdirectory(colors)
add_subdirectory(liblightlycommon)
add_subdirectory(kstyle)
add_subdirectory(misc)


include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX LIGHTLY
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LightlyConfigVersion.cmake"
                  )

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/Lightly")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/LightlyConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/LightlyConfig.cmake"
                                  PATH_VARS KDE_INSTALL_FULL_DATADIR
                                  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                                  )

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/LightlyConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/LightlyConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
