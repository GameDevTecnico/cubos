#ifndef CUBOS_ENGINE_DATA_LOADER_HPP
#define CUBOS_ENGINE_DATA_LOADER_HPP

#include <cubos/engine/data/meta.hpp>

#include <concepts>
#include <future>

namespace cubos::engine::data
{
    /// Base class of all asset loaders.
    class Loader
    {
    public:
        Loader() = default;
        virtual ~Loader() = default;

        /// Loads an asset, synchronously.
        /// @param meta Asset meta data.
        /// @returns Pointer to asset data.
        virtual const void* load(const Meta& meta) = 0;

        /// Loads an asset, asynchronously.
        /// @param meta Asset meta data.
        /// @returns Pointer to asset data.
        virtual std::future<const void*> loadAsync(const Meta& meta) = 0;

        /// Unloads an asset.
        /// @param meta Asset meta data.
        /// @param asset Pointer to asset data.
        virtual void unload(const Meta& meta, const void* asset) = 0;
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_LOADER_HPP
