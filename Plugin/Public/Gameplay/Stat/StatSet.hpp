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

#include "Stat.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of stats.
    class StatSet final
    {
    public:

        /// \brief Polls all recorded stat change events and invokes the provided action for each event.
        ///
        /// \param Source The context used to evaluate stat outcomes.
        /// \param Action The action to invoke for each stat change event.
        template<typename Context, typename Function>
        ZYPHRYON_INLINE void Poll(ConstRef<Context> Source, AnyRef<Function> Action)
        {
            for (auto [Handle, Value] : mEvents)
            {
                if (const Real32 Current = Source.GetStat(Handle); Current != Value)
                {
                    Action(Handle, Value, Current);
                }
            }
            mEvents.clear();
        }

        /// \brief Attempts to retrieve a stat by its handle.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return A constant pointer to the stat if found, otherwise nullptr.
        ZYPHRYON_INLINE ConstPtr<Stat> TryGet(StatHandle Handle) const
        {
            if (const auto Iterator = mRegistry.find(Handle); Iterator != mRegistry.end())
            {
                return &* Iterator;
            }
            return nullptr;
        }

        /// \brief Retrieves an existing stat or inserts a new one based on the provided archetype.
        ///
        /// \param Source    The context source used for calculation.
        /// \param Archetype The archetype defining the stat to retrieve or insert.
        /// \return A reference to the retrieved or newly inserted stat.
        template<typename Context>
        ZYPHRYON_INLINE Ref<Stat> GetOrInsert(ConstRef<Context> Source, ConstRef<StatArchetype> Archetype)
        {
            const auto [Iterator, Inserted] = mRegistry.emplace(Archetype);

            if (Inserted)
            {
                Iterator->Calculate(Source);
            }
            return const_cast<Ref<Stat>>(* Iterator);
        }

        /// \brief Clears all stats from the registry.
        ZYPHRYON_INLINE void Clear()
        {
            mRegistry.clear();
        }

        /// \brief Publishes a stat change event for the specified stat handle and previous value.
        ///
        /// \param Handle The handle of the stat that changed.
        /// \param Value  The previous value of the stat before the change.
        ZYPHRYON_INLINE void Publish(StatHandle Handle, Real32 Value)
        {
            mEvents.emplace(Handle, Value);
        }

    private:

        /// \brief Represents a stat change event.
        struct Event
        {
            /// \brief The handle of the stat that changed.
            StatHandle Handle;

            /// \brief The previous value of the stat before the change.
            Real32     Value;

            /// \brief Comparison operator to check equality between two events based on their stat handles.
            ZYPHRYON_INLINE Bool operator==(ConstRef<Event> Other) const
            {
                return Handle == Other.Handle;
            }

            /// \brief Generates a hash value for the event based on its stat handle.
            ZYPHRYON_INLINE UInt64 Hash() const
            {
                return Handle.Hash();
            }
        };

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<Stat>  mRegistry;
        Set<Event> mEvents;
    };
}