# bindings/CMakeLists.txt
# Cubos bindings build configuration

option(CUBOS_BINDINGS_LUA "Build lua bindings library" ON)

if(CUBOS_BINDINGS_LUA)
    add_subdirectory(lua)
endif()
