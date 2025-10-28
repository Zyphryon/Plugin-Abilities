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

#include "Gameplay/Effect/EffectArchetype.hpp"
#include <Zyphryon.Content/Service.hpp>

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
        static constexpr UInt32 kMaxArchetypes = 1'024;  // TODO: Macro Configurable

    public:

        /// \brief Loads effect archetypes from the content service.
        ///
        /// \param Content  The content service to load from.
        /// \param Filename The filename of the resource to load.
        void Load(Ref<Content::Service> Content, ConstStr8 Filename);

        /// \brief Saves effect archetypes to the content service.
        ///
        /// \param Content  The content service to save to.
        /// \param Filename The filename of the resource to save.
        void Save(Ref<Content::Service> Content, ConstStr8 Filename) const;

        /// \brief Allocates a new effect archetype in the repository.
        ///
        /// \return A reference to the newly allocated effect archetype.
        ZYPHRYON_INLINE Ref<EffectArchetype> Allocate()
        {
            const Effect Handle = mArchetypes.Allocate();

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

        /// \brief Clears all effect archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            mArchetypes.Clear();
        }

        /// \brief Retrieves an effect archetype by its handle.
        ///
        /// \param Handle The handle of the effect archetype to retrieve.
        /// \return The effect archetype associated with the given handle.
        ZYPHRYON_INLINE ConstRef<EffectArchetype> Get(Effect Handle) const
        {
            return mArchetypes[Handle.GetID()];
        }

        /// \brief Retrieves all registered effect archetypes.
        ///
        /// \return A span containing all stat archetypes.
        ZYPHRYON_INLINE ConstSpan<EffectArchetype> GetAll() const
        {
            return mArchetypes.GetSpan();
        }

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        ZYPHRYON_INLINE static Ref<EffectRepository> Instance()
        {
            static EffectRepository Singleton;
            return Singleton;
        }

    private:

        /// \brief Loads effect archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing effect archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves effect archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save effect archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Pool<EffectArchetype, kMaxArchetypes> mArchetypes;
    };
}