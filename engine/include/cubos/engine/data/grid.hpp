#ifndef CUBOS_ENGINE_DATA_GRID_HPP
#define CUBOS_ENGINE_DATA_GRID_HPP

#include <cubos/core/gl/grid.hpp>

#include <cubos/engine/data/loader.hpp>
#include <cubos/engine/gl/renderer.hpp>

namespace cubos::engine::data
{
    namespace impl
    {
        class GridLoader;
    } // namespace impl

    struct Grid
    {
        static constexpr const char* TypeName = "Grid";
        using Loader = impl::GridLoader;

        core::gl::Grid grid;           ///< Raw grid data.
        gl::RendererGrid rendererGrid; ///< Handle of the grid uploaded to the GPU.
    };

    namespace impl
    {
        /// Loader for Grid assets.
        class GridLoader : public Loader
        {
        public:
            GridLoader(AssetManager* manager, gl::Renderer* renderer);
            virtual ~GridLoader() override = default;

            virtual const void* load(const Meta& meta) override;
            virtual std::future<const void*> loadAsync(const Meta& meta) override;
            virtual void unload(const Meta& meta, const void* asset) override;

        private:
            gl::Renderer* renderer;
        };
    } // namespace impl
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_GRID_HPP
