cmake_minimum_required(VERSION 3.21)

if(WIN32)
    FetchContent_Declare(
        glew
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
    )
    FetchContent_MakeAvailable(glew)

    FetchContent_Declare(
        sdl2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.26.4/SDL2-devel-2.26.4-VC.zip
    )
    FetchContent_MakeAvailable(sdl2)

    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/_deps/sdl2-src")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/_deps/glew-src")
endif(WIN32)

find_package(sdl2 REQUIRED)
include_directories(${sdl2_SOURCE_DIR})

find_package(GLEW REQUIRED)
include_directories(${glew_INCLUDE_DIR})

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
  "${glm_SOURCE_DIR}"
  "${sdl2_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/build/_deps/sdl2-src/include"
  "${glew_SOURCE_DIR}/include"
)
target_link_libraries(gfx PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})


