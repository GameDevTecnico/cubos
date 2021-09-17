cmake_minimum_required(VERSION 3.0.0)
project(cubos VERSION 0.1.0)

option(GLFW_USE_SUBMODULE ON "Compile GLFW from source?")
option(GLM_USE_SUBMODULE ON "Compile GLM from source?")

set(CUBOS_SOURCE
    "src/main.cpp"
)

set(CUBOS_INCLUDE
)

add_library(cubos ${CUBOS_SOURCE} ${CUBOS_INCLUDE})
target_include_directories(cubos PUBLIC ${CUBOS_INCLUDE})

# Dependencies
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
add_library(glad "lib/glad/src/glad.c")
target_include_directories(glad PUBLIC "lib/glad/include")
target_link_libraries(cubos glfw glm glad ${CMAKE_DL_LIBS})

# Enable CTest and CPack
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
