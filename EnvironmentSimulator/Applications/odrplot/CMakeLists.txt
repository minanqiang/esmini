
set( CMAKE_CURRENT_SOURCE_DIR "../../../")
set( ROADMANAGER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentSimulator/Modules/RoadManager" )
set( COMMON_MINI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentSimulator/Modules/CommonMini")
set( PUGIXML_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/externals/pugixml")

include_directories (
${PUGIXML_INCLUDE_DIR}
  ${ROADMANAGER_INCLUDE_DIR}
  ${COMMON_MINI_INCLUDE_DIR}  
)

set(TARGET odrplot)

set ( SOURCES
  appendLaneId.cpp
)

set ( INCLUDES
)

add_executable ( ${TARGET} ${SOURCES} ${INCLUDES} )

target_link_libraries ( 
	${TARGET}
	RoadManager
	CommonMini	
	${TIME_LIB}
)

install ( TARGETS ${TARGET} DESTINATION "${INSTALL_DIRECTORY}")
