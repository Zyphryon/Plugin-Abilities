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

        /// \brief Polls for marker change events and invokes the provided action for each event.
        ///
        /// \param Action The action to invoke for each marker change event.
        template<typename Function>
        ZYPHRYON_INLINE void Poll(AnyRef<Function> Action)
        {
            for (auto [Handle, Value] : mEvents)
            {
                if (const UInt32 Current = Count(Handle); Current != Value)
                {
                    Action(Handle, Value, Current);
                }
            }
            mEvents.clear();
        }

        /// \brief Inserts a marker token into the set, incrementing its count.
        ///
        /// \param Token The marker token to insert.
        /// \param Count The number of times to insert the marker token.
        ZYPHRYON_INLINE void Insert(Marker Token, UInt32 Count)
        {
            Token.Iterate([this, Count](Marker Child)
            {
                Ref<UInt32> Value = mRegistry[Child];

                Publish(Child, Value);

                Value += Count;
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
                    Ref<UInt32> Value = mRegistry[Child];

                    Publish(Child, Value);

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

        /// \brief Publishes a marker change event for the specified token and previous value.
        ///
        /// \param Token The marker token that changed.
        /// \param Value The previous value of the marker before the change.
        ZYPHRYON_INLINE void Publish(Marker Token, UInt32 Value)
        {
            mEvents.emplace(Token, Value);
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

        /// \brief Represents a marker change event with its token and previous value.
        struct Event
        {
            /// \brief The marker token that changed.
            Marker Token;

            /// \brief The previous value of the marker before the change.
            UInt32 Value;

            /// \brief Compares two events for equality based on their tokens.
            ZYPHRYON_INLINE Bool operator==(ConstRef<Event> Other) const
            {
                return Token == Other.Token;
            }

            /// \brief Generates a hash value for the event based on its token.
            ZYPHRYON_INLINE UInt64 Hash() const
            {
                return Token.Hash();
            }
        };

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<Marker, UInt32> mRegistry;
        Set<Event>            mEvents;
    };
}