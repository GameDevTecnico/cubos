#ifndef CUBOS_ENGINE_DATA_SCENE_HPP
#define CUBOS_ENGINE_DATA_SCENE_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/core/ecs/blueprint.hpp>

namespace cubos::engine::data
{
    namespace impl
    {
        class SceneLoader;
    } // namespace impl

    struct Scene
    {
        static constexpr const char* TypeName = "Scene";
        using Loader = impl::SceneLoader;

        core::ecs::Blueprint blueprint; ///< Blueprint of the scene.
    };

    namespace impl
    {
        /// Loader for Scene assets.
        class SceneLoader : public Loader
        {
        public:
            SceneLoader(AssetManager* manager);
            virtual ~SceneLoader() override = default;

            virtual const void* load(const Meta& meta) override;
            virtual std::future<const void*> loadAsync(const Meta& meta) override;
            virtual void unload(const Meta& meta, const void* asset) override;
        };
    } // namespace impl
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_SCENE_HPP
