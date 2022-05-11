#ifndef CUBOS_CORE_GL_PALETTE_HPP
#define CUBOS_CORE_GL_PALETTE_HPP

#include <cubos/core/gl/material.hpp>

#include <vector>

namespace cubos::core::gl
{
    /// Represents a palette of materials. Supports up to 65535 materials.
    class Palette final
    {
    public:
        /// @param materials The materials to add to the palette.
        Palette(std::vector<Material>&& materials);

        Palette() = default;
        ~Palette() = default;

        /// @return Pointer to the array of materials on the palette.
        const Material* getData() const;

        /// @return The number of materials in the palette.
        uint16_t getSize() const;

        /// @param index The index of the material to get (1-based, 0 is empty).
        /// @return The material at the given index.
        const Material& get(uint16_t index) const;

        /// @param index The index of the material to set (1-based, 0 is empty).
        /// @param material The material to set.
        void set(uint16_t index, const Material& material);

        /// Searches for the index of the material most similar to the given material.
        /// @param material The material to search for.
        /// @return The index of the material.
        uint16_t find(const Material& material);

        /// Serializes the palette.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the palette.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);

    private:
        std::vector<Material> materials; ///< The materials in the palette.
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_PALETTE_HPP
