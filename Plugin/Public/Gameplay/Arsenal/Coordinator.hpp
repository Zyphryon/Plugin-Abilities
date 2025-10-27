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

#include "Gameplay/Stat/Stat.hpp"
#include "Gameplay/Token/Token.hpp"
#include <Zyphryon.Scene/Entity.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Coordinates stat and marker modification events within the gameplay system.
    /// TODO: Investigate Batch / ECS way
    /// TODO: Consider using Event Bus pattern?
    class Coordinator final
    {
    public:

        /// \brief Multicast delegate type for stat modification events.
        using OnModifyStatMulticast   = MulticastDelegate<void(Scene::Entity, Real32, Real32), DelegateInlineSize::Small>;

        /// \brief Delegate type for stat modification events.
        using OnModifyStat            = OnModifyStatMulticast::Type;

        /// \brief Delegate type for token modification events.
        using OnModifyTokenMulticast = MulticastDelegate<void(Scene::Entity, UInt32, UInt32), DelegateInlineSize::Small>;

        /// \brief Multicast delegate type for token modification events.
        using OnModifyToken          = OnModifyTokenMulticast::Type;

    public:

        /// \brief Subscribes a delegate to stat modification events for a specific stat.
        ///
        /// \param Target   The handle of the stat to subscribe to.
        /// \param Delegate The delegate to invoke on stat modification.
        ZYPHRYON_INLINE void Subscribe(Stat Target, AnyRef<OnModifyStat> Delegate)
        {
            std::unique_lock Guard(mStatDelegatesMutex);
            mStatDelegates[Target].Add(Move(Delegate));
        }

        /// \brief Publishes a stat modification event to all subscribed delegates.
        ///
        /// \param Target   The handle of the modified stat.
        /// \param Entity   The entity whose stat was modified.
        /// \param Previous The previous value of the stat.
        /// \param Current  The current value of the stat.
        ZYPHRYON_INLINE void Publish(Stat Target, Scene::Entity Entity, Real32 Previous, Real32 Current)
        {
            std::shared_lock Guard(mStatDelegatesMutex);

            if (const auto Iterator = mStatDelegates.find(Target); Iterator != mStatDelegates.end())
            {
                Iterator->second.Broadcast(Entity, Previous, Current);
            }
        }

        /// \brief Unsubscribes a delegate from stat modification events for a specific stat.
        ///
        /// \param Target   The handle of the stat to unsubscribe from.
        /// \param Delegate The delegate to remove from the subscription list.
        ZYPHRYON_INLINE void Unsubscribe(Stat Target, ConstRef<OnModifyStat> Delegate)
        {
            std::unique_lock Guard(mStatDelegatesMutex);

            if (const auto Iterator = mStatDelegates.find(Target); Iterator != mStatDelegates.end())
            {
                Ref<OnModifyStatMulticast> Delegates = Iterator->second;
                Delegates.Remove(Delegate);

                if (Delegates.IsEmpty())
                {
                    mStatDelegates.erase(Iterator);
                }
            }
        }

        /// \brief Subscribes a delegate to token modification events for a specific token.
        ///
        /// \param Target   The handle of the token to subscribe to.
        /// \param Delegate The delegate to invoke on token modification.
        ZYPHRYON_INLINE void Subscribe(Token Target, AnyRef<OnModifyToken> Delegate)
        {
            std::unique_lock Guard(mTokenDelegatesMutex);
            mTokenDelegates[Target].Add(Move(Delegate));
        }

        /// \brief Publishes a token modification event to all subscribed delegates.
        ///
        /// \param Target   The handle of the modified token.
        /// \param Entity   The entity whose token was modified.
        /// \param Previous The previous count of the token.
        /// \param Current  The current count of the token.
        ZYPHRYON_INLINE void Publish(Token Target, Scene::Entity Entity, UInt32 Previous, UInt32 Current)
        {
            std::shared_lock Guard(mTokenDelegatesMutex);

            if (const auto Iterator = mTokenDelegates.find(Target); Iterator != mTokenDelegates.end())
            {
                Iterator->second.Broadcast(Entity, Previous, Current);
            }
        }

        /// \brief Unsubscribes a delegate from token modification events for a specific token.
        ///
        /// \param Target   The handle of the token to unsubscribe from.
        /// \param Delegate The delegate to remove from the subscription list.
        ZYPHRYON_INLINE void Unsubscribe(Token Target, ConstRef<OnModifyToken> Delegate)
        {
            std::unique_lock Guard(mTokenDelegatesMutex);

            if (const auto Iterator = mTokenDelegates.find(Target); Iterator != mTokenDelegates.end())
            {
                Ref<OnModifyTokenMulticast> Delegates = Iterator->second;
                Delegates.Remove(Delegate);

                if (Delegates.IsEmpty())
                {
                    mTokenDelegates.erase(Iterator);
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

        Table<Stat, OnModifyStatMulticast>   mStatDelegates;
        mutable std::shared_mutex            mStatDelegatesMutex;
        Table<Token, OnModifyTokenMulticast> mTokenDelegates;
        mutable std::shared_mutex            mTokenDelegatesMutex;
    };
}