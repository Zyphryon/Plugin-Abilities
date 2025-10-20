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

#include "Marker.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of marker tokens with their associated counts.
    class MarkerSet final
    {
    public:

        /// \brief Inserts a marker token into the set, incrementing its count.
        ///
        /// \param Token The marker token to insert.
        /// \param Count The number of times to insert the marker token.
        ZYPHRYON_INLINE void Insert(Marker Token, UInt32 Count)
        {
            Token.Iterate([this, Count](Marker Child)
            {
                mRegistry[Child] += Count;
            });
        }

        /// \brief Removes a marker token from the set, decrementing its count.
        ///
        /// \param Token The marker token to remove.
        /// \param Count The number of times to remove the marker token.
        ZYPHRYON_INLINE void Remove(Marker Token, UInt32 Count)
        {
            Token.Iterate([this, Count](Marker Child)
            {
                if (const auto Iterator = mRegistry.find(Child); Iterator != mRegistry.end())
                {
                    if (Iterator->second <= Count)
                    {
                        mRegistry.erase(Iterator);
                    }
                    else
                    {
                        Iterator->second -= Count;
                    }
                }
            });
        }

        /// \brief Clears all markers from the set.
        ZYPHRYON_INLINE void Clear()
        {
            mRegistry.clear();
        }

        /// \brief Retrieves the count of a specific marker token in the set.
        ///
        /// \param Token The marker token to retrieve the count for.
        /// \return The count of the specified marker token.
        ZYPHRYON_INLINE UInt32 Count(Marker Token) const
        {
            const auto Iterator = mRegistry.find(Token);
            return Iterator != mRegistry.end() ? Iterator->second : 0;
        }

        /// \brief Checks if the set contains a specific marker token.
        ///
        /// \param Token The marker token to check for.
        /// \return `true` if the marker token is present in the set, `false` otherwise.
        ZYPHRYON_INLINE Bool Contains(Marker Token) const
        {
            return Count(Token) > 0;
        }

        /// \brief Traverses all marker tokens in the set, invoking the provided action for each.
        ///
        /// \param Action The action to invoke for each marker token and its count.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (const auto [Token, Count] : mRegistry)
            {
                Action(Token, Count);
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<Marker, UInt32> mRegistry;
    };
}