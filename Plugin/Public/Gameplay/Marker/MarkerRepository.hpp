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

#include "MarkerArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of marker archetypes, allowing loading and saving from/to TOML resources.
    class MarkerRepository final
    {
        /// TODO: Reuse tokens from deleted markers?

    public:

        /// \brief Default constructor, initializes the repository.
        ZYPHRYON_INLINE MarkerRepository()
        {
            Clear();
        }

        /// \brief Inserts a new marker archetype into the repository by name and optional parent.
        ///
        /// \param Name   The name of the marker archetype to insert.
        /// \param Parent The parent marker archetype token.
        ZYPHRYON_INLINE void Insert(ConstStr8 Name, Marker Parent = Marker::kEmpty)
        {
            Ref<MarkerArchetype> Archetype = GetMutable(Parent);
            Insert(Archetype.Extend(Name));
        }

        /// \brief Deletes a marker archetype from the repository by its token.
        ///
        /// \param Token The token of the marker archetype to delete.
        ZYPHRYON_INLINE void Delete(Marker Token)
        {
            // Clear the name to unregister the token.
            Ref<MarkerArchetype> Archetype = GetMutable(Token);
            Archetype.SetPath("");

            // Remove the token from the lookup table.
            mTokens.erase(Archetype.GetName());

            // Delete all children recursively.
            for (UInt8 Children = 1; Children <= Archetype.GetArity(); ++Children)
            {
                Delete(Archetype.GetHandle().With(Children));
            }
        }

        /// \brief Clears all marker archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            // Clear all registered tokens.
            mTokens.clear();

            // Reset the root archetype.
            mArchetypes.clear();
            mArchetypes.emplace();
        }

        /// \brief Retrieves a marker token by its name.
        ///
        /// \param Name The name of the marker archetype.
        /// \return The token of the requested marker archetype, or `Marker::kEmpty` if not found.
        ZYPHRYON_INLINE Marker GetTokenByName(ConstStr8 Name) const
        {
            const auto Iterator = mTokens.find(Name);
            return Iterator != mTokens.end() ? Iterator->second : Marker::kEmpty;
        }

        /// \brief Retrieves a marker archetype by its token.
        ///
        /// \param Token The token of the marker archetype to retrieve.
        /// \return A constant reference to the requested marker archetype.
        ZYPHRYON_INLINE ConstRef<MarkerArchetype> Get(Marker Token) const
        {
            const auto Iterator = mArchetypes.find(Token);
            LOG_ASSERT(Iterator != mArchetypes.end(), "Marker not found in repository");

            return * Iterator;
        }

        /// \brief Retrieves all marker archetypes in the repository.
        ///
        /// \return A constant span of all marker archetypes.
        ZYPHRYON_INLINE ConstSpan<MarkerArchetype> GetAll() const
        {
            return mArchetypes;
        }

        /// \brief Loads marker archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing marker archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves marker archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save marker archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

    private:

        /// \brief Retrieves a mutable reference to a marker archetype by its token.
        ///
        /// \param Token The token of the marker archetype to retrieve.
        /// \return A reference to the requested marker archetype.
        ZYPHRYON_INLINE Ref<MarkerArchetype> GetMutable(Marker Token)
        {
            return const_cast<Ref<MarkerArchetype>>(Get(Token));
        }

        /// \brief Inserts a new marker archetype into the repository.
        ///
        /// \param Archetype The marker archetype to insert.
        ZYPHRYON_INLINE void Insert(AnyRef<MarkerArchetype> Archetype)
        {
            mArchetypes.insert(Move(Archetype));
            mTokens.emplace(Archetype.GetName(), Archetype.GetHandle());
        }

        /// \brief Recursively loads marker archetypes from a TOML array.
        ///
        /// \param Collection The TOML array to load marker archetype definitions from.
        /// \param Parent     The parent marker token.
        /// \param Prefix     The name prefix for the current hierarchy level.
        void LoadItemRecursive(TOMLArray Collection, Marker Parent, ConstStr8 Prefix);

        /// \brief Recursively saves marker archetypes to a TOML array.
        ///
        /// \param Collection The TOML array to save marker archetype definitions into.
        /// \param Parent     The parent marker archetype reference.
        void SaveItemRecursive(TOMLArray Collection, ConstRef<MarkerArchetype> Parent) const;

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        static Ref<MarkerRepository> Instance()
        {
            static MarkerRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<MarkerArchetype> mArchetypes;
        TextTable<Marker>    mTokens;
    };
}