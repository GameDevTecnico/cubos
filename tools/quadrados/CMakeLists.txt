# tools/quadrados/CMakeLists.txt
# Quadrados build configuration

# ------------------------ Declare quadrados options -------------------------

option(QUADRADOS_DISTRIBUTE "Build quadrados for distribution" OFF)

# ----------------------- Define quadrados source files -----------------------

set(QUADRADOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
    "src/convert.cpp"
    "src/init.cpp"
)

# ------------------------ Configure quadrados target -------------------------

add_executable(quadrados ${QUADRADOS_SOURCE})
target_link_libraries(quadrados cubos::engine)
cubos_common_target_options(quadrados)


# ----------------------- Set template path variables -----------------------

target_compile_definitions(quadrados PRIVATE QUADRADOS_DISTRIBUTE=$<BOOL:${QUADRADOS_DISTRIBUTE}>)

set(QUADRADOS_INSTALL_PATH "${CMAKE_SOURCE_DIR}/tools/tesseratos/")
target_compile_definitions(quadrados PRIVATE QUADRADOS_INSTALL_PATH=\"${QUADRADOS_INSTALL_PATH}\")

# ------------------------- Install quadrados target --------------------------

install(TARGETS quadrados RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
