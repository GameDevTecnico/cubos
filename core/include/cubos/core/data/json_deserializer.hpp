#pragma once

#include <stack>

#include <nlohmann/json.hpp>

#include <cubos/core/data/deserializer.hpp>

namespace cubos::core::old::data
{
    /// Implementation of the abstract Deserializer class for deserializing from JSON.
    class JSONDeserializer : public Deserializer
    {
    public:
        /// @param src The string to deserialize from. Must correspond to a JSON literal/object/array.
        JSONDeserializer(const std::string& src);

        // Implement interface methods.

        void readI8(int8_t& value) override;
        void readI16(int16_t& value) override;
        void readI32(int32_t& value) override;
        void readI64(int64_t& value) override;
        void readU8(uint8_t& value) override;
        void readU16(uint16_t& value) override;
        void readU32(uint32_t& value) override;
        void readU64(uint64_t& value) override;
        void readF32(float& value) override;
        void readF64(double& value) override;
        void readBool(bool& value) override;
        void readString(std::string& value) override;
        void beginObject() override;
        void endObject() override;
        std::size_t beginArray() override;
        void endArray() override;
        std::size_t beginDictionary() override;
        void endDictionary() override;

    private:
        /// The possible states of deserialization.
        enum class Mode
        {
            Object,
            Array,
            Dictionary
        };

        /// The current frame of deserialization.
        struct Frame
        {
            Mode mode;                             ///< The current mode of deserialization.
            nlohmann::ordered_json::iterator iter; ///< The current node.
            bool key;                              ///< Whether the current node is a key.
        };

        std::stack<Frame> mFrame;     ///< The current frame of the deserializer.
        nlohmann::ordered_json mJson; ///< The current JSON value being read.
    };
} // namespace cubos::core::old::data
