cmake_minimum_required(VERSION 3.0.0)
project(cubos VERSION 0.1.0)

option(GLFW_USE_SUBMODULE "Compile GLFW from source?" ON)
option(GLM_USE_SUBMODULE "Compile GLM from source?" ON)
option(YAMLCPP_USE_SUBMODULE "Compile YAML CPP from source?" ON)
option(GOOGLETEST_USE_SUBMODULE "Compile Google Test from source?" ON)

set(CUBOS_SOURCE
    "src/main.cpp"
)

set(CUBOS_INCLUDE
)

add_library(cubos ${CUBOS_SOURCE} ${CUBOS_INCLUDE})
target_include_directories(cubos PUBLIC ${CUBOS_INCLUDE})

# Dependencies
if(GLFW_USE_SUBMODULE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(lib/glfw)
else()
    find_package(glfw3 REQUIRED)
endif()

if(GLM_USE_SUBMODULE)
    add_subdirectory(lib/glm)
else()
    find_package(glm REQUIRED)
endif()

if (YAMLCPP_USE_SUBMODULE)
    add_subdirectory(lib/yaml-cpp)
else()
    find_package(yaml-cpp REQUIRED)
endif()



if (GOOGLETEST_USE_SUBMODULE)
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory(lib/googletest)
else()
    find_package(googletest REQUIRED)
endif()


add_subdirectory(lib/glad)

target_link_libraries(cubos glfw glm glad yaml-cpp ${CMAKE_DL_LIBS})

# Enable CTest and CPack
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Enable Google Test
set(CUBOS_TESTS_SOURCE
    "tests/test_example.cpp"
)

add_executable(
    cubos_tests
    ${CUBOS_TESTS_SOURCE}
)

target_link_libraries(
    cubos_tests
    gtest_main
)

include(googletest)
gtest_discover_tests(cubos_tests)