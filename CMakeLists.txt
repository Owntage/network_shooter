cmake_minimum_required(VERSION 2.8)
project(network_shooter_project)

#gui sources
file(GLOB GUI_SOURCE_FILES ${PROJECT_SOURCE_DIR}/gui_source/*.cpp ${PROJECT_SOURCE_DIR}/gui_source/*.h)

#game_logic sources
file(GLOB GAME_LOGIC_FILES ${PROJECT_SOURCE_DIR}/game_logic_source/*.cpp 	${PROJECT_SOURCE_DIR}/game_logic_source/*.h)
file(GLOB GAME_LOGIC_COMPONENTS_FILES ${PROJECT_SOURCE_DIR}/game_logic_source/components/*.cpp 	${PROJECT_SOURCE_DIR}/game_logic_source/components/*.h)

#network_sources
file(GLOB NETWORK_FILES ${PROJECT_SOURCE_DIR}/network_source/*.cpp
	${PROJECT_SOURCE_DIR}/network_source/*.h)

#render_system sources
file(GLOB RENDER_SYSTEM_FILES ${PROJECT_SOURCE_DIR}/render_system/*.h ${PROJECT_SOURCE_DIR}/render_system/*.cpp)

#client sources
file(GLOB_RECURSE CLIENT_SOURCE_FILES ${PROJECT_SOURCE_DIR}/client_source/*.cpp ${PROJECT_SOURCE_DIR}/client_source/*.h)

#server sources
file(GLOB SERVER_SOURCE_FILES ${PROJECT_SOURCE_DIR}/server_source/*.cpp 	${PROJECT_SOURCE_DIR}/server_source/*.h)

#server source groups for visual studio
source_group("source" FILES ${SERVER_SOURCE_FILES})

#game_logic source groups for visual studio
source_group("game_logic" FILES ${GAME_LOGIC_FILES})
source_group("game_logic\\components" FILES ${GAME_LOGIC_COMPONENTS_FILES})

#test project sources
file(GLOB_RECURSE TEST_SOURCE_FILES ${PROJECT_SOURCE_DIR}/test_source/*.cpp ${PROJECT_SOURCE_DIR}/test_source/*.h)

add_library(gui ${GUI_SOURCE_FILES})
add_library(game_logic ${GAME_LOGIC_FILES} ${GAME_LOGIC_COMPONENTS_FILES})
add_library(render_system ${RENDER_SYSTEM_FILES})
add_library(network ${NETWORK_FILES})

add_executable(shooter_client ${CLIENT_SOURCE_FILES})
add_executable(shooter_server ${SERVER_SOURCE_FILES} ${SERVER_SOURCE_FILES_GAME_LOGIC} ${SERVER_SOURCE_FILES_GAME_LOGIC_COMPONENTS})
add_executable(shooter_test ${TEST_SOURCE_FILES})

add_dependencies(shooter_client gui game_logic render_system network)
add_dependencies(shooter_server gui game_logic render_system network)
add_dependencies(shooter_test gui game_logic render_system network)

add_definitions(-std=c++11 -g)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SFML REQUIRED system window graphics network audio)

#insert your boost root here
#header-only boost libraries are used, so you shouldn't build it
set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/include)
find_package(Boost REQUIRED)

if(SFML_FOUND AND Boost_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	include_directories(${Boost_INCLUDE_DIRS})
	include_directories(${PROJECT_SOURCE_DIR}/gui_source)
	include_directories(${PROJECT_SOURCE_DIR}/game_logic_source)
	include_directories(${PROJECT_SOURCE_DIR}/render_system)
	include_directories(${PROJECT_SOURCE_DIR}/network_source)
	include_directories(${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(game_logic ${PROJECT_SOURCE_DIR}/lib/Box2D.lib)
    	target_link_libraries(shooter_server ${SFML_LIBRARIES} gui game_logic render_system network)
	target_link_libraries(shooter_client ${SFML_LIBRARIES} gui game_logic render_system network)
	target_link_libraries(shooter_test ${SFML_LIBRARIES} gui game_logic render_system network)
endif()




