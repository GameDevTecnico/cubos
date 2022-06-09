#ifndef CUBOS_ENGINE_DATA_PALETTE_HPP
#define CUBOS_ENGINE_DATA_PALETTE_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/core/gl/palette.hpp>

namespace cubos::engine::data
{
    namespace impl
    {
        class PaletteLoader;
    } // namespace impl

    struct Palette
    {
        static constexpr const char* TypeName = "Palette";
        using Loader = impl::PaletteLoader;

        core::gl::Palette palette;
    };

    namespace impl
    {
        /// Loader for Palette assets.
        class PaletteLoader : public Loader
        {
        public:
            using Loader::Loader;
            virtual ~PaletteLoader() override = default;

            virtual const void* load(const Meta& meta) override;
            virtual std::future<const void*> loadAsync(const Meta& meta) override;
            virtual void unload(const Meta& meta, const void* asset) override;
        };
    } // namespace impl
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_PALETTE_HPP
