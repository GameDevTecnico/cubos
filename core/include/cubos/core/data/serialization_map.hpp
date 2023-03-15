#ifndef CUBOS_CORE_DATA_SERIALIZATION_MAP_HPP
#define CUBOS_CORE_DATA_SERIALIZATION_MAP_HPP

#include <unordered_map>
#include <functional>
#include <cassert>

namespace cubos::core::data
{
    /// Class used to map between references and their serialized identifiers.
    /// @tparam R Reference type.
    /// @tparam I Serialized identifier type.
    template <typename R, typename I>
    class SerializationMap final
    {
    public:
        ~SerializationMap() = default;
        SerializationMap(SerializationMap&&) = default;

        inline SerializationMap() : usingFunctions(false)
        {
        }

        /// Makes the serialization map use functions instead of keeping a map.
        /// @param serialize Function used to serialize references.
        /// @param deserialize Function used to deserialize references.
        inline SerializationMap(std::function<bool(const R&, I&)> serialize,
                                std::function<bool(R&, const I&)> deserialize)
            : usingFunctions(true), serialize(serialize), deserialize(deserialize)
        {
        }

        /// Adds a reference <-> serialized identifier pair.
        /// @param reference Reference to add.
        /// @param id Serialized identifier to add.
        inline void add(const R& reference, const I& id)
        {
            assert(!this->usingFunctions);
            this->refToId.insert({reference, id});
            this->idToRef.insert({id, reference});
        }

        /// Checks if a reference is in the map.
        /// @param reference Reference to check.
        /// @return True if the reference is in the map, false otherwise.
        inline bool hasRef(const R& reference) const
        {
            if (this->usingFunctions)
            {
                I i;
                return this->serialize(reference, i);
            }
            else
            {
                return this->refToId.find(reference) != this->refToId.end();
            }
        }

        /// Checks if a serialized identifier is in the map.
        /// @param id Serialized identifier to check.
        /// @return True if the serialized identifier is in the map, false otherwise.
        inline bool hasId(const I& id) const
        {
            if (this->usingFunctions)
            {
                R r;
                return this->deserialize(r, id);
            }
            else
            {
                return this->idToRef.find(id) != this->idToRef.end();
            }
        }

        /// Gets the reference of a serialized identifier.
        /// @param id Serialized identifier to get the reference of.
        /// @returns Reference of the serialized identifier.
        inline R getRef(const I& id) const
        {
            if (this->usingFunctions)
            {
                R reference;
                assert(this->deserialize(reference, id));
                return reference;
            }
            else
            {
                return this->idToRef.at(id);
            }
        }

        /// Gets the serialized identifier of a reference.
        /// @param reference Reference to get the serialized identifier of.
        /// @returns Serialized identifier of the reference.
        inline I getId(const R& reference) const
        {
            if (this->usingFunctions)
            {
                I id;
                assert(this->serialize(reference, id));
                return id;
            }
            else
            {
                return this->refToId.at(reference);
            }
        }

        /// Clears the map.
        inline void clear()
        {
            this->usingFunctions = false;
            this->refToId.clear();
            this->idToRef.clear();
        }

        /// Gets the number of mapped references.
        /// @returns Number of mapped references.
        inline size_t size() const
        {
            return this->refToId.size();
        }

    private:
        SerializationMap(const SerializationMap&) = delete;

        bool usingFunctions;                           ///< True if the map is using functions instead of keeping a map.
        std::function<bool(const R&, I&)> serialize;   ///< Function used to serialize references.
        std::function<bool(R&, const I&)> deserialize; ///< Function used to deserialize references.
        std::unordered_map<R, I> refToId;              ///< Map of references to serialized IDs.
        std::unordered_map<I, R> idToRef;              ///< Map of serialized IDs to references.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_SERIALIZATION_MAP_HPP
