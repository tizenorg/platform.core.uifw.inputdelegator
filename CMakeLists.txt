CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(org.tizen.inputdelegator CXX C)

INCLUDE(GNUInstallDirs)
INCLUDE(FindPkgConfig)

SET(VERSION_MAJOR 1)
SET(VERSION ${VERSION_MAJOR}.0.0)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS} -Wall -fPIE")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wall -fPIE")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(po)

ADD_CUSTOM_TARGET(w-input-selector.edj
		COMMAND edje_cc
		-id ${CMAKE_SOURCE_DIR}/edje/images
		${CMAKE_SOURCE_DIR}/res/edje/w-input-selector.edc
		${CMAKE_BINARY_DIR}/res/edje/w-input-selector.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/res/edje/w-input-selector.edc)

ADD_DEPENDENCIES(inputdelegator w-input-selector.edj)

ADD_CUSTOM_TARGET(w-input-stt.edj
		COMMAND edje_cc
		-id ${CMAKE_SOURCE_DIR}/edje/images
		${CMAKE_SOURCE_DIR}/res/edje/w-input-stt.edc
		${CMAKE_BINARY_DIR}/res/edje//w-input-stt.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/res/edje//w-input-stt.edc)

ADD_DEPENDENCIES(inputdelegator w-input-stt.edj)

ADD_CUSTOM_TARGET(w-input-stt-button.edj
		COMMAND edje_cc
		-id ${CMAKE_SOURCE_DIR}/edje/images
		${CMAKE_SOURCE_DIR}/res/edje/w-input-stt-button.edc
		${CMAKE_BINARY_DIR}/res/edje/w-input-stt-button.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/res/edje/w-input-stt-button.edc)

ADD_DEPENDENCIES(inputdelegator w-input-stt-button.edj)

INSTALL(FILES ${CMAKE_BINARY_DIR}/res/edje/w-input-selector.edj
	DESTINATION res/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/res/edje/w-input-stt.edj
	DESTINATION res/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/res/edje/w-input-stt-button.edj
	DESTINATION res/edje)

FILE(GLOB_RECURSE W_INPUT_SELECTOR_IMAGES "${CMAKE_SOURCE_DIR}/res/images/*.png")
INSTALL(FILES ${W_INPUT_SELECTOR_IMAGES} DESTINATION res/images)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/shared/res/w-input-selector.png DESTINATION shared/res)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/shared/res/org.tizen.inputdelegator_color.xml DESTINATION shared/res)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/shared/res/org.tizen.inputdelegator_font.xml DESTINATION shared/res)
