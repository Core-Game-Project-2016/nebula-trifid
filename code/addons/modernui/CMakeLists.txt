#-------------------------------------------------------------------------------
# modernui
#-------------------------------------------------------------------------------

OPTION(N_BUILD_AWESOMIUM "Awesomium Modern UI Library HTML5,CSS3,Javasript" OFF)
#SET(N_DEFAULT_UI "N_BUILD_AWESOMIUM")
#SET(N_UI ${N_DEFAULT_UI} CACHE STRING "UI backend chosen by CMake")
#SET_PROPERTY(CACHE N_UI PROPERTY STRINGS "N_BUILD_AWESOMIUM")
#SET(${N_UI} ON)

IF(N_BUILD_AWESOMIUM)

set(SOURCES_files_modernui__awesomium
	awesomium/awesomiumlayout.cc
	awesomium/awesomiumlayout.h
	awesomium/awesomiumeventlistener.h
	awesomium/awesomiumeventlistener.cc
	awesomium/awesomiumrenderer.cc
	awesomium/awesomiumrenderer.h
	awesomium/awesomiumserver.cc
	awesomium/awesomiumserver.h
	awesomium/awesomiumelement.cc
	awesomium/awesomiumelement.h
	awesomium/awesomiumsurface.cc
	awesomium/awesomiumsurface.h
	awesomium/awesomiumsurfacefactory.cc
	awesomium/awesomiumsurfacefactory.h
	awesomium/awesomiumjsmethodhandler.cc
	awesomium/awesomiumjsmethodhandler.h
)
source_group("modernui\\awesomium" FILES ${SOURCES_files_modernui__awesomium})

set(SOURCES_modernui__awesomium
	${SOURCES_files_modernui__awesomium}
)

set(SOURCES_files_modernui
	modernuirtplugin.cc
	modernuirtplugin.h
	modernuiinputhandler.cc
	modernuiinputhandler.h
)
source_group("modernui" FILES ${SOURCES_files_modernui})

set(SOURCES_modernui
  ${SOURCES_files_modernui__awesomium}
  ${SOURCES_files_modernui}
)

set(SOURCES_mfiles
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_mfiles})

set(MSOURCES_
	${SOURCES_modernui}
	${SOURCES_mfiles}		
)

ADD_LIBRARY( modernui STATIC ${MSOURCES_} )

TARGET_LINK_LIBRARIES(modernui PUBLIC render application)

ADD_DEPENDENCIES(modernui faudio render)

TARGET_INCLUDE_DIRECTORIES(modernui PUBLIC ${AWESOMIUM_INCLUDE} ${Nebula3_SOURCE_DIR}/code/addons)
TARGET_COMPILE_DEFINITIONS(modernui PUBLIC __AWESOMIUM__)
TARGET_LINK_LIBRARIES(modernui INTERFACE awesomium)
SET_TARGET_PROPERTIES(modernui PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(modernui stdneb.h stdneb.cc ../)
ENDIF()

ENDIF()
