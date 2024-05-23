/// @file
/// @brief Class @ref cubos::engine::UIDrawList.
/// @ingroup ui-canvas-plugin

#pragma once
#include <vector>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Holds a collections of draw commands and their data.
    /// @note See the @ref ui-canvas-plugin documentation page for more information.
    /// @ingroup ui-canvas-plugin
    class CUBOS_ENGINE_API UIDrawList final
    {
    public:
        /// @brief Describes a UI draw call type.
        /// @ingroup ui-canvas-plugin
        struct CUBOS_ENGINE_API Type
        {
            static constexpr int MaxTextures = 4;

            cubos::core::gl::ShaderPipeline pipeline; ///< Handle to shader pipeline.
            cubos::core::gl::ConstantBuffer
                constantBuffer; ///< Handle to the constant buffer to hold the per element data.
            cubos::core::gl::ShaderBindingPoint constantBufferBindingPoint; ///< Binding point of the element data.
            size_t perElementSize; ///< Size of the struct holding the element data.
            cubos::core::gl::ShaderBindingPoint texBindingPoint[MaxTextures]; ///< Textures binding points.

            /// @brief Constructs.
            Type();
        };

        /// @brief Describes a UI draw instruction.
        /// @ingroup ui-canvas-plugin
        struct CUBOS_ENGINE_API Command
        {
            cubos::core::gl::VertexArray vertexArray;                    ///< Vertex array handle.
            size_t vertexOffset;                                         ///< Index of the first vertex to be drawn.
            size_t vertexCount;                                          ///< Number of vertexes to draw.
            cubos::core::gl::Sampler samplers[Type::MaxTextures] = {};   ///< Samplers.
            cubos::core::gl::Texture2D textures[Type::MaxTextures] = {}; ///< Textures.
        };

        /// @brief Describes a draw commands and how it fits into the draw list carrying it.
        /// @ingroup ui-canvas-plugin
        struct CUBOS_ENGINE_API Entry
        {
            /// @brief Constructs.
            /// @param commandType Command type.
            explicit Entry(const Type& commandType);

            /// @brief Constructs.
            /// @param commandType Command type.
            /// @param drawCommand UI draw command.
            /// @param dataOffset Position of the command data in the draw list.
            Entry(const Type& commandType, Command drawCommand, size_t dataOffset);

            const Type& type; ///< Command type.
            Command command;  ///< UI draw command.
            size_t offset;    ///< Position of the command data in the draw list.
        };

        /// @brief Builds an Entry.
        /// @ingroup ui-canvas-plugin
        class CUBOS_ENGINE_API EntryBuilder
        {
        public:
            /// @brief Constructs builder with required data.
            /// @param entry Reference to the associated entry.
            EntryBuilder(Entry& entry);

            /// @brief Adds a texture to the entry.
            /// @param texIndex Index of the texture to add.
            /// @param texture Texture to add.
            /// @param sampler Sampler to be used with the texture.
            /// @returns The builder.
            EntryBuilder withTexture(int texIndex, cubos::core::gl::Texture2D texture,
                                     cubos::core::gl::Sampler sampler = nullptr);

        private:
            Entry& mEntry;
        };

        /// @brief Constructs a draw list.
        UIDrawList();

        /// @brief Move constructs.
        /// @param other Other draw list.
        UIDrawList(UIDrawList&& other) noexcept;

        /// @brief Copy constructs.
        /// @param other Other draw list.
        UIDrawList(const UIDrawList& other) noexcept;

        ~UIDrawList();

        /// @brief Adds entry to the draw list.
        /// @param type Command type.
        /// @param command UI draw command.
        /// @param data Command data.
        /// @return A builder for the created entry.
        EntryBuilder push(const Type& type, const Command& command, const void* data);

        /// @brief Creates a new entry in the draw list.
        /// @param type Command type.
        /// @param vertexArray Vertex array handle.
        /// @param vertexOffset Index of the first vertex to be drawn.
        /// @param vertexCount Number of vertexes to draw.
        /// @param data Command data.
        /// @return A builder for the created entry.
        EntryBuilder push(const Type& type, core::gl::VertexArray vertexArray, size_t vertexOffset, size_t vertexCount,
                          const void* data);

        /// @brief Empties draw list.
        void clear();

        /// @brief Gets list size.
        /// @return Number of elements in the list.
        std::size_t size() const;

        /// @brief Gets an entry from the list.
        /// @param i Entry index.
        /// @return A list entry.
        const Entry& entry(std::size_t i) const;

        /// @brief Gets the data of an entry.
        /// @param i Entry index.
        /// @return A pointer to the entry's data.
        const void* data(std::size_t i) const;

    private:
        std::vector<Entry> mEntries; ///< List holding the draw commands of this element and all its children.

        void* mData;                 ///< Buffer holding the data for all the draw commands.
        size_t mDataCapacity = 1024; ///< How much data the data buffer can hold.
        size_t mDataSize = 0;        ///< How much data the data buffer is holding.
    };
} // namespace cubos::engine