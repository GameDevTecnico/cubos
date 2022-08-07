#ifndef CUBOS_CORE_DATA_JSON_DESERIALIZER_HPP
#define CUBOS_CORE_DATA_JSON_DESERIALIZER_HPP

#include <cubos/core/data/deserializer.hpp>

#include <nlohmann/json.hpp>
#include <stack>

namespace cubos::core::data
{
    /// Implementation of the abstract Deserializer class for deserializing from JSON.
    class JSONDeserializer : public Deserializer
    {
    public:
        /// @param src The string to deserialize from. Must correspond to a JSON literal/object/array.
        JSONDeserializer(const std::string& src);

        // Implement interface methods.

        virtual void readI8(int8_t& value) override;
        virtual void readI16(int16_t& value) override;
        virtual void readI32(int32_t& value) override;
        virtual void readI64(int64_t& value) override;
        virtual void readU8(uint8_t& value) override;
        virtual void readU16(uint16_t& value) override;
        virtual void readU32(uint32_t& value) override;
        virtual void readU64(uint64_t& value) override;
        virtual void readF32(float& value) override;
        virtual void readF64(double& value) override;
        virtual void readBool(bool& value) override;
        virtual void readString(std::string& value) override;
        virtual void beginObject() override;
        virtual void endObject() override;
        virtual size_t beginArray() override;
        virtual void endArray() override;
        virtual size_t beginDictionary() override;
        virtual void endDictionary() override;

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

        std::stack<Frame> frame;     ///< The current frame of the deserializer.
        nlohmann::ordered_json json; ///< The current JSON value being read.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_JSON_DESERIALIZER_HPP
