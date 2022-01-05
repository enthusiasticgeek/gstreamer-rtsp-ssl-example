cmake_minimum_required(VERSION 3.5)

project( gstreamer-rtsp-ssl-example LANGUAGES C)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GSTLIBS REQUIRED
        gobject-2.0
        glib-2.0)

pkg_search_module(GLIB REQUIRED glib-2.0) 
pkg_check_modules(GSTREAMER REQUIRED 
        gstreamer-controller-1.0 
        gstreamer-plugins-base-1.0 
        gstreamer-base-1.0 
        gstreamer-1.0 
        gstreamer-rtsp-1.0 
        gstreamer-rtsp-server-1.0)

include_directories(${GSTREAMER_INCLUDE_DIRS})
link_directories(${GSTREAMER_LIBRARIES})

add_executable(rtsp_client rtsp_client.c rtsp_client_tls_interaction.c)
add_executable(rtsp_server rtsp_server.c)

target_link_libraries(rtsp_client ${GSTREAMER_LIBRARIES})

target_link_libraries(rtsp_server ${GSTREAMER_LIBRARIES})

