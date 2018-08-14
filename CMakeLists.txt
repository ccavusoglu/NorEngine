set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.6)
project(NorEngine)
set(PROJECT_VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(LOCAL_LIB_PATH "C:/Development/Library/")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

set(SDL2_GPU_LIBRARY_PATH ${LOCAL_LIB_PATH}bin/libSDL2_gpu.dll)
set(SDL2_GPU_INCLUDE_PATH ${LOCAL_LIB_PATH}include/SDL2_gpu)

add_library(GLM src/include/Glm.h src/util/Glm.cpp)
set_source_files_properties(Glm.cpp PROPERTIES COMPILE_FLAGS "-include src/include/Glm.h")

include_directories(
        src/include
        ${SDL2_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
        ${SDL2_GPU_INCLUDE_PATH}
        libs/glm/glm/)

set(SOURCE_FILES src/main.cpp
        src/AssetManager.cpp src/include/AssetManager.h
        src/include/Entity.h
        src/Game.cpp src/include/Game.h
        src/include/Screen.h
        src/SplashScreen.cpp src/include/SplashScreen.h
        src/include/Texture.h src/util/Glm.cpp src/include/Glm.h
        src/Renderer.cpp src/include/Renderer.h src/EntityManager.cpp
        src/include/EntityManager.h src/include/Component.h src/MenuScreen.cpp
        src/include/MenuScreen.h src/InputManager.cpp src/include/InputManager.h
        src/Camera.cpp src/include/Camera.h src/include/Configuration.h
        src/GameScreen.cpp src/include/GameScreen.h src/include/Util.h src/util/Util.cpp
        src/include/Action.h src/include/Interpolation.h src/Action.cpp
        src/include/Definitions.h src/include/SignalManager.h src/SignalManager.cpp src/util/ResHelper.cpp src/include/ResHelper.h src/PlayerManager.cpp src/include/PlayerManager.h src/EnemyManager.cpp src/include/EnemyManager.h src/GameConstants.cpp src/include/GameConstants.h
        src/AiManager.cpp src/include/AiManager.h
        src/include/NFont.h
        src/external/NFont.cpp
        src/include/NFont_gpu.h
        src/include/SDL_FontCache.h
        src/external/SDL_FontCache.c src/ScreenManager.cpp src/include/ScreenManager.h
        src/include/tinyxml2.h src/tinyxml2.cpp)

add_executable(NorEngine ${SOURCE_FILES})

# needed to get rid of -mwindows flag in order for console to work
target_link_libraries(NorEngine
        -lmingw32
        ${SDL2MAIN_LIBRARY}
        ${LOCAL_LIB_PATH}lib/libSDL2.dll.a
        -lpthread
        ${SDL2_GPU_LIBRARY_PATH}
        ${SDL2_TTF_LIBRARY}
        GLM
        )
