#pragma once

#include <functional>
#include <utility>

#include <cubos/core/log.hpp>

namespace cubos::core::data::old
{
    /// Class used to map between references and their serialized identifiers.
    /// @tparam R Reference type.
    /// @tparam I Serialized identifier type.
    /// @tparam RH Reference hash type.
    /// @tparam IH Identifier hash type.
    template <typename R, typename I, typename RH = std::hash<R>, typename IH = std::hash<I>>
    class SerializationMap final
    {
    public:
        ~SerializationMap() = default;
        SerializationMap(SerializationMap&&) noexcept = default;
        SerializationMap(const SerializationMap&) = default;

        inline SerializationMap()
            : mUsingFunctions(false)
        {
        }

        /// Makes the serialization map use functions instead of keeping a map.
        /// @param serialize Function used to serialize references.
        /// @param deserialize Function used to deserialize references.
        inline SerializationMap(std::function<bool(const R&, I&)> serialize,
                                std::function<bool(R&, const I&)> deserialize)
            : mUsingFunctions(true)
            , mSerialize(std::move(serialize))
            , mDeserialize(std::move(deserialize))
        {
        }

        /// Adds a reference <-> serialized identifier pair.
        /// @param reference Reference to add.
        /// @param id Serialized identifier to add.
        inline void add(const R& reference, const I& id)
        {
            CUBOS_ASSERT(!mUsingFunctions);
            mRefToId.insert({reference, id});
            mIdToRef.insert({id, reference});
        }

        /// Checks if a reference is in the map.
        /// @param reference Reference to check.
        /// @return True if the reference is in the map, false otherwise.
        inline bool hasRef(const R& reference) const
        {
            if (mUsingFunctions)
            {
                I i;
                return mSerialize(reference, i);
            }

            return mRefToId.find(reference) != mRefToId.end();
        }

        /// Checks if a serialized identifier is in the map.
        /// @param id Serialized identifier to check.
        /// @return True if the serialized identifier is in the map, false otherwise.
        inline bool hasId(const I& id) const
        {
            if (mUsingFunctions)
            {
                R r;
                return mDeserialize(r, id);
            }

            return mIdToRef.find(id) != mIdToRef.end();
        }

        /// Gets the reference of a serialized identifier.
        /// @param id Serialized identifier to get the reference of.
        /// @return Reference of the serialized identifier.
        inline R getRef(const I& id) const
        {
            if (mUsingFunctions)
            {
                R reference;
                bool success = mDeserialize(reference, id);
                CUBOS_ASSERT(success);
                return reference;
            }

            return mIdToRef.at(id);
        }

        /// Gets the serialized identifier of a reference.
        /// @param reference Reference to get the serialized identifier of.
        /// @return Serialized identifier of the reference.
        inline I getId(const R& reference) const
        {
            if (mUsingFunctions)
            {
                I id;
                bool success = mSerialize(reference, id);
                CUBOS_ASSERT(success);
                return id;
            }

            return mRefToId.at(reference);
        }

        /// Clears the map.
        inline void clear()
        {
            mUsingFunctions = false;
            mRefToId.clear();
            mIdToRef.clear();
        }

        /// Gets the number of mapped references.
        /// @return Number of mapped references.
        inline std::size_t size() const
        {
            return mRefToId.size();
        }

        /// @brief Returns the internal map that maps references to IDs
        /// @return Map of references and Ids.
        inline std::unordered_map<R, I, RH> getMap() const
        {
            return mRefToId;
        }

    private:
        bool mUsingFunctions;                         ///< True if the map is using functions instead of keeping a map.
        std::function<bool(const R&, I&)> mSerialize; ///< Function used to serialize references.
        std::function<bool(R&, const I&)> mDeserialize; ///< Function used to deserialize references.
        std::unordered_map<R, I, RH> mRefToId;          ///< Map of references to serialized IDs.
        std::unordered_map<I, R, IH> mIdToRef;          ///< Map of serialized IDs to references.
    };
} // namespace cubos::core::data::old
