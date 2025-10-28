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

#include "Gameplay/Stat/StatData.hpp"
#include "Gameplay/Stat/StatRepository.hpp"

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
            for (auto [Handle, Value] : mNotifications)
            {
                Real32 Current;

                if (const Ptr<StatData> Instance = const_cast<Ptr<StatData>>(TryGet(Handle)); Instance)
                {
                    // Resolve the stat instance for the current context.
                    Current = Instance->Resolve(Source);
                }
                else
                {
                    Current = StatRepository::Instance().Get(Handle).Calculate(Source, 0.0f, 0.0f, 1.0f);
                }

                if (Current != Value)
                {
                    Action(Handle, Value, Current);
                }
            }
            mNotifications.clear();
        }

        /// \brief Attempts to retrieve a stat by its handle.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return A constant pointer to the stat if found, otherwise nullptr.
        ZYPHRYON_INLINE ConstPtr<StatData> TryGet(Stat Handle) const
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
        ZYPHRYON_INLINE Ref<StatData> GetOrInsert(ConstRef<Context> Source, ConstRef<StatArchetype> Archetype)
        {
            const auto [Iterator, Inserted] = mRegistry.emplace(Archetype);

            Ref<StatData> Instance = const_cast<Ref<StatData>>(* Iterator);

            if (Inserted)
            {
                if (Archetype.GetKind() == StatKind::Attribute)
                {
                    Instance.Resolve(Source);
                }
                else
                {
                    Instance.SetEffective(Source, Archetype.GetBase().Resolve(Source));
                }
            }
            return Instance;
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
        /// \return `true` if the notification was successfully published, `false` if it was already recorded.
        ZYPHRYON_INLINE Bool Publish(Stat Handle, Real32 Value)
        {
            return mNotifications.emplace(Handle, Value).second;
        }

        /// \brief Iterates over all stat instances in the set.
        ///
        /// \param Action The action to apply to each stat instance.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<StatData> Instance : mRegistry)
            {
                Action(Instance);
            }
        }

    private:

        /// \brief Represents a notification of a stat change.
        struct Notification final
        {
            /// \brief The handle of the stat that changed.
            Stat   Key;

            /// \brief The previous value of the stat before the change.
            Real32 Value;

            /// \brief Generates a hash value for the notification based on its stat handle.
            ///
            /// \return A hash value uniquely representing the notification.
            ZYPHRYON_INLINE UInt64 Hash() const
            {
                return Key.Hash();
            }

            /// \brief Compares two notifications for equality based on their stat handles.
            ///
            /// \param Other The other notification to compare against.
            /// \return `true` if the notifications refer to the same stat handle, `false` otherwise.
            ZYPHRYON_INLINE Bool operator==(ConstRef<Notification> Other) const
            {
                return Key == Other.Key;
            }
        };

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<StatData>     mRegistry;
        Set<Notification> mNotifications;
    };
}