################# dependencies #################
### Qt/KDE
find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets)

################# lightlystyle target #################
set(lightlycommon_LIB_SRCS
    lightlyboxshadowrenderer.cpp
)

add_library(lightlycommon5 ${lightlycommon_LIB_SRCS})

generate_export_header(lightlycommon5
    BASE_NAME lightlycommon
    EXPORT_FILE_NAME lightlycommon_export.h)

target_link_libraries(lightlycommon5
    PUBLIC
        Qt5::Core
        Qt5::Gui)

set_target_properties(lightlycommon5 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS lightlycommon5 ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
