#include <cubos/core/data/des/json.hpp>
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/reflection/comparison.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/type_registry.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::data::JSONSerializer;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::Entity;
using cubos::core::memory::AnyValue;
using cubos::core::memory::Stream;
using cubos::core::reflection::TypeRegistry;

using namespace cubos::engine;

SceneBridge::SceneBridge(TypeRegistry components, TypeRegistry relations)
    : FileBridge(core::reflection::reflect<Scene>())
    , mComponents{std::move(components)}
    , mRelations{std::move(relations)}
{
}

TypeRegistry& SceneBridge::components()
{
    return mComponents;
}

bool SceneBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    // Dump the file contents into a string.
    std::string contents{};
    stream.readUntil(contents, nullptr);

    // Parse the file contents as JSON.
    nlohmann::json json{};
    try
    {
        json = nlohmann::json::parse(contents);
    }
    catch (nlohmann::json::parse_error& e)
    {
        CUBOS_ERROR("{}", e.what());
        return false;
    }

    // Deserialize the JSON into a scene node.
    SceneNode node{};
    if (!node.load(json, mComponents, mRelations))
    {
        CUBOS_ERROR("Failed to load root scene node from JSON");
        return false;
    }

    // Create a new scene from the scene node.
    Scene scene{};
    if (!scene.loadFromNode(std::move(node), assets))
    {
        CUBOS_ERROR("Failed to load scene from root scene node");
        return false;
    }

    // Finally, write the scene to the asset.
    assets.store(handle, std::move(scene));
    return true;
}

bool SceneBridge::saveToFile(const Assets& /*assets*/, const AnyAsset& /*handle*/, Stream& /*stream*/)
{
    CUBOS_ERROR("Saving scenes is not yet implemented");
    return false;
}
