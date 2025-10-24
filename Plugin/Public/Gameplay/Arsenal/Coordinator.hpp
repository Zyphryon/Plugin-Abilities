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

#include "Gameplay/Effect/Effect.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Coordinates stat and marker modification events within the gameplay system.
    /// TODO: Investigate Batch / ECS way
    class Coordinator final
    {
    public:

        /// \brief Delegate type for stat modification events.
        using OnModifyStatMulticast   = MulticastDelegate<void(UInt64, Real32, Real32), DelegateInlineSize::Small>;

        /// \brief Delegate type for stat modification events.
        using OnModifyStat            = OnModifyStatMulticast::Type;

        /// \brief Delegate type for stat modification events.
        using OnModifyMarkerMulticast = MulticastDelegate<void(UInt64, UInt32, UInt32), DelegateInlineSize::Small>;

        /// \brief Delegate type for stat modification events.
        using OnModifyMarker          = OnModifyMarkerMulticast::Type;

    public:

        /// \brief Subscribes a delegate to stat modification events for a specific stat.
        ///
        /// \param Stat     The handle of the stat to subscribe to.
        /// \param Delegate The delegate to invoke on stat modification.
        ZYPHRYON_INLINE void Subscribe(StatHandle Stat, AnyRef<OnModifyStat> Delegate)
        {
            std::unique_lock Guard(mStatDelegatesMutex);
            mStatDelegates[Stat].Add(Move(Delegate));
        }

        /// \brief Publishes a stat modification event to all subscribed delegates.
        ///
        /// \param Stat     The handle of the modified stat.
        /// \param Entity   The entity whose stat was modified.
        /// \param Previous The previous value of the stat.
        /// \param Current  The current value of the stat.
        ZYPHRYON_INLINE void Publish(StatHandle Stat, UInt64 Entity, Real32 Previous, Real32 Current)
        {
            std::shared_lock Guard(mStatDelegatesMutex);

            if (const auto Iterator = mStatDelegates.find(Stat); Iterator != mStatDelegates.end())
            {
                Iterator->second.Broadcast(Entity, Previous, Current);
            }
        }

        /// \brief Unsubscribes a delegate from stat modification events for a specific stat.
        ///
        /// \param Stat     The handle of the stat to unsubscribe from.
        /// \param Delegate The delegate to remove from the subscription list.
        ZYPHRYON_INLINE void Unsubscribe(StatHandle Stat, ConstRef<OnModifyStat> Delegate)
        {
            std::unique_lock Guard(mStatDelegatesMutex);

            if (const auto Iterator = mStatDelegates.find(Stat); Iterator != mStatDelegates.end())
            {
                Ref<OnModifyStatMulticast> Delegates = Iterator->second;
                Delegates.Remove(Delegate);

                if (Delegates.IsEmpty())
                {
                    mStatDelegates.erase(Iterator);
                }
            }
        }

        /// \brief Subscribes a delegate to marker modification events for a specific marker.
        ///
        /// \param Token    The marker token to subscribe to.
        /// \param Delegate The delegate to invoke on marker modification.
        ZYPHRYON_INLINE void Subscribe(Marker Token, AnyRef<OnModifyMarker> Delegate)
        {
            std::unique_lock Guard(mMarkerDelegatesMutex);
            mMarkerDelegates[Token].Add(Move(Delegate));
        }

        /// \brief Publishes a marker modification event to all subscribed delegates.
        ///
        /// \param Token    The marker token that was modified.
        /// \param Entity   The entity whose marker was modified.
        /// \param Previous The previous count of the marker.
        /// \param Current  The current count of the marker.
        ZYPHRYON_INLINE void Publish(Marker Token, UInt64 Entity, UInt32 Previous, UInt32 Current)
        {
            std::shared_lock Guard(mMarkerDelegatesMutex);

            if (const auto Iterator = mMarkerDelegates.find(Token); Iterator != mMarkerDelegates.end())
            {
                Iterator->second.Broadcast(Entity, Previous, Current);
            }
        }

        /// \brief Unsubscribes a delegate from marker modification events for a specific marker.
        ///
        /// \param Token    The marker token to unsubscribe from.
        /// \param Delegate The delegate to remove from the subscription list.
        ZYPHRYON_INLINE void Unsubscribe(Marker Token, ConstRef<OnModifyMarker> Delegate)
        {
            std::unique_lock Guard(mMarkerDelegatesMutex);

            if (const auto Iterator = mMarkerDelegates.find(Token); Iterator != mMarkerDelegates.end())
            {
                Ref<OnModifyMarkerMulticast> Delegates = Iterator->second;
                Delegates.Remove(Delegate);

                if (Delegates.IsEmpty())
                {
                    mMarkerDelegates.erase(Iterator);
                }
            }
        }

    public:

        /// \brief Retrieves the singleton instance of the Coordinator.
        ///
        /// \return A reference to the singleton Coordinator instance.
        ZYPHRYON_INLINE static Ref<Coordinator> Instance()
        {
            static Coordinator Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<StatHandle, OnModifyStatMulticast> mStatDelegates;
        mutable std::shared_mutex                mStatDelegatesMutex;
        Table<Marker, OnModifyMarkerMulticast>   mMarkerDelegates;
        mutable std::shared_mutex                mMarkerDelegatesMutex;
    };
}