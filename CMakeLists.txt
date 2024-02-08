cmake_minimum_required(VERSION 3.21)

if(WIN32)
    FetchContent_Declare(
        GLEW
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
    )
    FetchContent_MakeAvailable(GLEW)

    FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.26.4/SDL2-devel-2.26.4-VC.zip
    )
    FetchContent_MakeAvailable(SDL2)

    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/_deps/sdl2-src")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/_deps/glew-src")
endif(WIN32)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

FetchContent_Declare(
    glm GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)

add_library(gfx STATIC
    gl.cpp
    image.cpp
    transform.cpp
    camera.cpp
)

target_include_directories(gfx PUBLIC
  "."
  "${SDL2_INCLUDE_DIR}"
  "${CMAKE_SOURCE_DIR}/build/_deps/glm-src"
  "${CMAKE_SOURCE_DIR}/build/_deps/glew-src/include"
)
target_link_libraries(gfx PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})


