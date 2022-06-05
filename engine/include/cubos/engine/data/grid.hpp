#ifndef CUBOS_ENGINE_DATA_GRID_HPP
#define CUBOS_ENGINE_DATA_GRID_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/engine/data/qb_model.hpp>

#include <cubos/core/gl/grid.hpp>

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

        core::gl::Grid grid;
    };

    namespace impl
    {
        /// Loader for Grid assets.
        class GridLoader : public Loader
        {
        public:
            using Loader::Loader;
            virtual ~GridLoader() override = default;

            virtual const void* load(const Meta& meta) override;
            virtual std::future<const void*> loadAsync(const Meta& meta) override;
            virtual void unload(const Meta& meta, const void* asset) override;
        };
    } // namespace impl
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_GRID_HPP
