# core/tests/CMakeLists.txt
# Core tests build configuration

add_executable(
	cubos-core-tests
	main.cpp
	utils.cpp

	reflection/reflect.cpp
	reflection/comparison.cpp
	reflection/type.cpp
	reflection/type_registry.cpp
	reflection/traits/constructible.cpp
	reflection/traits/fields.cpp
	reflection/traits/nullable.cpp
	reflection/traits/enum.cpp
	reflection/traits/mask.cpp
	reflection/external/primitives.cpp
	reflection/external/cstring.cpp
	reflection/external/string.cpp
	reflection/external/string_view.cpp
	reflection/external/uuid.cpp
	reflection/external/glm.cpp
	reflection/external/vector.cpp
	reflection/external/map.cpp
	reflection/external/unordered_map.cpp

	data/fs/embedded_archive.cpp
	data/fs/standard_archive.cpp
	data/fs/file_system.cpp
	data/ser/debug.cpp
	data/ser/json.cpp
	data/des/json.cpp
	data/context.cpp

	memory/any_value.cpp
	memory/any_vector.cpp
	memory/function.cpp
	memory/type_map.cpp
	memory/unordered_bimap.cpp

	ecs/utils.cpp
	ecs/cubos.cpp
	ecs/world.cpp
	ecs/query.cpp
	ecs/blueprint.cpp

	ecs/commands.cpp
	ecs/system.cpp
	ecs/dispatcher.cpp

	ecs/utils/expected.cpp
	ecs/utils/action.cpp
	ecs/types.cpp
	ecs/table/dense/table.cpp
	ecs/table/sparse_relation/table.cpp
	ecs/entity/archetype_graph.cpp
	ecs/entity/pool.cpp
	ecs/query/data.cpp
	ecs/query/term.cpp
	ecs/query/filter.cpp
	ecs/system/access.cpp
	ecs/system/registry.cpp
	ecs/system/schedule.cpp
	ecs/system/planner.cpp
	ecs/observer/observers.cpp
	ecs/stress.cpp

	geom/box.cpp
	geom/capsule.cpp

	thread/task.cpp
)

target_link_libraries(cubos-core-tests cubos-core doctest::doctest)
cubos_common_target_options(cubos-core-tests)

add_test(NAME cubos-core-tests COMMAND cubos-core-tests)
