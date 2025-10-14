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

#include "EffectArchetype.hpp"
#include <Zyphryon.Scene/Entity.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents an instance of an effect applied to an entity.
    class Effect final
    {
    public:

        /// \brief Constructs an effect instance with default values.
        ZYPHRYON_INLINE Effect()
            : mArchetype  { nullptr },
              mHandle     { 0 },
              mStack      { 0 },
              mDuration   { 0 },
              mPeriod     { 0 },
              mIntensity  { 0 },
              mExpiration { 0 },
              mInterval   { 0 }
        {
        }

        /// \brief Constructs an effect instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the effect's properties.
        ZYPHRYON_INLINE explicit Effect(ConstRef<EffectArchetype> Archetype)
            : mArchetype  { & Archetype },
              mHandle     { 0 },
              mStack      { 1 },
              mDuration   { 0 },
              mPeriod     { 0 },
              mIntensity  { 1.0f },
              mExpiration { 0 },
              mInterval   { 0 }
        {
        }

        /// \brief Destruct an effect instance.
        ZYPHRYON_INLINE ~Effect()
        {
            mHandle.Reset();
        }

        /// \brief Checks if the effect instance has a valid handle.
        ///
        /// \return `true` if the effect instance is valid, `false` otherwise.
        ZYPHRYON_INLINE Bool IsValid() const
        {
            return mHandle.IsValid();
        }

        /// \brief Retrieves the archetype associated with this effect instance.
        ///
        /// \return The effect archetype.
        ZYPHRYON_INLINE ConstPtr<EffectArchetype> GetArchetype() const
        {
            return mArchetype;
        }

        /// \brief Sets the unique identifier for this effect instance.
        ///
        /// \param Handle The unique identifier to assign.
        ZYPHRYON_INLINE void SetHandle(EffectHandle Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique identifier of this effect instance.
        ///
        /// \return The effect handle.
        ZYPHRYON_INLINE EffectHandle GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the current stack count of the effect.
        ///
        /// \param Stack The stack count.
        ZYPHRYON_INLINE void SetStack(UInt16 Stack)
        {
            mStack = Stack;
        }

        /// \brief Retrieves the current stack count of the effect.
        ///
        /// \return The stack count.
        ZYPHRYON_INLINE UInt16 GetStack() const
        {
            return mStack;
        }

        /// \brief Sets the total duration of the effect (snapshot) in seconds.
        ///
        /// \param Duration The duration in seconds.
        ZYPHRYON_INLINE void SetDuration(Real32 Duration)
        {
            mDuration = Duration;
        }

        /// \brief Retrieves the total duration of the effect in seconds.
        ///
        /// \return The duration in seconds.
        ZYPHRYON_INLINE Real32 GetDuration() const
        {
            return mDuration;
        }

        /// \brief Sets the period between effect applications (snapshot) in seconds.
        ///
        /// \param Period The period in seconds.
        ZYPHRYON_INLINE void SetPeriod(Real32 Period)
        {
            mPeriod = Period;
        }

        /// \brief Retrieves the period between effect applications in seconds.
        ///
        /// \return The period in seconds.
        ZYPHRYON_INLINE Real32 GetPeriod() const
        {
            return mPeriod;
        }

        /// \brief Sets the intensity of the effect, influencing its strength.
        ///
        /// \param Intensity The intensity value.
        ZYPHRYON_INLINE void SetIntensity(Real32 Intensity)
        {
            mIntensity = Intensity;
        }

        /// \brief Retrieves the intensity of the effect.
        ///
        /// \return The intensity value.
        ZYPHRYON_INLINE Real32 GetIntensity() const
        {
            return mIntensity;
        }

        /// \brief Sets the expiration time of the effect in seconds.
        ///
        /// \param Expiration The expiration time in seconds.
        ZYPHRYON_INLINE void SetExpiration(Real64 Expiration)
        {
            mExpiration = Expiration;
        }

        /// \brief Retrieves the expiration time of the effect in seconds.
        ///
        /// \return The expiration time in seconds.
        ZYPHRYON_INLINE Real64 GetExpiration() const
        {
            return mExpiration;
        }

        /// \brief Sets the next tick time of the effect in seconds.
        ///
        /// \param Interval The next tick time in seconds.
        ZYPHRYON_INLINE void SetInterval(Real64 Interval)
        {
            mInterval = Interval;
        }

        /// \brief Retrieves the next tick time of the effect in seconds.
        ///
        /// \return The next tick time in seconds.
        ZYPHRYON_INLINE Real64 GetInterval() const
        {
            return mInterval;
        }

        /// \brief Sets the entity that instigated the effect.
        ///
        /// \param Instigator The instigating entity.
        ZYPHRYON_INLINE void SetInstigator(Scene::Entity Instigator)
        {
            mInstigator = Instigator;
        }

        /// \brief Retrieves the entity that instigated the effect.
        ///
        /// \return The instigating entity.
        ZYPHRYON_INLINE Scene::Entity GetInstigator() const
        {
            return mInstigator;
        }

        /// \brief Generates a hash value for the effect instance based on its archetype.
        ///
        /// \return A hash value uniquely representing the effect effect.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mArchetype->Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        ConstPtr<EffectArchetype> mArchetype;
        EffectHandle              mHandle;
        UInt16                    mStack;
        Real32                    mDuration;
        Real32                    mPeriod;
        Real32                    mIntensity;
        Real64                    mExpiration;
        Real64                    mInterval;
        Scene::Entity             mInstigator;
    };

}