/// @file
/// @brief Resource @ref cubos::engine::Toolbox.
/// @ingroup toolbox-tool-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Manages the visibility of each tool.
    /// @ingroup toolbox-tool-plugin
    class CUBOS_ENGINE_API Toolbox final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Checks if the tool with the given name is open.
        /// @param toolName Tool name.
        ///@return Whether the tool is open.
        bool isOpen(const std::string& toolName);

        /// @brief Opens a tool.
        /// @param toolName Tool name.
        void open(const std::string& toolName);

        /// @brief Closes a tool.
        /// @param toolName Tool name.
        void close(const std::string& toolName);

        /// @brief If the given tool is open, closes it. Otherwise, opens it.
        /// @param toolName Tool name.
        void toggle(const std::string& toolName);

        /// @brief Returns the beginning of the map with known tools.
        /// @return Beginning of the map.
        auto begin() const
        {
            return mToolsMap.begin();
        }

        /// @brief Returns the end of the map with known tools.
        /// @return End of the map.
        auto end() const
        {
            return mToolsMap.end();
        }

    private:
        std::unordered_map<std::string, bool> mToolsMap;
    };
} // namespace cubos::engine
