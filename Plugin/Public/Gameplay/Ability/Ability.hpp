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

#include "AbilityArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents an instance of an ability.
    class Ability final
    {
    public:

        /// \brief Constructs an ability instance with default values.
        ZYPHRYON_INLINE Ability()
            : mArchetype { nullptr },
              mTime      { 0.0 }
        {
        }

        /// \brief Constructs an ability instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the ability's properties.
        ZYPHRYON_INLINE explicit Ability(ConstRef<AbilityArchetype> Archetype)
            : mArchetype { & Archetype },
              mTime      { 0.0 }
        {
        }

        /// \brief Retrieves the archetype associated with this ability instance.
        ///
        /// \return The ability archetype.
        ZYPHRYON_INLINE ConstPtr<AbilityArchetype> GetArchetype() const
        {
            return mArchetype;
        }

        /// \brief Sets the elapsed time since the ability was activated.
        ///
        /// \param Time The elapsed time to set.
        ZYPHRYON_INLINE void SetTime(Real64 Time)
        {
            mTime = Time;
        }

        /// \brief Retrieves the elapsed time since the ability was activated.
        ///
        /// \return The elapsed time.
        ZYPHRYON_INLINE Real64 GetTime() const
        {
            return mTime;
        }

        /// \brief Checks equality between this ability instance and another ability handle.
        ZYPHRYON_INLINE Bool operator==(AbilityHandle Handle) const
        {
            return mArchetype->GetHandle() == Handle;
        }

        /// \brief Checks equality between this ability instance and another ability archetype.
        ZYPHRYON_INLINE Bool operator==(ConstRef<AbilityArchetype> Archetype) const
        {
            return mArchetype->GetHandle() == Archetype.GetHandle();
        }

        /// \brief Generates a hash value for the ability instance based on its archetype.
        ///
        /// \return A hash value uniquely representing the ability ability.
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mArchetype->Hash();
        }

        // TODO: Activation / Resource Management / Etc

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        ConstPtr<AbilityArchetype> mArchetype;
        Real64                     mTime;
        // TODO: Charges?
    };
}