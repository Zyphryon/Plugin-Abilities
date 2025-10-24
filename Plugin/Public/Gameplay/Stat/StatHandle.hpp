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
    /// \brief A lightweight handle representing a unique stat identifier.
    class StatHandle final
    {
    public:

        /// \brief Default constructor, initializes the stat handle with ID 0.
        ZYPHRYON_INLINE constexpr StatHandle()
            : mID { 0 }
        {
        }

        /// \brief Construct a stat handle with given ID.
        ///
        /// \param ID The unique identifier for the stat.
        template<typename Type>
        ZYPHRYON_INLINE constexpr StatHandle(Type ID)
            : mID { static_cast<UInt16>(ID) }
        {
        }

        /// \brief Check if the stat handle is valid (non-zero ID).
        ///
        /// \return `true` if the stat handle is valid, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsValid() const
        {
            return mID != 0;
        }

        /// \brief Retrieve the unique identifier of the stat handle.
        ///
        /// \return The unique stat ID.
        ZYPHRYON_INLINE constexpr UInt16 GetID() const
        {
            return mID;
        }

        /// \brief Reset the stat handle to an invalid state.
        ZYPHRYON_INLINE constexpr void Reset()
        {
            mID = 0;
        }

        /// \brief Check equality between two stat handles.
        ZYPHRYON_INLINE constexpr Bool operator==(StatHandle Other) const
        {
            return mID == Other.mID;
        }

        /// \brief Check inequality between two stat handles.
        ZYPHRYON_INLINE constexpr Bool operator!=(StatHandle Other) const
        {
            return mID != Other.mID;
        }

        /// \brief Generate a hash value for the stat handle.
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