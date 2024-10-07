# tools/tesseratos/CMakeLists.txt
# Tesseratos build configuration

# ------------------------ Declare tesseratos options -------------------------

option(TESSERATOS_DISTRIBUTE "Build tesseratos for distribution" OFF)

# ---------------------- Define tesseratos source files -----------------------

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
)

# ------------------------ Configure tesseratos target ------------------------

add_executable(tesseratos ${TESSERATOS_SOURCE})
target_link_libraries(tesseratos cubos::engine)
cubos_common_target_options(tesseratos)

if (TESSERATOS_DISTRIBUTE)
	target_compile_definitions(tesseratos PRIVATE
		CUBOS_ENGINE_ASSETS_PATH="${CMAKE_INSTALL_FULL_DATADIR}/tesseratos/builtin"
		TESSERATOS_ASSETS_PATH="${CMAKE_INSTALL_FULL_DATADIR}/tesseratos/assets"
	)
else()
	target_compile_definitions(tesseratos PRIVATE
		CUBOS_ENGINE_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
		TESSERATOS_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
	)
endif()

# ------------------------- Install quadrados target --------------------------

if(TESSERATOS_DISTRIBUTE)
	install(TARGETS tesseratos RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(DIRECTORY assets/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tesseratos/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/tesseratos/builtin)
endif()
