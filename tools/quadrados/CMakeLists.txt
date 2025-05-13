# tools/quadrados/CMakeLists.txt
# Quadrados build configuration

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

set(QUADRADOS_DEV_TEMPLATE_DIR "${CMAKE_SOURCE_DIR}/tools/tesseratos/templates/simple")
target_compile_definitions(quadrados PRIVATE QUADRADOS_DEV_TEMPLATE_DIR=\"${QUADRADOS_DEV_TEMPLATE_DIR}\")

set(QUADRADOS_INSTALL_TEMPLATE_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/tesseratos/templates/simple")
target_compile_definitions(quadrados PRIVATE QUADRADOS_INSTALL_TEMPLATE_DIR=\"${QUADRADOS_INSTALL_TEMPLATE_DIR}\")

# ------------------------- Install quadrados target --------------------------

install(TARGETS quadrados RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
