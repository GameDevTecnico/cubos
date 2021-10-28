# core/samples/CMakeLists.txt
# Core samples build configuration

add_executable(render-device render_device.cpp)
target_link_libraries(render-device cubos-core)
