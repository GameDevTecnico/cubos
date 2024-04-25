# engine/CMakeLists.txt
# Cubos engine build configuration

option(BUILD_ENGINE_SAMPLES "Build cubos engine samples" OFF)
option(BUILD_ENGINE_TESTS "Build cubos engine tests?" OFF)
option(BUILD_ENGINE_SHARED "Build cubos engine as shared library?" ON)

message("# Building engine samples: " ${BUILD_ENGINE_SAMPLES})
message("# Building engine tests: " ${BUILD_ENGINE_TESTS})
message("# Building engine as shared library: " ${BUILD_ENGINE_SHARED})

# Set engine source files
set(CUBOS_ENGINE_SOURCE
	"src/api.cpp"
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
	"src/voxels/grid_bridge.cpp"
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
	"src/render/hdr/plugin.cpp"
	"src/render/hdr/hdr.cpp"
	"src/render/target/plugin.cpp"
	"src/render/target/target.cpp"
	"src/render/picker/plugin.cpp"
	"src/render/picker/picker.cpp"
	"src/render/depth/plugin.cpp"
	"src/render/depth/depth.cpp"
	"src/render/camera/plugin.cpp"
	"src/render/camera/perspective_camera.cpp"
	"src/render/camera/draws_to.cpp"
	"src/render/voxels/plugin.cpp"
	"src/render/voxels/load.cpp"
	"src/render/voxels/grid.cpp"
	"src/render/voxels/palette.cpp"
	"src/render/tone_mapping/plugin.cpp"
	"src/render/tone_mapping/tone_mapping.cpp"
	"src/render/lights/plugin.cpp"
	"src/render/lights/environment.cpp"
	"src/render/lights/directional.cpp"
	"src/render/lights/point.cpp"
	"src/render/lights/spot.cpp"
	"src/render/mesh/plugin.cpp"
	"src/render/mesh/vertex.cpp"
	"src/render/mesh/pool.cpp"
	"src/render/mesh/mesh.cpp"
	"src/render/mesh/task.cpp"
	"src/render/g_buffer_rasterizer/plugin.cpp"
	"src/render/g_buffer_rasterizer/g_buffer_rasterizer.cpp"
	"src/render/ssao/plugin.cpp"
	"src/render/ssao/ssao.cpp"
	"src/render/deferred_shading/plugin.cpp"
	"src/render/deferred_shading/deferred_shading.cpp"
	"src/render/split_screen/plugin.cpp"
	"src/render/split_screen/split_screen.cpp"
	"src/render/bloom/plugin.cpp"
	"src/render/bloom/bloom.cpp"
)

# Create cubos engine
if(BUILD_ENGINE_SHARED)
	add_library(cubos-engine SHARED ${CUBOS_ENGINE_SOURCE})
	target_compile_definitions(cubos-engine
		PRIVATE -DCUBOS_ENGINE_EXPORT
		PUBLIC -DCUBOS_ENGINE_IMPORT
	)
else()
	add_library(cubos-engine STATIC ${CUBOS_ENGINE_SOURCE})
endif()
target_include_directories(cubos-engine PUBLIC "include")
target_link_libraries(cubos-engine PUBLIC cubos-core)
target_compile_definitions(cubos-engine PRIVATE CUBOS_BUILTIN_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
cubos_common_target_options(cubos-engine)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(cubos-engine PUBLIC -Wno-attributes)
endif()

# Add ImGui and ImPlot
set(IMGUI_SOURCES
	"lib/imgui/imgui.cpp"
	"lib/imgui/imgui_draw.cpp"
	"lib/imgui/imgui_tables.cpp"
	"lib/imgui/imgui_widgets.cpp"
	"lib/imgui/imgui_demo.cpp"
	"lib/imgui/misc/cpp/imgui_stdlib.cpp"

	"lib/implot/implot.cpp"
	"lib/implot/implot_items.cpp"
	"lib/implot/implot_demo.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(imgui PUBLIC
	"lib/imgui"
	"lib/imgui/misc/cpp"
	"lib/implot"
)
target_link_libraries(cubos-engine PUBLIC imgui)

# Add engine tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_ENGINE_TESTS)
	add_subdirectory(tests)
endif()

# Add engine samples
if(BUILD_ENGINE_SAMPLES)
	add_subdirectory(samples)
endif()
