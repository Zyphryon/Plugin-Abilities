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

#include "Gameplay/Ability/AbilityArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of ability archetypes, allowing loading and saving from/to TOML resources.
    class AbilityRepository final
    {
    public:

        /// \brief Maximum number of ability archetypes that can be registered.
        static constexpr UInt32 kMaxArchetypes = 1'024;  // TODO: Macro Configurable

    public:

        /// \brief Allocates a new ability archetype in the repository.
        ///
        /// \return A reference to the newly allocated ability archetype.
        ZYPHRYON_INLINE Ref<AbilityArchetype> Allocate()
        {
            const Ability Handle = mArchetypes.Allocate();

            Ref<AbilityArchetype> Archetype = mArchetypes[Handle.GetID()];
            Archetype.SetHandle(Handle);
            return Archetype;
        }

        /// \brief Deletes a ability archetype from the repository.
        ///
        /// \param Archetype The ability archetype to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<AbilityArchetype> Archetype)
        {
            LOG_ASSERT(Archetype.GetHandle().IsValid(), "Cannot delete an ability archetype with an invalid handle.");

            mArchetypes.Free(Archetype.GetHandle().GetID());
        }

        /// \brief Clears all ability archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            mArchetypes.Clear();
        }

        /// \brief Retrieves an ability archetype by its handle.
        ///
        /// \param Handle The handle of the ability archetype to retrieve.
        /// \return The ability archetype associated with the given handle.
        ZYPHRYON_INLINE ConstRef<AbilityArchetype> Get(Ability Handle) const
        {
            return mArchetypes[Handle.GetID()];
        }

        /// \brief Retrieves all registered ability archetypes.
        ///
        /// \return A span containing all stat archetypes.
        ZYPHRYON_INLINE ConstSpan<AbilityArchetype> GetAll() const
        {
            return mArchetypes.GetSpan();
        }

        /// \brief Loads ability archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing ability archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves ability archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save ability archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        static Ref<AbilityRepository> Instance()
        {
            static AbilityRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Pool<AbilityArchetype, kMaxArchetypes> mArchetypes;
    };
}