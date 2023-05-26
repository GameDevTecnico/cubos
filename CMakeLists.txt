# CMakeLists.txt
# Cubos project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Should clang-tidy errors be automatically fixed?
option(FIX_CLANG_TIDY_ERRORS OFF)

# Enable coverage reports
option(ENABLE_COVERAGE OFF)
if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE core/tests/cubos-core-tests
        DEPENDENCIES cubos-core-tests

        EXCLUDE "core/lib/*" "core/tests/*"

        LCOV_ARGS "--no-external"
    )
endif()

# Function which configures a target with the options common to all CUBOS. targets.
function(cubos_common_target_options target)
    target_compile_features(${target} PRIVATE cxx_std_20)

    # Enable all warnings and treat them as errors
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor # Enable preprocessor conformance mode - required for __VA_ARGS__ to work correctly
            /W4
            /WX
            /wd4996  # Disable warning about deprecated functions
            /wd4458  # Disable warning about shadowing member variables
            /wd4456  # Disable warning about shadowing local variables
            /wd5030> # Introduces false positives for [[cubos::component]]
            
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
            -Wall
            -Wextra
            -pedantic
            -Wconversion
            -Werror
            -Wno-unknown-attributes> # Introduces false positives for [[cubos::component]]
    )

    # Enable clang-tidy
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        option(USE_CLANG_TIDY ON)
        if(USE_CLANG_TIDY)
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
            if(FIX_CLANG_TIDY_ERRORS)
                set(CLANG_TIDY_COMMAND "${CLANG_TIDY_COMMAND}" "-fix" "-fix-errors")
            endif()
            set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        endif()
    endif()
endfunction()

add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(engine)

# Add doxygen documentation

find_package(Doxygen COMPONENTS dot)

if (Doxygen_FOUND)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HIDE_SCOPE_NAMES YES)
    set(DOXYGEN_PROJECT_NAME "CUBOS.")
    set(DOXYGEN_HTML_EXTRA_FILES ${CMAKE_SOURCE_DIR}/docs/css/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_SOURCE_DIR}/docs/css/doxygen-awesome-css/doxygen-awesome.css)
    set(DOXYGEN_HTML_HEADER ${CMAKE_SOURCE_DIR}/docs/html/header.html)
    set(DOXYGEN_HTML_FOOTER ${CMAKE_SOURCE_DIR}/docs/html/footer.html)
    set(DOXYGEN_IMAGE_PATH ${CMAKE_SOURCE_DIR}/docs/images)
    set(DOXYGEN_EXCLUDE_PATTERNS "css" "html")
    doxygen_add_docs(
        doxygen
        "${CMAKE_SOURCE_DIR}/core/include"
        "${CMAKE_SOURCE_DIR}/engine/include"
        "${CMAKE_SOURCE_DIR}/docs"
        COMMENT "Generate documentation"
    )
endif ()
