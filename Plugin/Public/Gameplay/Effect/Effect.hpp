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
    /// \brief A lightweight handle representing a unique effect identifier.
    class Effect final
    {
    public:

        /// \brief Default constructor, initializes the effect handle with ID 0.
        ZYPHRYON_INLINE constexpr Effect()
            : mID { 0 }
        {
        }

        /// \brief Construct a effect handle with given ID.
        ///
        /// \param ID The unique identifier for the effect.
        template<typename Type>
        ZYPHRYON_INLINE constexpr Effect(Type ID)
            : mID { static_cast<UInt16>(ID) }
        {
        }

        /// \brief Check if the effect handle is valid (non-zero ID).
        ///
        /// \return `true` if the effect handle is valid, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsValid() const
        {
            return mID != 0;
        }

        /// \brief Retrieve the unique identifier of the effect handle.
        ///
        /// \return The unique effect ID.
        ZYPHRYON_INLINE constexpr UInt16 GetID() const
        {
            return mID;
        }

        /// \brief Reset the effect handle to an invalid state.
        ZYPHRYON_INLINE constexpr void Reset()
        {
            mID = 0;
        }

        /// \brief Check equality between two effect handles.
        ///
        /// \param Other The other effect handle to compare with.
        /// \return `true` if both effect handles are equal, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool operator==(Effect Other) const
        {
            return mID == Other.mID;
        }

        /// \brief Check inequality between two effect handles.
        ///
        /// \param Other The other effect handle to compare with.
        /// \return `true` if both effect handles are not equal, `false` otherwise
        ZYPHRYON_INLINE constexpr Bool operator!=(Effect Other) const
        {
            return mID != Other.mID;
        }

        /// \brief Generate a hash value for the effect handle.
        ///
        /// \return A hash value uniquely representing the effect handle.
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