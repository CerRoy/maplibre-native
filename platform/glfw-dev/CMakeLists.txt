# Experimental GLFW frontend: camera updates run on the GLFW event thread while
# GPU rendering runs on a dedicated render thread.
# This target is added by platform/linux/linux.cmake only when OpenGL is enabled.
if(WIN32)
    find_package(glfw3 REQUIRED)
    find_package(libuv REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW glfw3 REQUIRED)
endif()

add_executable(
    mbgl-glfw-dev
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw_view.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw_renderer_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/settings_json.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/test_writer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mln/map/map_snapshotter.cpp
)

set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/glfw_view.cpp
    PROPERTY COMPILE_DEFINITIONS MLN_ASSETS_PATH="${PROJECT_SOURCE_DIR}/platform/glfw/assets/")

if(MLN_WITH_OPENGL)
    find_package(OpenGL REQUIRED)
    target_sources(mbgl-glfw-dev PRIVATE ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_gl_backend.cpp)
endif()
if(MLN_WITH_EGL)
    target_compile_definitions(mbgl-glfw-dev PRIVATE MBGL_WITH_EGL=1)
endif()
if(DEFINED MLN_WITH_X11 AND MLN_WITH_X11)
    target_compile_definitions(mbgl-glfw-dev PRIVATE MLN_WITH_X11=1)
endif()
if(DEFINED MLN_WITH_WAYLAND AND MLN_WITH_WAYLAND)
    target_compile_definitions(mbgl-glfw-dev PRIVATE MLN_WITH_WAYLAND=1)
endif()

target_include_directories(mbgl-glfw-dev PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/platform/glfw ${GLFW_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(mbgl-glfw-dev PRIVATE $<$<BOOL:${GLFW_LIBRARY_DIRS}>:-L${GLFW_LIBRARY_DIRS}>)
if(WIN32)
    target_compile_definitions(mbgl-glfw-dev PRIVATE NOMINMAX CURL_STATICLIB)
    target_link_libraries(mbgl-glfw-dev PRIVATE glfw $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
else()
    target_link_libraries(mbgl-glfw-dev PRIVATE ${GLFW_LIBRARIES})
endif()
target_link_libraries(mbgl-glfw-dev PRIVATE mbgl-vendor-args mbgl-vendor-rapidjson Mapbox::Map mbgl-compiler-options MapLibreNative::Base::cheap-ruler-cpp)
if(NOT WIN32 AND MLN_WITH_OPENGL)
    target_link_libraries(mbgl-glfw-dev PRIVATE OpenGL::GL)
endif()
set_property(TARGET mbgl-glfw-dev PROPERTY FOLDER MapLibre)
install(TARGETS mbgl-glfw-dev RUNTIME DESTINATION bin)
