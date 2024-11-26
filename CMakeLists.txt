# CMakeLists.txt
# Cubos project root build configuration

cmake_minimum_required(VERSION 3.25.0)
project(cubos VERSION 0.4.0)

include(GNUInstallDirs) # Get default install directories

# ------------------------ Set global CMake variables -------------------------

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)

# -------------------------- Declare global options ---------------------------

option(CUBOS_FIX_CLANG_TIDY_ERRORS "Automatically fix cubos clang-tidy errors" OFF)
option(CUBOS_USE_CCACHE "Enable CCache for building cubos" ON)
option(CUBOS_DOCUMENTATION "Build cubos docs" OFF)
option(CUBOS_ENABLE_INSTALL "Configure cubos for installation" ${PROJECT_IS_TOP_LEVEL})

# ------------------------ Configure coverage reports -------------------------

# Enable coverage reports
option(ENABLE_COVERAGE "Generate coverage report" OFF)
if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    setup_target_for_coverage_lcov(
        NAME coverage-core
        EXECUTABLE cubos-core-tests
        DEPENDENCIES cubos-core-tests

        EXCLUDE "core/lib/*" "core/tests/*" "engine/*" "lib/*" "tools/*" "*-components.cpp"

        LCOV_ARGS "--no-external"
    )

    setup_target_for_coverage_lcov(
        NAME coverage-engine
        EXECUTABLE cubos-engine-tests
        DEPENDENCIES cubos-engine-tests

        EXCLUDE "engine/lib/*" "engine/tests/*" "core/*" "lib/*" "tools/*" "*-components.cpp"

        LCOV_ARGS "--no-external"
    )

    add_custom_target(coverage DEPENDS coverage-core coverage-engine)
endif()

# ----------------------------- Configure CCache ------------------------------

find_program(CCACHE_EXE NAMES "ccache")
if(CCACHE_EXE AND CUBOS_USE_CCACHE)
    if (CMAKE_HOST_WIN32)
        option(CCACHE_VERSION "Pinned CCache Version")

        # find_program only finds Chocolatey's shim which is unable to be used with another name, the original executable must be used.
        set(CCACHE_PATH C:/ProgramData/chocolatey/lib/ccache/tools/ccache-${CCACHE_VERSION}-windows-x86_64/ccache.exe)

        file(COPY_FILE
            ${CCACHE_PATH} ${CMAKE_BINARY_DIR}/cl.exe
            RESULT FILE_NOT_FOUND ONLY_IF_DIFFERENT)

        if (FILE_NOT_FOUND)
            set(CCACHE_PATH C:/msys64/mingw64/bin/ccache.exe)
            file(COPY_FILE
                ${CCACHE_PATH} ${CMAKE_BINARY_DIR}/cl.exe
                ONLY_IF_DIFFERENT)
        endif()

        set(CMAKE_VS_GLOBALS
            "CLToolExe=cl.exe"
            "CLToolPath=${CMAKE_BINARY_DIR}"
            "TrackFileAccess=false"
            "UseMultiToolTask=true"
            "DebugInformationFormat=OldStyle"
        )
    else ()
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXE}")
    endif()
endif()

# ---------------- Define configuration common to all targets -----------------

function(cubos_common_target_options target)
    # Enable all warnings and treat them as errors
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor # Enable preprocessor conformance mode - required for __VA_ARGS__ to work correctly
            /W4
            /WX
            /wd4996  # Disable warning about deprecated functions
            /wd4458  # Disable warning about shadowing member variables
            /wd4456  # Disable warning about shadowing local variables
            /wd4335  # Disable warning about Mac file format
            /wd4702  # Disable warning about unreachable code
            /wd4251> # Disable warning about missing DLL interfaces

        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
            -Wall
            -Wextra
            -pedantic
            -Wconversion
            -Werror
            -Wno-attributes>

        $<$<CXX_COMPILER_ID:GNU>:
            -Wno-maybe-uninitialized> # Causes false positives with optionals 
    )

    # Enable clang-tidy
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        option(USE_CLANG_TIDY "Enable clang-tidy" OFF)
        if(USE_CLANG_TIDY)
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
            if(CUBOS_FIX_CLANG_TIDY_ERRORS)
                set(CLANG_TIDY_COMMAND "${CLANG_TIDY_COMMAND}" "-fix" "-fix-errors")
            endif()
            set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        endif()
    endif()
endfunction()

# -------------- Fetch dependencies common to multiple libraries --------------

# Fetch doctest, which we use in all tests
include(FetchContent)
FetchContent_Declare(
	doctest
	GIT_REPOSITORY https://github.com/doctest/doctest
	GIT_TAG        v2.4.11
	SYSTEM
	FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(doctest)

# --------------------------- Configure all targets ---------------------------

add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(api)
add_subdirectory(tools)
if(CUBOS_DOCUMENTATION)
    add_subdirectory(docs)
endif()

# ---------------------- Configure project installation -----------------------

if(CUBOS_ENABLE_INSTALL)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/cubos-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
    )

    set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/cubos)

    install(EXPORT cubos-core-targets
        FILE cubos-core-targets.cmake
        NAMESPACE cubos::
        DESTINATION ${INSTALL_CONFIGDIR}
    )

    install(EXPORT cubos-engine-targets
        FILE cubos-engine-targets.cmake
        NAMESPACE cubos::
        DESTINATION ${INSTALL_CONFIGDIR}
    )

    # We need to pass the path to the assets directory relative to the config file,
    # so that the config file can find it when included through find_package in another project
    set(CUBOS_ENGINE_ASSETS_INSTALL_RELPATH ${CUBOS_ENGINE_ASSETS_INSTALL_PATH})
    cmake_path(RELATIVE_PATH CUBOS_ENGINE_ASSETS_INSTALL_RELPATH BASE_DIRECTORY ${INSTALL_CONFIGDIR})

    configure_file(cmake/cubos-config.cmake.in cubos-config.cmake @ONLY)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cubos-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cubos-config-version.cmake"
        DESTINATION ${INSTALL_CONFIGDIR}
    )
endif()
