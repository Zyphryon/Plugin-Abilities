// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2025 by Agustin L. Alvarez. All rights reserved.
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
    /// \brief A lightweight handle that uniquely identifies a gameplay stat.
    class Stat final
    {
    public:

        /// \brief Default constructor, initializes an invalid stat.
        ZYPHRYON_INLINE constexpr Stat()
            : mID { 0 }
        {
        }

        /// \brief Constructs a stat handle from a given identifier.
        ///
        /// \param ID The unique identifier for the stat.
        template<typename Type>
        ZYPHRYON_INLINE constexpr Stat(Type ID)
            : mID { static_cast<UInt16>(ID) }
        {
        }

        /// \brief Checks if the stat handle is valid (non-zero ID).
        ///
        /// \return `true` if the stat handle is valid, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsValid() const
        {
            return mID != 0;
        }

        /// \brief Retrieves the identifier of the stat handle.
        ///
        /// \return The stat identifier.
        ZYPHRYON_INLINE constexpr UInt16 GetID() const
        {
            return mID;
        }

        /// \brief Resets the stat handle to an invalid state.
        ZYPHRYON_INLINE constexpr void Reset()
        {
            mID = 0;
        }

        /// \brief Compares two stat handles for equality based on their identifiers.
        ///
        /// \param Other The other stat handle to compare with.
        /// \return `true` if the stat handles are equal, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool operator==(Stat Other) const
        {
            return mID == Other.mID;
        }

        /// \brief Compares two stat handles for inequality based on their identifiers.
        ///
        /// \param Other The other stat handle to compare with.
        /// \return `true` if the stat handles are not equal, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool operator!=(Stat Other) const
        {
            return mID != Other.mID;
        }

        /// \brief Generates a hash value for the stat handle based on its identifier.
        ///
        /// \return A hash value uniquely representing the stat handle.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mID;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        UInt16 mID;
    };
}