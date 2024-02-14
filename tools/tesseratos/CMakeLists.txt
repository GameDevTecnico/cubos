# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

option(BUILD_TESSERATOS_SAMPLES "Build tesseratos samples" OFF)

set(TESSERATOS_SOURCE
	"src/tesseratos/plugin.cpp"
	"src/tesseratos/asset_explorer/plugin.cpp"
	"src/tesseratos/asset_explorer/popup.cpp"
	"src/tesseratos/settings_inspector/plugin.cpp"
	"src/tesseratos/entity_selector/plugin.cpp"
	"src/tesseratos/world_inspector/plugin.cpp"
	"src/tesseratos/entity_inspector/plugin.cpp"
	"src/tesseratos/scene_editor/plugin.cpp"
	"src/tesseratos/debug_camera/plugin.cpp"
	"src/tesseratos/voxel_palette_editor/plugin.cpp"
	"src/tesseratos/toolbox/plugin.cpp"
	"src/tesseratos/transform_gizmo/plugin.cpp"
	"src/tesseratos/metrics_panel/plugin.cpp"
	"src/tesseratos/collider_gizmos/plugin.cpp"
)

add_library(tesseratos ${TESSERATOS_SOURCE})
target_include_directories(tesseratos PUBLIC "include")
target_link_libraries(tesseratos PUBLIC cubos-core cubos-engine)
cubos_common_target_options(tesseratos)

# Add tesseratos samples
if(BUILD_TESSERATOS_SAMPLES)
	add_subdirectory(samples)
endif()
