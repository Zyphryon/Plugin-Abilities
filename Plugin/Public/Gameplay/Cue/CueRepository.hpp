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

#include "Gameplay/Cue/CueData.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages the subscription and publication of gameplay cues.
    class CueRepository final
    {
    public:

        /// \brief Delegate type for executing cue actions.
        using OnExecuteCue = Delegate<void(ConstRef<CueData>), DelegateInlineSize::Small>;

    public:

        /// \brief Subscribes a delegate to a specific cue token.
        ///
        /// \param Cue      The cue token to subscribe to.
        /// \param Delegate The delegate to invoke when the cue is published.
        ZYPHRYON_INLINE void Subscribe(Token Cue, AnyRef<OnExecuteCue> Delegate)
        {
            mDelegates[Cue] = Move(Delegate);
        }

        /// \brief Publishes a cue event, invoking the associated delegate if subscribed.
        ///
        /// \param Data The cue data to publish.
        ZYPHRYON_INLINE void Publish(ConstRef<CueData> Data)
        {
            if (const auto Iterator = mDelegates.find(Data.GetHandle()); Iterator != mDelegates.end())
            {
                Iterator->second(Data);
            }
        }

        /// \brief Unsubscribes the delegate associated with a specific cue token.
        ///
        /// \param Cue The cue token to unsubscribe from.
        ZYPHRYON_INLINE void Unsubscribe(Token Cue)
        {
            if (const auto Iterator = mDelegates.find(Cue); Iterator != mDelegates.end())
            {
                mDelegates.erase(Iterator);
            }
        }

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return The singleton instance of the repository.
        ZYPHRYON_INLINE static Ref<CueRepository> Instance()
        {
            static CueRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<Token, OnExecuteCue> mDelegates;
    };
}