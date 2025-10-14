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

#include "EffectArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of effect archetypes, allowing loading and saving from/to TOML resources.
    class EffectRepository final
    {
    public:

        /// \brief Maximum number of effect archetypes that can be registered.
        static constexpr UInt32 kMaxArchetypes = 2'048;  // TODO: Macro Configurable

    public:

        /// \brief Allocates a new effect archetype in the repository.
        ///
        /// \return A reference to the newly allocated effect archetype.
        ZYPHRYON_INLINE Ref<EffectArchetype> Allocate()
        {
            const EffectHandle Handle = mArchetypes.Allocate();

            Ref<EffectArchetype> Archetype = mArchetypes[Handle.GetID()];
            Archetype.SetHandle(Handle);
            return Archetype;
        }

        /// \brief Deletes a effect archetype from the repository.
        ///
        /// \param Archetype The effect archetype to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<EffectArchetype> Archetype)
        {
            LOG_ASSERT(Archetype.GetHandle().IsValid(), "Cannot delete a effect archetype with an invalid handle.");

            mArchetypes.Free(Archetype.GetHandle().GetID());
        }

        /// \brief Fetches a effect archetype by its handle.
        ///
        /// \param Handle The handle of the effect archetype to fetch.
        /// \return The effect archetype associated with the given handle.
        ZYPHRYON_INLINE ConstRef<EffectArchetype> Fetch(EffectHandle Handle) const
        {
            return mArchetypes[Handle.GetID()];
        }

        /// \brief Clears all effect archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            mArchetypes.Clear();
        }

        /// \brief Retrieves all registered effect archetypes.
        ///
        /// \return A span containing all effect archetypes.
        ZYPHRYON_INLINE ConstSpan<EffectArchetype> GetArchetypes() const
        {
            return mArchetypes.GetSpan();
        }

        /// \brief Loads effect archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing effect archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves effect archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save effect archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        static Ref<EffectRepository> Instance()
        {
            static EffectRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Pool<EffectArchetype, kMaxArchetypes> mArchetypes;
    };
}