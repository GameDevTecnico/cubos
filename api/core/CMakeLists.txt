option(BUILD_API_CORE_SAMPLES "Build cubos core c api samples" OFF)
option(BUILD_API_CORE_SHARED "Build cubos core c api as shared library?" ON)

message("# Building core c api samples: " ${BUILD_API_CORE_SAMPLES})
message("# Building core c api as shared library: " ${BUILD_API_CORE_SHARED})

set(CUBOS_API_CORE_SOURCE
    "src/api.cpp"
)

# Create core c api library
if(BUILD_API_CORE_SHARED)
    add_library(cubos-api-core SHARED ${CUBOS_API_CORE_SOURCE})
    target_compile_definitions(cubos-api-core
        PRIVATE -DCUBOS_CORE_C_EXPORT
        PUBLIC -DCUBOS_CORE_C_IMPORT
    )
else()
    add_library(cubos-api-core STATIC ${CUBOS_API_CORE_SOURCE})
endif()

target_include_directories(cubos-api-core PUBLIC "include")
target_link_libraries(cubos-api-core PRIVATE cubos-core)
cubos_common_target_options(cubos-api-core)

# Add core c api samples
if(BUILD_API_CORE_SAMPLES)
    add_subdirectory(samples)
endif()
