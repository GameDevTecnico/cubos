set(CUBOS_API_ENGINE_SOURCE
    "src/api.cpp"
)

option(BUILD_API_ENGINE_SAMPLES "Build cubos engine c api samples" OFF)
option(BUILD_API_ENGINE_SHARED "Build cubos engine c api as shared library?" ON)

if (EMSCRIPTEN AND BUILD_API_ENGINE_SHARED)
	message(WARNING "Cannot build API engine as shared library on Emscripten")
	set(BUILD_API_ENGINE_SHARED OFF)
endif()

message("# Building engine c api samples: " ${BUILD_API_ENGINE_SAMPLES})
message("# Building engine c api as shared library: " ${BUILD_API_ENGINE_SHARED})

# Create engine c api library
if(BUILD_API_ENGINE_SHARED)
    add_library(cubos-api-engine SHARED ${CUBOS_API_ENGINE_SOURCE})
    target_compile_definitions(cubos-api-engine
        PRIVATE -DCUBOS_ENGINE_C_EXPORT
        PUBLIC -DCUBOS_ENGINE_C_IMPORT
    )
else()
    add_library(cubos-api-engine STATIC ${CUBOS_API_ENGINE_SOURCE})
endif()

target_include_directories(cubos-api-engine PUBLIC "include")
target_link_libraries(cubos-api-engine PRIVATE cubos-engine)
cubos_common_target_options(cubos-api-engine)

# Add core c api samples
if(BUILD_API_ENGINE_SAMPLES)
    add_subdirectory(samples)
endif()
