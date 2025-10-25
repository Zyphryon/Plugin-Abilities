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

#include "AbilityCost.hpp"
#include "AbilityHandle.hpp"
#include "AbilityPolicies.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of an ability, including its properties and behavior policies.
    class AbilityArchetype final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE AbilityArchetype()
            : mHandle { 0 },
              mName   { "" }
        {
        }

        /// \brief Constructs an ability archetype by loading data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE AbilityArchetype(TOMLSection Section)
        {
            Load(Section);
        }

        /// \brief Destruct an ability archetype.
        ZYPHRYON_INLINE ~AbilityArchetype()
        {
            mHandle.Reset();
        }

        /// \brief Checks if the ability archetype has a valid handle.
        ///
        /// \return `true` if the ability archetype is valid, `false` otherwise.
        ZYPHRYON_INLINE Bool IsValid() const
        {
            return mHandle.IsValid();
        }

        /// \brief Sets the unique handle for this ability archetype.
        ///
        /// \param Handle The unique ability handle to assign.
        ZYPHRYON_INLINE void SetHandle(AbilityHandle Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique handle of this ability archetype.
        ///
        /// \return The ability handle.
        ZYPHRYON_INLINE AbilityHandle GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the name of the ability archetype.
        ///
        /// \param Name The name to assign.
        ZYPHRYON_INLINE void SetName(ConstStr8 Name)
        {
            mName = Name;
        }

        /// \brief Retrieves the name of the ability archetype.
        ///
        /// \return The ability name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            return mName;
        }

        /// \brief Loads the ability archetype data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        void Load(TOMLSection Section);

        /// \brief Saves the ability archetype data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        void Save(TOMLSection Section) const;

        /// \brief Generates a hash value for the ability archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the ability archetype.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mHandle.Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        AbilityHandle mHandle;
        Str8          mName;
        // TODO: Policies
        // TODO: Cooldown
        // TODO: Effects
        AbilityCost   mCost;
    };
}