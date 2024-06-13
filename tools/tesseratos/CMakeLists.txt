# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

set(TESSERATOS_SOURCE
	"src/tesseratos/main.cpp"
	"src/tesseratos/game/plugin.cpp"
	"src/tesseratos/game/game.cpp"
	"src/tesseratos/game_window/plugin.cpp"
	"src/tesseratos/game_window/window.cpp"
	"src/tesseratos/game_window/render_device.cpp"
	"src/tesseratos/game_imgui/plugin.cpp"
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
	"src/tesseratos/play_pause/plugin.cpp"
	"src/tesseratos/ecs_statistics/plugin.cpp"
)

add_executable(tesseratos ${TESSERATOS_SOURCE})
target_link_libraries(tesseratos PRIVATE cubos-core cubos-engine)
target_compile_definitions(tesseratos PRIVATE TESSERATOS_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
cubos_common_target_options(tesseratos)
