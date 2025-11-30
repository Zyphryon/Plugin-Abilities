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
    /// \brief Represents data associated with a gameplay cue event.
    class CueData final
    {
        // TODO: Extra payload info with payload pool = Awesome!

    public:

        /// \brief Defines the types of cue events.
        enum class Event : UInt8
        {
            OnApply,    ///< Event triggered when a cue is applied.
            OnRemove,   ///< Event triggered when a cue is removed.
            OnRefresh,  ///< Event triggered when a cue is refreshed.
        };

    public:

        /// \brief Constructs cue data with the specified parameters.
        ///
        /// \param Handle    The token handle associated with the cue.
        /// \param Event     The type of cue event.
        /// \param Timestmap The timestamp of the cue event.
        /// \param Source    The source entity for the cue event.
        /// \param Target    The target entity for the cue event.
        /// \param Magnitude The magnitude of the cue event.
        ZYPHRYON_INLINE CueData(Token Handle, Event Event, Real64 Timestmap, UInt64 Source, UInt64 Target, Real32 Magnitude)
            : mHandle    { Handle },
              mEvent     { Event },
              mTimestamp { Timestmap },
              mSource    { Source },
              mTarget    { Target },
              mMagnitude { Magnitude }
        {
        }

        /// \brief Retrieves the token handle associated with this cue data.
        ///
        /// \return The token handle.
        ZYPHRYON_INLINE Token GetHandle() const
        {
            return mHandle;
        }

        /// \brief Retrieves the type of cue event.
        ///
        /// \return The cue event type.
        ZYPHRYON_INLINE Event GetEvent() const
        {
            return mEvent;
        }

        /// \brief Retrieves the timestamp of the cue event.
        ///
        /// \return The cue event timestamp.
        ZYPHRYON_INLINE Real64 GetTimestamp() const
        {
            return mTimestamp;
        }

        /// \brief Retrieves the source entity for the cue event.
        ///
        /// \return The source entity.
        ZYPHRYON_INLINE UInt64 GetSource() const
        {
            return mSource;
        }

        /// \brief Retrieves the target entity for the cue event.
        ///
        /// \return The target entity.
        ZYPHRYON_INLINE UInt64 GetTarget() const
        {
            return mTarget;
        }

        /// \brief Retrieves the magnitude of the cue event.
        ///
        /// \return The cue event magnitude.
        ZYPHRYON_INLINE Real32 GetMagnitude() const
        {
            return mMagnitude;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Token  mHandle;
        Event  mEvent;
        Real64 mTimestamp;
        UInt64 mSource;
        UInt64 mTarget;
        Real32 mMagnitude;
    };
}