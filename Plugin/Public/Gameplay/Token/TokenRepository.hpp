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

#include "Gameplay/Token/TokenArchetype.hpp"
#include <Zyphryon.Content/Service.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of token archetypes, allowing loading and saving from/to TOML resources.
    class TokenRepository final
    {
        /// TODO: Reuse tokens from deleted tokens?

    public:

        /// \brief Default constructor, initializes the repository with a root archetype.
        ZYPHRYON_INLINE TokenRepository()
        {
            mArchetypes.emplace();
        }

        /// \brief Loads token archetypes from the content service.
        ///
        /// \param Content  The content service to load from.
        /// \param Filename The filename of the resource to load.
        void Load(Ref<Content::Service> Content, ConstStr8 Filename);

        /// \brief Saves token archetypes to the content service.
        ///
        /// \param Content  The content service to save to.
        /// \param Filename The filename of the resource to save.
        void Save(Ref<Content::Service> Content, ConstStr8 Filename) const;

        /// \brief Inserts a new token archetype into the repository by name and optional parent.
        ///
        /// \param Name   The name of the token archetype to insert.
        /// \param Parent The parent handle.
        void Insert(ConstStr8 Name, Token Parent = Token::kEmpty);

        /// \brief Deletes a token archetype from the repository.
        ///
        /// \param Handle The handle of the token archetype to delete.
        void Delete(Token Handle);

        /// \brief Clears all token archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            // Clear all registered tokens.
            mTokens.clear();

            // Reset the root archetype.
            mArchetypes.clear();
            mArchetypes.emplace();
        }

        /// \brief Retrieves a token by its name.
        ///
        /// \param Name The name of the token to retrieve.
        /// \return The token associated with the given name, or an empty token if not found
        ZYPHRYON_INLINE Token GetByName(ConstStr8 Name) const
        {
            const auto Iterator = mTokens.find(Name);
            return Iterator != mTokens.end() ? Iterator->second : Token::kEmpty;
        }

        /// \brief Retrieves a token archetype by its token.
        ///
        /// \param Handle The handle of the token archetype to retrieve.
        /// \return A constant reference to the requested token archetype.
        ZYPHRYON_INLINE ConstRef<TokenArchetype> Get(Token Handle) const
        {
            const auto Iterator = mArchetypes.find(Handle);
            LOG_ASSERT(Iterator != mArchetypes.end(), "Token not found in repository");

            return * Iterator;
        }

        /// \brief Retrieves all token archetypes in the repository.
        ///
        /// \return A constant span of all token archetypes.
        ZYPHRYON_INLINE ConstSpan<TokenArchetype> GetAll() const
        {
            return mArchetypes;
        }

    private:

        /// \brief Loads token archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing token archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves token archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save token archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

        /// \brief Retrieves a mutable reference to a token archetype by its token.
        ///
        /// \param Handle The handle of the token archetype to retrieve.
        /// \return A reference to the requested token archetype.
        ZYPHRYON_INLINE Ref<TokenArchetype> GetMutable(Token Handle) const
        {
            return const_cast<Ref<TokenArchetype>>(Get(Handle));
        }

        /// \brief Inserts a new token archetype into the repository.
        ///
        /// \param Archetype The token archetype to insert.
        ZYPHRYON_INLINE void Insert(AnyRef<TokenArchetype> Archetype)
        {
            mTokens.emplace(Archetype.GetPath(), Archetype.GetHandle());
            mArchetypes.insert(Move(Archetype));
        }

        /// \brief Recursively loads token archetypes from a TOML array.
        ///
        /// \param Collection The TOML array to load token archetype definitions from.
        /// \param Parent     The parent handle.
        /// \param Prefix     The name prefix for the current hierarchy level.
        void LoadItemRecursive(TOMLArray Collection, Token Parent, ConstStr8 Prefix);

        /// \brief Recursively saves token archetypes to a TOML array.
        ///
        /// \param Collection The TOML array to save token archetype definitions into.
        /// \param Parent     The parent token archetype reference.
        void SaveItemRecursive(TOMLArray Collection, ConstRef<TokenArchetype> Parent) const;

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        ZYPHRYON_INLINE static Ref<TokenRepository> Instance()
        {
            static TokenRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<TokenArchetype> mArchetypes;
        TextTable<Token>    mTokens;
    };
}