/// @dir
/// @brief @ref Function @ref tesseratos::assetSelectionPopup.

/// @file
/// @brief Utility function to show up a popup containing assets with given type.
/// @ingroup tesseratos-asset-explorer

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>

namespace tesseratos
{
    /// @brief Displays a modal popup to select an asset of a specified type.
    /// @param title Popup title.
    /// @param[out] selectedAsset Output asset.
    /// @param type Asset type to filter by.
    /// @param assets Assets database to query.
    /// @return Whether an asset is selected.
    bool assetSelectionPopup(const std::string& title, cubos::engine::AnyAsset& selectedAsset,
                             const cubos::core::reflection::Type& type, const cubos::engine::Assets& assets);

    /// @brief Displays a modal popup to select an asset of a specified type.
    /// @tparam T Asset type to filter by.
    /// @param title Popup title.
    /// @param[out] selectedAsset Output asset.
    /// @param assets Assets database to query.
    /// @return Whether an asset is selected.
    template <typename T>
    bool assetSelectionPopup(const std::string& title, cubos::engine::AnyAsset& selectedAsset,
                             const cubos::engine::Assets& assets)
    {
        return assetSelectionPopup(title, selectedAsset, cubos::core::reflection::reflect<T>(), assets);
    }
} // namespace tesseratos
