cmake_minimum_required(VERSION 3.18)

project(gfx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

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

    list(APPEND CMAKE_PREFIX_PATH "${sdl2_SOURCE_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${glew_SOURCE_DIR}")
endif(WIN32)

find_package(sdl2 REQUIRED)
include_directories(${sdl2_SOURCE_DIR})

find_package(GLEW REQUIRED)
include_directories(${glew_INCLUDE_DIR})

find_package(OpenGL REQUIRED)

FetchContent_Declare(
    glm GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 33b0eb9fa336ffd8551024b1d2690e418014553b # v1.0.0
)
FetchContent_MakeAvailable(glm)

add_library(gfx STATIC
    gfx.h
    gl.cpp  gl.h
    image.cpp image.h
    transform.cpp transform.h
    camera.cpp camera.h
    util.h
)

target_include_directories(gfx PUBLIC
  "."
  "${glm_SOURCE_DIR}"
  "${sdl2_SOURCE_DIR}/include"
  "${glew_SOURCE_DIR}/include"
  "${SDL2_INCLUDE_DIRS}"
  "${glew_INCLUDE_DIR}"
)
target_link_libraries(gfx PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})


