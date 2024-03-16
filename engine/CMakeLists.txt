# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)
option(BUILD_ENGINE_TESTS "Build cubos engine tests?" OFF)

message("# Building engine samples: " ${BUILD_ENGINE_SAMPLES})
message("# Building engine tests: " ${BUILD_ENGINE_TESTS})

# Set engine source files
set(CUBOS_ENGINE_SOURCE
	"src/prelude.cpp"

	"src/defaults/plugin.cpp"

	"src/settings/plugin.cpp"
	"src/settings/settings.cpp"

	"src/window/plugin.cpp"

	"src/imgui/plugin.cpp"
	"src/imgui/imgui.cpp"
	"src/imgui/data_inspector.cpp"

	"src/transform/plugin.cpp"
	"src/transform/child_of.cpp"
	"src/transform/local_to_parent.cpp"
	"src/transform/local_to_world.cpp"
	"src/transform/position.cpp"
	"src/transform/rotation.cpp"
	"src/transform/scale.cpp"

	"src/utils/free_camera/plugin.cpp"
	"src/utils/free_camera/controller.cpp"

	"src/assets/plugin.cpp"
	"src/assets/assets.cpp"
	"src/assets/bridge.cpp"
	"src/assets/asset.cpp"
	"src/assets/meta.cpp"
	"src/assets/bridges/file.cpp"

	"src/gizmos/plugin.cpp"
	"src/gizmos/gizmos.cpp"
	"src/gizmos/renderer.cpp"

	"src/scene/plugin.cpp"
	"src/scene/scene.cpp"
	"src/scene/bridge.cpp"

	"src/voxels/plugin.cpp"
	"src/voxels/grid.cpp"
	"src/voxels/material.cpp"
	"src/voxels/palette.cpp"
	"src/voxels/palette_bridge.cpp"

	"src/collisions/plugin.cpp"
	"src/collisions/interface/plugin.cpp"
	"src/collisions/interface/collider.cpp"
	"src/collisions/interface/colliding_with.cpp"
	"src/collisions/interface/shapes/box.cpp"
	"src/collisions/interface/shapes/capsule.cpp"
	"src/collisions/broad_phase/plugin.cpp"
	"src/collisions/broad_phase/sweep_and_prune.cpp"
	"src/collisions/broad_phase/potentially_colliding_with.cpp"
	"src/collisions/narrow_phase/plugin.cpp"

	"src/physics/plugin.cpp"
	"src/physics/gravity.cpp"
	"src/physics/solver.cpp"

	"src/input/plugin.cpp"
	"src/input/input.cpp"
	"src/input/bindings.cpp"
	"src/input/action.cpp"
	"src/input/axis.cpp"

	"src/renderer/plugin.cpp"
	"src/renderer/vertex.cpp"
	"src/renderer/frame.cpp"
	"src/renderer/camera.cpp"
	"src/renderer/environment.cpp"
	"src/renderer/directional_light.cpp"
	"src/renderer/point_light.cpp"
	"src/renderer/spot_light.cpp"
	"src/renderer/renderer.cpp"
	"src/renderer/deferred_renderer.cpp"
	"src/renderer/pps/bloom.cpp"
	"src/renderer/pps/copy_pass.cpp"
	"src/renderer/pps/manager.cpp"
	"src/renderer/pps/pass.cpp"
	"src/renderer/viewport.cpp"

	"src/splitscreen/plugin.cpp"

	"src/screen_picker/plugin.cpp"
	"src/screen_picker/screen_picker.cpp"
	
	"src/fixed_step/plugin.cpp"
	"src/fixed_step/fixed_delta_time.cpp"

	"src/render/shader/plugin.cpp"
	"src/render/shader/shader.cpp"
	"src/render/shader/bridge.cpp"

	"src/render/g_buffer/plugin.cpp"
	"src/render/g_buffer/g_buffer.cpp"
)

# Create cubos engine
add_library(cubos-engine ${CUBOS_ENGINE_SOURCE})
target_include_directories(cubos-engine PUBLIC "include")
target_link_libraries(cubos-engine PUBLIC cubos-core)
cubos_common_target_options(cubos-engine)

add_library(imgui STATIC
	"lib/imgui/imgui.cpp"
	"lib/imgui/imgui_draw.cpp"
	"lib/imgui/imgui_tables.cpp"
	"lib/imgui/imgui_widgets.cpp"
	"lib/imgui/imgui_demo.cpp"
	"lib/imgui/misc/cpp/imgui_stdlib.cpp"
)

add_library(implot STATIC
	"lib/implot/implot.cpp"
	"lib/implot/implot_items.cpp"
	"lib/implot/implot_demo.cpp"
)

target_include_directories(imgui PUBLIC "lib/imgui" "lib/imgui/misc/cpp/imgui_stdlib.hpp")
target_include_directories(implot PUBLIC "lib/implot")
target_link_libraries(implot PUBLIC imgui)
target_link_libraries(cubos-engine PUBLIC implot)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(cubos-engine PUBLIC -Wno-attributes)
endif()

# Add engine tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_ENGINE_TESTS)
	add_subdirectory(tests)
endif()

# Add engine samples
if(BUILD_ENGINE_SAMPLES)
	add_subdirectory(samples)
endif()
