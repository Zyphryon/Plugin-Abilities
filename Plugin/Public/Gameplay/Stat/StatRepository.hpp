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

#include "StatArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of stat archetypes, allowing loading and saving from/to TOML resources.
    class StatRepository final
    {
    public:

        /// \brief Maximum number of stat archetypes that can be registered.
        static constexpr UInt32 kMaxArchetypes = 256;   // TODO: Macro Configurable

    public:

        /// \brief Allocates a new stat archetype in the repository.
        ///
        /// \return A reference to the newly allocated stat archetype.
        ZYPHRYON_INLINE Ref<StatArchetype> Allocate()
        {
            const StatHandle Handle = mArchetypes.Allocate();

            Ref<StatArchetype> Archetype = mArchetypes[Handle.GetID()];
            Archetype.SetHandle(Handle);
            return Archetype;
        }

        /// \brief Deletes a stat archetype from the repository.
        ///
        /// \param Archetype The stat archetype to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<StatArchetype> Archetype)
        {
            LOG_ASSERT(Archetype.GetHandle().IsValid(), "Cannot delete a stat archetype with an invalid handle.");

            mArchetypes.Free(Archetype.GetHandle().GetID());
        }

        /// \brief Clears all stat archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            mArchetypes.Clear();
        }

        /// \brief Retrieves a stat archetype by its handle.
        ///
        /// \param Handle The handle of the stat archetype to retrieve.
        /// \return The stat archetype associated with the given handle.
        ZYPHRYON_INLINE ConstRef<StatArchetype> Get(StatHandle Handle) const
        {
            return mArchetypes[Handle.GetID()];
        }

        /// \brief Retrieves all registered stat archetypes.
        ///
        /// \return A span containing all stat archetypes.
        ZYPHRYON_INLINE ConstSpan<StatArchetype> GetAll() const
        {
            return mArchetypes.GetSpan();
        }

        /// \brief Loads stat archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing stat archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves stat archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save stat archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        static Ref<StatRepository> Instance()
        {
            static StatRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Pool<StatArchetype, kMaxArchetypes> mArchetypes;
    };
}