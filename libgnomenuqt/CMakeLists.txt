project(plasma-globalmenu)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)

find_package(Qt4)
SET(QT_USE_QTDBUS 1)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	${KDE4_INCLUDES}
	#	${QT_QTDBUS_INCLUDE_DIR}
	)

set(globalmenu_SRCS plasma-globalmenu.cpp server.cpp app-document.cpp)

kde4_add_plugin(plasma_applet_globalmenu ${globalmenu_SRCS})
target_link_libraries(plasma_applet_globalmenu
					  ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS}
					  ${QT_QTWEBKIT_LIBRARY} ${QT_QTDBUS_LIBRARY}
					  libtaskmanager.so
					  )

install (TARGETS plasma_applet_globalmenu
		  DESTINATION ${PLUGIN_INSTALL_DIR})
	
install (FILES plasma-applet-globalmenu.desktop
		DESTINATION ${SERVICES_INSTALL_DIR})
