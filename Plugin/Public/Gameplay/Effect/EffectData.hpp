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

#include "Gameplay/Effect/EffectArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents an instance of an effect applied to an entity.
    class EffectData final
    {
    public:

        /// \brief Defines the maximum number of bonuses an effect can have.
        static constexpr UInt32 kMaxBonuses = EffectArchetype::kMaxBonuses;

    public:

        /// \brief Constructs an effect instance with default values.
        ZYPHRYON_INLINE EffectData() = default;

        /// \brief Constructs an effect instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the effect's properties.
        ZYPHRYON_INLINE explicit EffectData(ConstRef<EffectArchetype> Archetype)
            : mArchetype  { & Archetype },
              mHandle     { 0 },
              mStack      { 1 },
              mDuration   { 0 },
              mPeriod     { 0 },
              mIntensity  { 1.0f },
              mExpiration { 0 },
              mInterval   { 0 },
              mInstigator { 0 },
              mSnapshot   { }
        {
        }

        /// \brief Destruct an effect instance.
        ZYPHRYON_INLINE ~EffectData()
        {
            mHandle.Reset();
        }

        /// \brief Merges another effect instance into this one, combining their properties.
        ///
        /// \param Other  The other effect instance to merge.
        void Merge(ConstRef<EffectData> Other);

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
        ZYPHRYON_INLINE void SetHandle(Effect Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique identifier of this effect instance.
        ///
        /// \return The effect handle.
        ZYPHRYON_INLINE Effect GetHandle() const
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

        /// \brief Checks if the effect is set to expire.
        ///
        /// \return `true` if the effect has a positive expiration time, `false` otherwise.
        ZYPHRYON_INLINE Bool CanExpire() const
        {
            return mExpiration > 0.0;
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

        /// \brief Checks if the effect is set to tick periodically.
        ///
        /// \return `true` if the effect has a positive period, `false` otherwise.
        ZYPHRYON_INLINE Bool CanTick() const
        {
            return mPeriod > 0.0f;
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

        /// \brief Calculates the effective intensity of the effect based on its stacking behavior.
        ///
        /// \return The effective intensity value.
        ZYPHRYON_INLINE Real32 GetEffectiveIntensity() const
        {
            switch (mArchetype->GetStack())
            {
            case EffectStack::Linear:
                return mIntensity * static_cast<Real32>(mStack);
            case EffectStack::Diminish:
                return 1.0f - Pow(0.5f, static_cast<Real32>(mStack));
            case EffectStack::Exponential:
                return Pow(mIntensity, static_cast<Real32>(mStack));
            default:
                return mIntensity;
            }
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
        ZYPHRYON_INLINE void SetInstigator(UInt64 Instigator)
        {
            mInstigator = Instigator;
        }

        /// \brief Retrieves the entity that instigated the effect.
        ///
        /// \return The instigating entity.
        ZYPHRYON_INLINE UInt64 GetInstigator() const
        {
            return mInstigator;
        }

        /// \brief Sets a snapshot value for the effect.
        ///
        /// \param Index The index of the snapshot to set.
        /// \param Value The value to assign to the snapshot.
        ZYPHRYON_INLINE void SetSnapshot(UInt16 Index, Real32 Value)
        {
            mSnapshot[Index] = Value;
        }

        /// \brief Retrieves a snapshot value for the effect.
        ///
        /// \param Index The index of the snapshot to retrieve.
        /// \return The value of the specified snapshot.
        ZYPHRYON_INLINE Real32 GetSnapshot(UInt16 Index) const
        {
            return mSnapshot[Index];
        }

        /// \brief Generates a hash value for the effect instance based on its archetype.
        ///
        /// \return A hash value uniquely representing the effect effect.
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mArchetype->Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        ConstPtr<EffectArchetype>  mArchetype;
        Effect                     mHandle;
        UInt16                     mStack;       // TODO: Move out of here.
        Real32                     mDuration;    // TODO: Move out of here.
        Real32                     mPeriod;      // TODO: Move out of here.
        Real32                     mIntensity;   // TODO: Move out of here.
        Real64                     mExpiration;  // TODO: Move out of here.
        Real64                     mInterval;    // TODO: Move out of here.
        UInt64                     mInstigator;
        Array<Real32, kMaxBonuses> mSnapshot;
    };
}