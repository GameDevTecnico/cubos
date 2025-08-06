# bindings/lua/CMakeLists.txt
# Lua bindings build configuration

# ---------------------- Declare lua bindings library options -----------------------

option(CUBOS_LUA_SAMPLES "Build lua bidings samples" ON)

message("# Building cubos::bindings::lua samples " ${CUBOS_LUA_SAMPLES})

# ----------------------- Define lua bindings source files -----------------------

set(LUA_BINDINGS_SOURCE
    "src/cubos.cpp"
    "src/plugin.cpp"
    "src/systems.cpp"
)

# ------------------------ Configure lua bindings target -------------------------

add_library(cubos-bindings-lua ${LUA_BINDINGS_SOURCE})
target_link_libraries(cubos-bindings-lua PRIVATE cubos::engine cubos-api-core cubos-api-engine)
target_include_directories(cubos-bindings-lua PUBLIC "include")
cubos_common_target_options(cubos-bindings-lua)

# ------------- Fetch and link private library dependencies -------------

include(FetchContent)

FetchContent_Declare(
	lua
	GIT_REPOSITORY "https://github.com/GameDevTecnico/cubos-lua.git"
    GIT_TAG "9560c5ab00cc32f97af0d9bfe5c0b8f22be148e1"
	SYSTEM
)
FetchContent_MakeAvailable(lua)
target_link_libraries(cubos-bindings-lua PRIVATE lua)

# ------------------------- Install lua bindings target --------------------------

if(CUBOS_ENABLE_INSTALL)
    install(TARGETS cubos-bindings-lua
        EXPORT cubos-bindings-lua-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cubos-bindings-lua)
endif()

# ------------------------- Add samples --------------------------

if(CUBOS_LUA_SAMPLES)
	add_subdirectory(samples)
endif()
