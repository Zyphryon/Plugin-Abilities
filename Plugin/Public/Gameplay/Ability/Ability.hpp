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
    /// \brief A lightweight handle representing a unique ability identifier.
    class Ability final
    {
    public:

        /// \brief Default constructor, initializes the ability handle with ID 0.
        ZYPHRYON_INLINE constexpr Ability()
            : mID { 0 }
        {
        }

        /// \brief Construct a ability handle with given ID.
        ///
        /// \param ID The unique identifier for the ability.
        template<typename Type>
        ZYPHRYON_INLINE constexpr Ability(Type ID)
            : mID { static_cast<UInt16>(ID) }
        {
        }

        /// \brief Check if the ability handle is valid (non-zero ID).
        ///
        /// \return `true` if the ability handle is valid, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsValid() const
        {
            return mID != 0;
        }

        /// \brief Retrieve the unique identifier of the ability handle.
        ///
        /// \return The unique ability ID.
        ZYPHRYON_INLINE constexpr UInt16 GetID() const
        {
            return mID;
        }

        /// \brief Reset the ability handle to an invalid abilitye.
        ZYPHRYON_INLINE constexpr void Reset()
        {
            mID = 0;
        }

        /// \brief Check equality between two ability handles.
        ///
        /// \param Other The other ability handle to compare with.
        /// \return `true` if the ability handles are equal, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool operator==(Ability Other) const
        {
            return mID == Other.mID;
        }

        /// \brief Check inequality between two ability handles.
        ///
        /// \param Other The other ability handle to compare with.
        /// \return `true` if the ability handles are not equal, `false` otherwise
        ZYPHRYON_INLINE constexpr Bool operator!=(Ability Other) const
        {
            return mID != Other.mID;
        }

        /// \brief Generate a hash value for the ability handle.
        ///
        /// \return A hash value uniquely representing the ability handle.
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