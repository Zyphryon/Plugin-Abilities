// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2025 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <Zyphryon.Base/Base.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a hierarchical marker used for categorization and identification.
    class Marker final
    {
    public:

        /// \brief Represents an invalid marker key.
        static constexpr UInt64 kEmpty = 0;

        /// \brief Maximum depth of the marker hierarchy.
        static constexpr UInt32 kDepth = 8;

        /// \brief Maximum value for a single node in the marker hierarchy.
        static constexpr UInt32 kLimit = (1 << kDepth) - 1;

    public:

        /// \brief Default constructor, initializes an empty marker.
        ZYPHRYON_INLINE constexpr Marker()
            : mKey { kEmpty }
        {
        }

        /// \brief Constructs a marker from a pre-encoded key.
        ///
        /// \param Key The pre-encoded marker key.
        ZYPHRYON_INLINE constexpr Marker(UInt64 Key)
            : mKey { Key }
        {
        }

        /// \brief Retrieves the encoded identifier of the marker.
        ///
        /// \return The encoded marker identifier.
        ZYPHRYON_INLINE constexpr UInt64 GetID() const
        {
            return mKey;
        }

        /// \brief Checks if the marker is empty.
        ///
        /// \return `true` if the marker is empty, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsEmpty() const
        {
            return mKey == kEmpty;
        }

        /// \brief Checks if the marker is a root marker (has no parent).
        ///
        /// \return `true` if the marker is a root marker, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsRoot() const
        {
            return mKey <= kLimit;
        }

        /// \brief Iterates over each hierarchical level of the marker, invoking the provided action.
        ///
        /// \tparam Function The type of the action to invoke for each level.
        template<typename Function>
        ZYPHRYON_INLINE void Iterate(AnyRef<Function> Action) const
        {
            UInt64 Key = kEmpty;

            for (UInt32 Index = 0; Index < kDepth; ++Index)
            {
                if (const UInt8 Byte = mKey >> (Index * 8) & 0xFF; Byte != 0)
                {
                    Key |= static_cast<UInt64>(Byte) << (Index * 8);
                    Action(Marker(Key));
                }
                else
                {
                    break;
                }
            }
        }

        /// \brief Creates a new marker by appending a child token to the current marker.
        ///
        /// \param Child The child token to append.
        /// \return The new marker with the appended child.
        ZYPHRYON_INLINE constexpr Marker With(UInt8 Child) const
        {
            return Marker(mKey << 8 | static_cast<UInt64>(Child));
        }

        /// \brief Compares two markers for equality based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator==(ConstRef<Marker> Other) const
        {
            return mKey == Other.mKey;
        }

        /// \brief Compares two markers for inequality based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator!=(ConstRef<Marker> Other) const
        {
            return !(*this == Other);
        }

        /// \brief Compares two markers to determine if this marker is less than the other based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator<(ConstRef<Marker> Other) const
        {
            return mKey < Other.mKey;
        }

        /// \brief Generates a hash value for the marker based on its encoded key.
        ///
        /// \return A hash value uniquely representing the marker.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return GetID();
        }

    public:

        /// \brief Creates a marker from a variable number of hierarchical tokens.
        ///
        /// \param Tokens The hierarchical tokens to encode.
        /// \return The constructed marker.
        template<typename... Values>
        ZYPHRYON_INLINE static constexpr Marker From(Values... Tokens)
        {
            static_assert(sizeof...(Tokens) <= kDepth, "Exceeded maximum marker hierarchy depth.");
            return Marker(Encode(std::index_sequence_for<Values...>{ }, static_cast<UInt8>(Tokens)...));
        }

    private:

        /// \brief Helper method to encode hierarchical tokens into a marker.
        ///
        /// \param Tokens  The hierarchical tokens to encode.
        /// \return The constructed marker.
        template<UInt... Indices, typename... Values>
        ZYPHRYON_INLINE static constexpr UInt64 Encode(std::index_sequence<Indices...>, Values... Tokens)
        {
            return ((static_cast<UInt64>(Tokens) << (Indices * 8)) | ...);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        UInt64 mKey;
    };
}