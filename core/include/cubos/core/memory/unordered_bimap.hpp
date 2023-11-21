/// @file
/// @brief Class @ref cubos::core::memory::UnorderedBimap.
/// @ingroup core-memory

#pragma once

#include <unordered_map>

namespace cubos::core::memory
{
    /// @brief A bidirectional hash table.
    /// @tparam L Left type.
    /// @tparam R Right type.
    /// @tparam LHash Hash functor type for @p L.
    /// @tparam RHash Hash functor type for @p R.
    /// @ingroup core-memory
    template <typename L, typename R, typename LHash = std::hash<L>, typename RHash = std::hash<R>>
    class UnorderedBimap final
    {
    public:
        using Iterator = typename std::unordered_map<L, R, LHash>::const_iterator;

        /// @brief Adds a new entry to the map.
        /// @note If any of the values already exists, the old entries with them are removed.
        /// @param left Left value.
        /// @param right Right value.
        void insert(L left, R right)
        {
            auto leftIt = mLeftToRight.find(left);
            if (leftIt != mLeftToRight.end())
            {
                mRightToLeft.erase(leftIt->second);
            }

            auto rightIt = mRightToLeft.find(right);
            if (rightIt != mRightToLeft.end())
            {
                mLeftToRight.erase(rightIt->second);
            }

            mLeftToRight.insert_or_assign(left, right);
            mRightToLeft.insert_or_assign(right, left);
        }

        /// @brief Removes the entry associated to the given left value.
        /// @param left Left value.
        /// @return Whether the entry was removed.
        bool eraseLeft(const L& left)
        {
            auto it = mLeftToRight.find(left);
            if (it == mLeftToRight.end())
            {
                return false;
            }

            mRightToLeft.erase(it->second);
            mLeftToRight.erase(it);
            return true;
        }

        /// @brief Removes the entry associated to the given right value.
        /// @param right Right value.
        /// @return Whether the entry was removed.
        bool eraseRight(const R& right)
        {
            auto it = mRightToLeft.find(right);
            if (it == mRightToLeft.end())
            {
                return false;
            }

            mLeftToRight.erase(it->second);
            mRightToLeft.erase(it);
            return true;
        }

        /// @brief Checks if the map has the given entry.
        /// @param left Left value.
        /// @param right Right value.
        /// @return Whether the map has the entry.
        bool contains(const L& left, const R& right) const
        {
            auto it = mLeftToRight.find(left);
            if (it == mLeftToRight.end())
            {
                return false;
            }
            return it->second == right;
        }

        /// @brief Checks if the map contains the given left value.
        /// @param left Left value.
        /// @return Whether the map contains the value.
        bool containsLeft(const L& left) const
        {
            return mLeftToRight.contains(left);
        }

        /// @brief Checks if the map contains the given right value.
        /// @param right Right value.
        /// @return Whether the map contains the value.
        bool containsRight(const R& right) const
        {
            return mRightToLeft.contains(right);
        }

        /// @brief Gets the right value associated to the given left value.
        /// @note Aborts if the left value isn't stored.
        /// @param left Left value.
        /// @return Right value.
        const R& atLeft(const L& left) const
        {
            return mLeftToRight.at(left);
        }

        /// @brief Gets the left value associated to the given right value.
        /// @note Aborts if the right value isn't stored.
        /// @param right Right value.
        /// @return Left value.
        const L& atRight(const R& right) const
        {
            return mRightToLeft.at(right);
        }

        /// @brief Clears the map.
        void clear()
        {
            mLeftToRight.clear();
            mRightToLeft.clear();
        }

        /// @brief Gets the number of entries in the map.
        /// @return Entry count.
        std::size_t size() const
        {
            return mLeftToRight.size();
        }

        /// @brief Checks if the map is empty.
        /// @return Whether the map is empty.
        bool empty() const
        {
            return mLeftToRight.empty();
        }

        /// @brief Gets an iterator to the beginning of the map.
        /// @return Iterator.
        Iterator begin() const
        {
            return mLeftToRight.begin();
        }

        /// @brief Gets an iterator to the end of the map.
        /// @return Iterator.
        Iterator end() const
        {
            return mLeftToRight.end();
        }

    private:
        std::unordered_map<L, R, LHash> mLeftToRight;
        std::unordered_map<R, L, RHash> mRightToLeft;
    };
} // namespace cubos::core::memory
