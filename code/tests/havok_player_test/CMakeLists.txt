#-------------------------------------------------------------------------------
# Havok player-test
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/foundation
					${Nebula3_SOURCE_DIR}/render
					${Nebula3_SOURCE_DIR}/application							
					${Nebula3_SOURCE_DIR}/application/basegamefeature
					${Nebula3_SOURCE_DIR}/application/corefeature
					${Nebula3_SOURCE_DIR}/application/graphicsfeature
					${Nebula3_SOURCE_DIR}/application/physicsfeature
					${Nebula3_SOURCE_DIR}/addons		
					${Nebula3_SOURCE_DIR}/extlibs
					${Nebula3_SOURCE_DIR}/sui
					${Nebula3_SOURCE_DIR}/tools		
					${Nebula3_SOURCE_DIR}/toolkit
					${Nebula3_SOURCE_DIR}/tests/
					${Nebula3_SOURCE_DIR}
					
					${Physics_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("havok_player_test" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(havok_player_test ${ALLSRCS})
SET_TARGET_PROPERTIES(havok_player_test PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(havok_player_test)
N_SET_EXE_SUBSYSTEM(havok_player_test Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(havok_player_test stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(havok_player_test
	${NEBULA_DEFAULT_LIBS}
	foundation
	zlib
	tinyxml
	)
TARGET_LINK_LIBRARIES(havok_player_test 
	${NEBULA_DEFAULT_LIBS}
	foundation
	zlib
	tinyxml
	render
	sui audio2 remote attr vibration tinyxml zlib sqlite db
		posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
		render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	${N_LIBS}
	${HAVOK_LIBS}
)
