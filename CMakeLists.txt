cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(MummyGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_DIR "${CMAKE_SOURCE_DIR}/vendor/SFML/lib/cmake/SFML")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/SFML/lib/cmake")

find_package(SFML 3 COMPONENTS Graphics Audio Window Main System REQUIRED)

set(SOURCES
	MummyN04.cpp
	src/Core/Game.cpp
    	src/Core/Maze.cpp
	src/Core/TimeControl.cpp 
	src/Core/GameData.cpp 
	src/Core/MazeGenerator.cpp
	src/GUI/Button.cpp 
    	src/Entities/Player.cpp
	src/Entities/Mummy.cpp
    	src/States/GameState.cpp
 	src/States/SettingState.cpp 
	src/States/MainMenuState.cpp 
	src/States/ModeSelectState.cpp
	src/States/CustomizeState.cpp 
	src/States/HowtoplayState.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Main
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets folder to build directory..."
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MummyGame PROPERTY CXX_STANDARD 20)
endif()
