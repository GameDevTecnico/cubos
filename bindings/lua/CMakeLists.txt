# bindings/lua/CMakeLists.txt
# Lua bindings build configuration

# ---------------------- Declare lua bindings library options -----------------------

option(CUBOS_BINDINGS_LUA "Build lua bindings library" ON)

message("# Building cubos::bindings::lua" ${CUBOS_BINDINGS_LUA})

# ----------------------- Define lua bindings source files -----------------------

set(LUA_BINDINGS_SOURCE
    "src/plugin.cpp"
)

# ------------------------ Configure lua bindings target -------------------------

add_library(cubos-bindings-lua ${LUA_BINDINGS_SOURCE})
target_link_libraries(cubos-bindings-lua PRIVATE cubos::engine cubos-api-core cubos-api-engine)
target_include_directories(cubos-bindings-lua PUBLIC "include")
cubos_common_target_options(cubos-bindings-lua)

# ------------- Fetch and link private library dependencies -------------

include(FetchContent)

# Fetch stb_image
FetchContent_Declare(
	lua
	GIT_REPOSITORY "https://github.com/GameDevTecnico/cubos-lua.git"
    GIT_TAG "52ecc4b3052f8f634e90a278d6822f1bed50f724"
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
