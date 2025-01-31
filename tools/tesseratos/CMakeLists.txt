# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

# ------------------------ Declare tesseratos options -------------------------

option(TESSERATOS_DISTRIBUTE "Build tesseratos for distribution" OFF)

# ---------------------- Define tesseratos source files -----------------------

set(TESSERATOS_SOURCE
	"src/tesseratos/main.cpp"
	"src/tesseratos/menu_bar/plugin.cpp"
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

# ------------------------ Configure tesseratos target ------------------------

add_executable(tesseratos ${TESSERATOS_SOURCE})
target_link_libraries(tesseratos cubos::engine)
cubos_common_target_options(tesseratos)

if(EMSCRIPTEN)
	target_link_options(tesseratos PRIVATE
		"SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/templates@/templates"
		"SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
		"SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
	)
	target_compile_definitions(tesseratos PRIVATE
		TEMPLATES_PATH="/templates"
		APP_ASSETS_PATH="/assets"
		BUILTIN_ASSETS_PATH="/builtin"
	)
else()
	if(TESSERATOS_DISTRIBUTE)
		target_compile_definitions(tesseratos PRIVATE
			TEMPLATES_PATH="templates"
			APP_ASSETS_PATH="assets"
			BUILTIN_ASSETS_PATH="builtin"
		)
	else()
		target_compile_definitions(tesseratos PRIVATE
			TEMPLATES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/templates"
			APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
			BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
		)
	endif()
endif()

# ------------------------- Install quadrados target --------------------------

if(TESSERATOS_DISTRIBUTE)
	install(TARGETS tesseratos RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(DIRECTORY templates/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tesseratos/templates)
	install(DIRECTORY assets/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tesseratos/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tesseratos/builtin)
endif()
