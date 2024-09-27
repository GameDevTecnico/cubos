# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

set(TESSERATOS_SOURCE
	"src/tesseratos/main.cpp"
	"src/tesseratos/debugger/plugin.cpp"
	"src/tesseratos/debugger/debugger.cpp"
	"src/tesseratos/project/plugin.cpp"
	"src/tesseratos/project/manager.cpp"
	"src/tesseratos/asset_explorer/plugin.cpp"
	"src/tesseratos/asset_explorer/popup.cpp"
	"src/tesseratos/scene_editor/plugin.cpp"
	"src/tesseratos/voxel_palette_editor/plugin.cpp"
	"src/tesseratos/importer/plugin.cpp"
)

add_executable(tesseratos ${TESSERATOS_SOURCE})
target_link_libraries(tesseratos PRIVATE cubos-core cubos-engine)
target_compile_definitions(tesseratos PRIVATE TESSERATOS_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
cubos_common_target_options(tesseratos)
