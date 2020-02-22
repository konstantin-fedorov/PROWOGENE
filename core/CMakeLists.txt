cmake_minimum_required(VERSION 3.1)

project(prowogene_core)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (BASE_HEADERS
    generator.h
    logger.h
    module_interface.h
    settings_interface.h
    storage.h
    types.h
)

set (BASE_SOURCES
    generator.cpp
    logger.cpp
)

set (MODULES_HEADERS
    modules/basis.h
    modules/cliff.h
    modules/filename_settings.h
    modules/general_settings.h
    modules/item.h
    modules/location.h
    modules/model.h
    modules/mountain.h
    modules/names_settings.h
    modules/post_process.h
    modules/river.h
    modules/system_settings.h
    modules/texture.h
    modules/water.h
)

set (MODULES_SOURCES
    modules/basis_module.cpp
    modules/basis_settings.cpp
    modules/cliff_module.cpp
    modules/cliff_settings.cpp
    modules/filename_settings.cpp
    modules/general_settings.cpp
    modules/item_module.cpp
    modules/item_settings.cpp
    modules/location_module.cpp
    modules/location_settings.cpp
    modules/model_module.cpp
    modules/model_settings.cpp
    modules/mountain_module.cpp
    modules/mountain_settings.cpp
    modules/names_settings.cpp
    modules/post_process_module.cpp
    modules/post_process_settings.cpp
    modules/river_module.cpp
    modules/river_settings.cpp
    modules/system_settings.cpp
    modules/texture_module.cpp
    modules/texture_settings.cpp
    modules/water_module.cpp
    modules/water_settings.cpp
)

set (UTILS_HEADERS
    utils/array2d.h
    utils/array2d_tools.h
    utils/bmp.h
    utils/image.h
    utils/image_io.h
    utils/json.h
    utils/model3d.h
    utils/model_io.h
    utils/obj.h
    utils/range.h
    utils/random.h
    utils/types_converter.h
)

set (UTILS_SOURCES
    utils/array2d_tools.cpp
    utils/bmp.cpp
    utils/image.cpp
    utils/image_io.cpp
    utils/json.cpp
    utils/model3d.cpp
    utils/model_io.cpp
    utils/obj.cpp
    utils/random.cpp
    utils/types_converter.cpp
)

source_group("modules_headers" FILES ${MODULES_HEADERS})
source_group("modules_sources" FILES ${MODULES_SOURCES})
source_group("utils_headers"   FILES ${UTILS_HEADERS})
source_group("utils_sources"   FILES ${UTILS_SOURCES})
source_group("base_headers"    FILES ${BASE_HEADERS})
source_group("base_sources"    FILES ${BASE_SOURCES})

add_library(${PROJECT_NAME} STATIC
    ${MODULES_SOURCES} ${MODULES_HEADERS}
    ${UTILS_SOURCES}   ${UTILS_HEADERS}
    ${BASE_HEADERS}    ${BASE_SOURCES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}"
)