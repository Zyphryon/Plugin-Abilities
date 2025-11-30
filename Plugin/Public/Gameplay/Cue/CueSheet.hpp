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
    /// \brief Represents a cue sheet containing a collection of tokens.
    class CueSheet final
    {
    public:

        /// \brief Maximum number of cues allowed in a cue sheet.
        static constexpr UInt32 kMaxCues = 4;

    public:

        /// \brief Default constructor, initializes an empty cue sheet.
        ZYPHRYON_INLINE CueSheet() = default;

        /// \brief Constructs a cue sheet with the provided list of token cues.
        ///
        /// \param Cues The list of token cues to initialize the cue sheet with.
        ZYPHRYON_INLINE CueSheet(List<Token> Cues)
        {
            Insert(Cues);    
        }

        /// \brief Inserts a new token cue into the cue sheet.
        ///
        /// \param Cue The token cue to insert.
        ZYPHRYON_INLINE void Insert(Token Cue)
        {
            LOG_ASSERT(mCues.size() < kMaxCues, "Exceeded maximum number of cues in the sheet.");

            mCues.emplace_back(Cue);
        }

        /// \brief Inserts multiple token cues into the cue sheet.
        ///
        /// \param Cues The list of token cues to insert.
        ZYPHRYON_INLINE void Insert(List<Token> Cues)
        {
            LOG_ASSERT(mCues.size() + Cues.size() <= kMaxCues, "Exceeded maximum number of cues in the sheet.");

            mCues.append_range(Cues);
        }

        /// \brief Removes a token cue from the cue sheet.
        ///
        /// \param Cue The token cue to remove.
        ZYPHRYON_INLINE void Remove(Token Cue)
        {
            mCues.erase(std::ranges::find(mCues, Cue));
        }

        /// \brief Retrieves the list of token cues in the cue sheet.
        ///
        /// \return A constant span of token cues.
        ZYPHRYON_INLINE ConstSpan<Token> GetChildren() const
        {
            return mCues;
        }

        /// \brief Loads the cue sheet from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            for (UInt32 Index = 0; Index < Array.GetSize(); ++Index)
            {
                mCues.emplace_back(Token(Array.GetInteger(Index)));
            }
        }

        /// \brief Saves the cue sheet to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            for (const Token Child : mCues)
            {
                Array.AddInteger(Child.GetID());
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<Token, kMaxCues> mCues;
    };
}