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

#include "Gameplay/Token/Token.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a family of tokens grouped together, allowing for organization and categorization.
    class TokenFamily final
    {
    public:

        /// \brief Maximum number of child tokens in a family.
        static constexpr UInt32 kMaxChildren = 4;

    public:

        /// \brief Default constructor, initializes an empty token family.
        ZYPHRYON_INLINE TokenFamily() = default;

        /// \brief Inserts a single child token into the family.
        ///
        /// \param Child The child token to insert.
        ZYPHRYON_INLINE Ref<TokenFamily> Insert(Token Child)
        {
            LOG_ASSERT(mChildren.size() < kMaxChildren, "Exceeded maximum number of children in token family.");

            mChildren.emplace_back(Child);
            return (* this);
        }

        /// \brief Inserts multiple child tokens into the family.
        ///
        /// \param Children The list of child tokens to insert.
        ZYPHRYON_INLINE Ref<TokenFamily> Insert(List<Token> Children)
        {
            LOG_ASSERT(mChildren.size() + Children.size() <= kMaxChildren, "Exceeded maximum number of children in token family.");

            mChildren.append_range(Children);
            return (* this);
        }

        /// \brief Removes a specific child token from the family.
        ///
        /// \param Child The child token to remove.
        ZYPHRYON_INLINE void Remove(Token Child)
        {
            mChildren.erase(std::ranges::find(mChildren, Child));
        }

        /// \brief Checks if the family includes a specific child token.
        ///
        /// \param Child The child token to check for.
        ZYPHRYON_INLINE Bool Includes(Token Child) const
        {
            return std::ranges::find(mChildren, Child) != mChildren.end();
        }

        /// \brief Loads the token family from a TOML array.
        ///
        /// \param Array The TOML array containing token IDs.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            for (UInt32 Index = 0; Index < Array.GetSize(); ++Index)
            {
                mChildren.emplace_back(Token(Array.GetInteger(Index)));
            }
        }

        /// \brief Saves the token family to a TOML array.
        ///
        /// \param Array The TOML array to save token IDs into.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            for (const Token Child : mChildren)
            {
                Array.AddInteger(Child.GetID());
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<Token, kMaxChildren> mChildren;
    };
}