# tools/quadrados/CMakeLists.txt
# Quadrados build configuration

# ----------------------- Define quadrados source files -----------------------

set(QUADRADOS_SOURCE
    "src/tools.hpp"
    "src/entry.cpp"
    "src/embed.cpp"
    "src/convert.cpp"
)

# ------------------------ Configure quadrados target -------------------------

add_executable(quadrados ${QUADRADOS_SOURCE})
target_link_libraries(quadrados cubos::engine)
cubos_common_target_options(quadrados)

# ------------------------- Install quadrados target --------------------------

install(TARGETS quadrados RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
