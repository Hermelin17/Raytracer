cmake_minimum_required(VERSION 3.10)
project(RayTracerOpenGL)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

add_executable(raytracer
    src/main.cpp
    src/shader.cpp
)

target_link_libraries(raytracer
    ${OPENGL_LIBRARIES}
    glfw
    GLEW::GLEW
    glm::glm
)

# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Release)
# endif()
# add_compile_options(
#   $<$<CONFIG:Release>:-O3 -ffast-math -fno-math-errno -DNDEBUG>
# )
# # Optional (often helps on Apple clang):
# # add_link_options($<$<CONFIG:Release>:-flto>)


# --- MC grade-3 CPU renderer ---
add_executable(mc3 src/mc3_main.cpp)
target_include_directories(mc3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(mc2 src/mc2_main.cpp)
target_include_directories(mc2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt3 src/rt3_main.cpp)
target_include_directories(rt3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt4 src/rt4_main.cpp)
target_include_directories(rt4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt5 src/rt5_main.cpp)
target_include_directories(rt5 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt6 src/rt6_main.cpp)
target_include_directories(rt6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt7 src/rt7_main.cpp)
target_include_directories(rt7 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rt_room src/rt_room.cpp)
target_include_directories(rt_room PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)




