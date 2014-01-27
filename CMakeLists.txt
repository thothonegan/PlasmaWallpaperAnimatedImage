project (plasma-wallpaper-animatedimage)

find_package (KDE4 REQUIRED)
find_package (KDE4Workspace REQUIRED)
include (KDE4Defaults)

add_definitions (${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set (animatedimage_HEADERS
	Wallpaper.hpp
)

set (animatedimage_SOURCES
	Wallpaper.cpp
)

kde4_add_ui_files (animatedimage_SOURCES config.ui)

kde4_add_plugin (plasma_wallpaper_animatedimage ${animatedimage_HEADERS} ${animatedimage_SOURCES})
target_link_libraries (plasma_wallpaper_animatedimage ${KDE4_PLASMA_LIBS})

install (TARGETS plasma_wallpaper_animatedimage DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES plasma-wallpaper-animatedimage.desktop DESTINATION ${SERVICES_INSTALL_DIR})


