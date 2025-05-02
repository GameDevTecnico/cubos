# engine/CMakeLists.txt
# Cubos engine build configuration

# ---------------------- Declare engine library options -----------------------

option(CUBOS_ENGINE_SAMPLES "Build cubos::engine samples" ${PROJECT_IS_TOP_LEVEL})
option(CUBOS_ENGINE_TESTS "Build cubos::engine tests?" ${PROJECT_IS_TOP_LEVEL})
option(CUBOS_ENGINE_SHARED "Build cubos::engine as shared library?" ON)

message("# Building cubos::engine samples: " ${CUBOS_ENGINE_SAMPLES})
message("# Building cubos::engine tests: " ${CUBOS_ENGINE_TESTS})
message("# Building cubos::engine as shared library: " ${CUBOS_ENGINE_SHARED})

if (EMSCRIPTEN AND CUBOS_ENGINE_SHARED)
	message(WARNING "Cannot build engine as shared library on Emscripten")
	set(CUBOS_ENGINE_SHARED OFF CACHE BOOL "Build cubos::engine as shared library?" FORCE)
endif()

# -------------------- Define engine library source files ---------------------

set(CUBOS_ENGINE_SOURCE
	"src/api.cpp"
	"src/prelude.cpp"

	"src/defaults/plugin.cpp"

	"src/settings/plugin.cpp"
	"src/settings/settings.cpp"

	"src/window/plugin.cpp"

	"src/imgui/plugin.cpp"
	"src/imgui/imgui.cpp"
	"src/imgui/inspector.cpp"
	"src/imgui/context.cpp"

	"src/transform/plugin.cpp"
	"src/transform/child_of.cpp"
	"src/transform/local_to_parent.cpp"
	"src/transform/local_to_world.cpp"
	"src/transform/position.cpp"
	"src/transform/rotation.cpp"
	"src/transform/scale.cpp"

	"src/utils/free_camera/plugin.cpp"
	"src/utils/free_camera/controller.cpp"

	"src/audio/plugin.cpp"
	"src/audio/source.cpp"
	"src/audio/source_impl.cpp"
	"src/audio/listener.cpp"
	"src/audio/listener_impl.cpp"
	"src/audio/pause.cpp"
	"src/audio/play.cpp"
	"src/audio/stop.cpp"

    "src/audio/audio.cpp"
    "src/audio/bridge.cpp"

	"src/assets/plugin.cpp"
	"src/assets/assets.cpp"
	"src/assets/bridge.cpp"
	"src/assets/asset.cpp"
	"src/assets/meta.cpp"
	"src/assets/bridges/file.cpp"

	"src/gizmos/plugin.cpp"
	"src/gizmos/gizmos.cpp"
	"src/gizmos/renderer.cpp"
	"src/gizmos/target.cpp"

	"src/image/bridge.cpp"
	"src/image/image.cpp"
	"src/image/plugin.cpp"

	"src/scene/plugin.cpp"
	"src/scene/scene.cpp"
	"src/scene/bridge.cpp"
	"src/scene/node.cpp"

	"src/voxels/plugin.cpp"
	"src/voxels/grid.cpp"
	"src/voxels/grid_bridge.cpp"
	"src/voxels/material.cpp"
	"src/voxels/palette.cpp"
	"src/voxels/palette_bridge.cpp"
	"src/voxels/model_bridge.cpp"
	"src/voxels/model.cpp"

	"src/collisions/plugin.cpp"
	"src/collisions/interface/plugin.cpp"
	"src/collisions/interface/colliding_with.cpp"
	"src/collisions/interface/intersection_start.cpp"
	"src/collisions/interface/intersection_end.cpp"
	"src/collisions/interface/contact_manifold.cpp"
	"src/collisions/interface/shapes/box.cpp"
	"src/collisions/interface/raycast.cpp"
	"src/collisions/interface/shapes/capsule.cpp"
	"src/collisions/interface/shapes/voxel.cpp"
	"src/collisions/interface/collision_layers.cpp"
	"src/collisions/interface/collision_mask.cpp"
	"src/collisions/interface/collider.cpp"
	"src/collisions/interface/collider_aabb.cpp"
	"src/collisions/interface/collider_bundle.cpp"
	"src/collisions/broad_phase/plugin.cpp"
	"src/collisions/broad_phase/sweep_and_prune.cpp"
	"src/collisions/broad_phase/potentially_colliding_with.cpp"
	"src/collisions/narrow_phase/plugin.cpp"

	"src/physics/plugin.cpp"
	"src/physics/gravity.cpp"
	"src/physics/solver/plugin.cpp"
	"src/physics/constraints/penetration_constraint.cpp"
	"src/physics/constraints/distance_constraint.cpp"
	"src/physics/solver/penetration_constraint/plugin.cpp"
	"src/physics/solver/distance_constraint/plugin.cpp"
	"src/physics/fixed_substep/plugin.cpp"
	"src/physics/fixed_substep/substeps.cpp"
	"src/physics/solver/integration/plugin.cpp"
	"src/physics/components/inertia.cpp"
	"src/physics/components/accumulated_correction.cpp"
	"src/physics/components/mass.cpp"
	"src/physics/components/center_of_mass.cpp"
	"src/physics/components/velocity.cpp"
	"src/physics/components/angular_velocity.cpp"
	"src/physics/components/force.cpp"
	"src/physics/components/torque.cpp"
	"src/physics/components/impulse.cpp"
	"src/physics/components/angular_impulse.cpp"
	"src/physics/components/physics_material.cpp"

	"src/input/plugin.cpp"
	"src/input/input.cpp"
	"src/input/combination.cpp"
	"src/input/bindings.cpp"
	"src/input/action.cpp"
	"src/input/axis.cpp"

	"src/ui/canvas/plugin.cpp"
	"src/ui/canvas/canvas.cpp"
	"src/ui/canvas/draw_list.cpp"
	"src/ui/canvas/keep_pixel_size.cpp"
	"src/ui/canvas/native_aspect_ratio.cpp"
	"src/ui/canvas/match_height.cpp"
	"src/ui/canvas/match_width.cpp"
	"src/ui/canvas/element.cpp"
	"src/ui/canvas/expand.cpp"
	"src/ui/canvas/vertical_stretch.cpp"
	"src/ui/canvas/horizontal_stretch.cpp"
	"src/ui/color_rect/plugin.cpp"
	"src/ui/color_rect/color_rect.cpp"
	"src/ui/image/plugin.cpp"
	"src/ui/image/image.cpp"
	"src/ui/text/plugin.cpp"
    "src/ui/text/text_impl.cpp"
    "src/ui/text/text_stretch.cpp"
	"src/ui/text/text.cpp"

	"src/fixed_step/plugin.cpp"
	"src/fixed_step/fixed_accumulated_time.cpp"
	"src/fixed_step/fixed_delta_time.cpp"

    "src/font/atlas/glyph.cpp"
    "src/font/atlas/bridge.cpp"
    "src/font/atlas/atlas.cpp"
	"src/font/plugin.cpp"
	"src/font/font.cpp"
	"src/font/bridge.cpp"

	"src/render/defaults/plugin.cpp"
	"src/render/defaults/target.cpp"
	"src/render/shader/plugin.cpp"
	"src/render/shader/shader.cpp"
	"src/render/shader/shader_builder.cpp"
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
	"src/render/camera/orthographic.cpp"
	"src/render/camera/perspective.cpp"
	"src/render/camera/draws_to.cpp"
	"src/render/camera/camera.cpp"
	"src/render/voxels/plugin.cpp"
	"src/render/voxels/load.cpp"
	"src/render/voxels/grid.cpp"
	"src/render/voxels/palette.cpp"
	"src/render/tone_mapping/plugin.cpp"
	"src/render/tone_mapping/tone_mapping.cpp"
	"src/render/tone_mapping/fxaa.cpp"
	"src/render/lights/plugin.cpp"
	"src/render/lights/environment.cpp"
	"src/render/lights/directional.cpp"
	"src/render/lights/point.cpp"
	"src/render/lights/spot.cpp"
	"src/render/mesh/plugin.cpp"
	"src/render/mesh/vertex.cpp"
	"src/render/mesh/pool.cpp"
	"src/render/mesh/mesh.cpp"
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
	"src/render/shadows/casters/plugin.cpp"
	"src/render/shadows/casters/caster.cpp"
	"src/render/shadows/casters/spot_caster.cpp"
	"src/render/shadows/casters/directional_caster.cpp"
	"src/render/shadows/casters/point_caster.cpp"
	"src/render/shadows/atlas/plugin.cpp"
	"src/render/shadows/atlas/spot_atlas.cpp"
	"src/render/shadows/atlas/point_atlas.cpp"
	"src/render/shadows/atlas_rasterizer/plugin.cpp"
	"src/render/shadows/atlas_rasterizer/atlas_rasterizer.cpp"
	"src/render/shadows/cascaded/plugin.cpp"
	"src/render/shadows/cascaded_rasterizer/plugin.cpp"
	"src/render/profiling/plugin.cpp"
	"src/render/profiling/profiler.cpp"

	"src/tools/settings_inspector/plugin.cpp"
	"src/tools/selection/plugin.cpp"
	"src/tools/selection/selection.cpp"
	"src/tools/world_inspector/plugin.cpp"
	"src/tools/entity_inspector/plugin.cpp"
	"src/tools/debug_camera/plugin.cpp"
	"src/tools/toolbox/plugin.cpp"
	"src/tools/toolbox/toolbox.cpp"
	"src/tools/transform_gizmo/plugin.cpp"
	"src/tools/metrics_panel/plugin.cpp"
	"src/tools/collider_gizmos/plugin.cpp"
	"src/tools/play_pause/plugin.cpp"
	"src/tools/ecs_statistics/plugin.cpp"
	"src/tools/console/plugin.cpp"
	"src/tools/plugin.cpp"
)

# ---------------------- Configure engine library target ----------------------

if(CUBOS_ENGINE_SHARED)
	add_library(cubos-engine SHARED ${CUBOS_ENGINE_SOURCE})
	target_compile_definitions(cubos-engine
		PRIVATE -DCUBOS_ENGINE_EXPORT # When compiling library source files, we export symbols
		PUBLIC -DCUBOS_ENGINE_IMPORT  # When compiling client source files, we import symbols
	)
else()
	add_library(cubos-engine STATIC ${CUBOS_ENGINE_SOURCE})
endif()
add_library(cubos::engine ALIAS cubos-engine) # Alias so that we can use cubos::engine inside the build tree
set_target_properties(cubos-engine PROPERTIES EXPORT_NAME engine) # Will be within the cubos namespace when exported
cubos_common_target_options(cubos-engine)

target_include_directories(cubos-engine PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/cubos-engine> # On installation, the include files will have been copied here 
)

# Let other targets know the path to the assets folder
set(CUBOS_ENGINE_ASSETS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(CUBOS_ENGINE_ASSETS_PATH ${CUBOS_ENGINE_ASSETS_PATH} PARENT_SCOPE)

# ------------- Fetch and link engine private library dependencies -------------

include(FetchContent)

# Fetch stb_image
FetchContent_Declare(
	stb_image
	GIT_REPOSITORY https://github.com/GameDevTecnico/cubos-stb.git
	GIT_TAG        5c340b5ee24ac74e69bf92b5bacdfbac6bbaa4a8
	SYSTEM
)
FetchContent_MakeAvailable(stb_image)

# We need to define our own stb_image target as the repository doesn't have its own CMakeLists.txt
add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${stb_image_SOURCE_DIR})

# Link stb_image
target_link_libraries(cubos-engine PRIVATE $<BUILD_INTERFACE:stb_image>)

# Fetch Freetype, which msdfgen depends on
FetchContent_Declare(Freetype
	GIT_REPOSITORY "https://github.com/freetype/freetype.git"
	GIT_TAG "VER-2-13-3"
	SYSTEM
)

set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)
if(CUBOS_ENGINE_SHARED OR NOT CUBOS_ENABLE_INSTALL)
	set(SKIP_INSTALL_LIBRARIES ON CACHE BOOL "Don't install Freetype" FORCE)
	set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install Freetype" FORCE)
else()
	set(SKIP_INSTALL_LIBRARIES OFF CACHE BOOL "Install Freetype" FORCE)
	set(SKIP_INSTALL_ALL OFF CACHE BOOL "Install Freetype" FORCE)
endif()

FetchContent_MakeAvailable(Freetype)
set_property(TARGET freetype PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(Freetype::Freetype ALIAS freetype)
if(EMSCRIPTEN)
	target_compile_options(freetype PUBLIC -pthread)
	target_link_options(freetype PUBLIC -pthread)
endif()
# Needed by msdfgen, as Freetype doesn't do this by itself
export(TARGETS freetype FILE "${CMAKE_BINARY_DIR}/FreetypeTargets.cmake")

# Fetch msdfgen, which msdf-atlas-gen depends on
FetchContent_Declare(msdfgen
	GIT_REPOSITORY "https://github.com/GameDevTecnico/cubos-msdfgen.git"
	GIT_TAG "7293c65d49219c0dea71c5c59760c7df7f452e71"
	SYSTEM
)

set(MSDFGEN_BUILD_STANDALONE OFF)
set(MSDFGEN_USE_VCPKG OFF)
set(MSDFGEN_USE_SKIA OFF)
set(MSDFGEN_DISABLE_SVG ON)
set(MSDFGEN_DISABLE_PNG ON)
if(CUBOS_ENGINE_SHARED)
	set(MSDFGEN_INSTALL OFF CACHE BOOL "Don't install msdfgen" FORCE)
	set(MSDFGEN_DYNAMIC_RUNTIME ON CACHE BOOL "Use dynamic runtime" FORCE)
else()
	set(MSDFGEN_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install msdfgen" FORCE)
	set(MSDFGEN_DYNAMIC_RUNTIME OFF CACHE BOOL "Don't use dynamic runtime" FORCE)
endif()

FetchContent_MakeAvailable(msdfgen)
set_property(TARGET msdfgen-core PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET msdfgen-ext PROPERTY POSITION_INDEPENDENT_CODE ON)

# Fetch and link msdf-atlas-gen
FetchContent_Declare(msdf-atlas-gen
    GIT_REPOSITORY "https://github.com/Chlumsky/msdf-atlas-gen.git"
	GIT_TAG "v1.3"
    SYSTEM
)

set(MSDF_ATLAS_BUILD_STANDALONE OFF)
set(MSDF_ATLAS_USE_VCPKG OFF)
set(MSDF_ATLAS_USE_SKIA OFF)
set(MSDF_ATLAS_NO_ARTERY_FONT ON)
set(MSDF_ATLAS_MSDFGEN_EXTERNAL ON)
if(CUBOS_ENGINE_SHARED)
	set(MSDF_ATLAS_INSTALL OFF CACHE BOOL "Don't install msdf-atlas-gen" FORCE)
	set(MSDF_ATLAS_DYNAMIC_RUNTIME ON CACHE BOOL "Use dynamic runtime" FORCE)
else()
	set(MSDF_ATLAS_INSTALL ${CUBOS_ENABLE_INSTALL} CACHE BOOL "Install msdf-atlas-gen" FORCE)
	set(MSDF_ATLAS_DYNAMIC_RUNTIME OFF CACHE BOOL "Don't use dynamic runtime" FORCE)
endif()

FetchContent_MakeAvailable(msdf-atlas-gen)
set_property(TARGET msdf-atlas-gen PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(cubos-engine PRIVATE msdf-atlas-gen)

if(EMSCRIPTEN)
	target_compile_options(msdf-atlas-gen PUBLIC -pthread)
	target_link_options(msdf-atlas-gen PUBLIC -pthread)
endif()

# ------------- Fetch and link engine public library dependencies -------------

# Link our own core library
target_link_libraries(cubos-engine PUBLIC cubos-core)

# Fetch ImGui and ImPlot
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG        v1.89.9-docking
)
FetchContent_Declare(
	implot
	GIT_REPOSITORY https://github.com/epezent/implot.git
	GIT_TAG        1f7a8c0314d838a76695bccebe0f66864f507bc0
)
FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(implot)

# We need to define our own ImGui target as the ImGui repository doesn't have its own CMakeLists.txt
set(IMGUI_SOURCES
	"${imgui_SOURCE_DIR}/imgui.cpp"
	"${imgui_SOURCE_DIR}/imgui_draw.cpp"
	"${imgui_SOURCE_DIR}/imgui_tables.cpp"
	"${imgui_SOURCE_DIR}/imgui_widgets.cpp"
	"${imgui_SOURCE_DIR}/imgui_demo.cpp"
	"${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"

	"${implot_SOURCE_DIR}/implot.cpp"
	"${implot_SOURCE_DIR}/implot_items.cpp"
	"${implot_SOURCE_DIR}/implot_demo.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
set_target_properties(imgui PROPERTIES SYSTEM TRUE)
set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(imgui PUBLIC
	$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
	$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/misc/cpp>
	$<BUILD_INTERFACE:${implot_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include/imgui> # On installation, we copy the include files here
	$<INSTALL_INTERFACE:include/imgui/misc/cpp>
	$<INSTALL_INTERFACE:include/implot>
)

# Finally, link the target we created for both ImGui and Implot
target_link_libraries(cubos-engine PUBLIC imgui)

# ------------------------ Configure tests and samples ------------------------

if(CUBOS_ENGINE_TESTS)
	add_subdirectory(tests)
endif()

if(CUBOS_ENGINE_SAMPLES)
	add_subdirectory(samples)
endif()

# ------------------- Configure engine library installation -------------------

if(CUBOS_ENABLE_INSTALL)
    set(CUBOS_ENGINE_ASSETS_INSTALL_PATH ${CMAKE_INSTALL_DATADIR}/cubos-engine)
    set(CUBOS_ENGINE_ASSETS_INSTALL_PATH ${CUBOS_ENGINE_ASSETS_INSTALL_PATH} PARENT_SCOPE)

	# Install engine target and imgui target
    install(TARGETS cubos-engine imgui
        EXPORT cubos-engine-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cubos-engine)
	install(DIRECTORY ${imgui_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/imgui)
	install(DIRECTORY ${implot_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/implot)
	install(DIRECTORY assets/ DESTINATION ${CUBOS_ENGINE_ASSETS_INSTALL_PATH})
endif()
