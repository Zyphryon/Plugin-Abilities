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

#include "EffectHandle.hpp"
#include "EffectPolicies.hpp"
#include "Gameplay/Stat/StatModifier.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of an effect, including its properties and behavior policies.
    class EffectArchetype final
    {
    public:

        /// \brief Maximum number of stat modifiers an effect can have.
        static constexpr UInt32 kMaxBonuses = 8;    // TODO: Macro Configurable

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE EffectArchetype()
            : mHandle       { 0 },
              mName         { "" },
              mDuration     { 0.0f },
              mPeriod       { 0.0f },
              mLimit        { 0 },
              mCategory     { EffectCategory::Temporary },
              mExpiration   { EffectExpiration::Single },
              mRefresh      { EffectRefresh::Replace },
              mResolution   { EffectResolution::Additive },
              mStack        { EffectStack::Linear }
        {
        }

        /// \brief Constructs an effect archetype by loading data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE EffectArchetype(TOMLSection Section)
        {
            Load(Section);
        }

        /// \brief Destruct an effect archetype.
        ZYPHRYON_INLINE ~EffectArchetype()
        {
            mHandle.Reset();
        }

        /// \brief Checks if the effect archetype has a valid handle.
        ///
        /// \return `true` if the effect archetype is valid, `false` otherwise.
        ZYPHRYON_INLINE Bool IsValid() const
        {
            return mHandle.IsValid();
        }

        /// \brief Sets the unique handle for this effect archetype.
        ///
        /// \param Handle The unique effect handle to assign.
        ZYPHRYON_INLINE void SetHandle(EffectHandle Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique handle of this effect archetype.
        ///
        /// \return The effect handle.
        ZYPHRYON_INLINE EffectHandle GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the name of the effect archetype.
        ///
        /// \param Name The name to assign.
        ZYPHRYON_INLINE void SetName(ConstStr8 Name)
        {
            mName = Name;
        }

        /// \brief Retrieves the name of the effect archetype.
        ///
        /// \return The effect name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            return mName;
        }

        /// \brief Sets the total duration of the effect.
        ///
        /// \param Duration The duration to assign.
        ZYPHRYON_INLINE void SetDuration(StatInput Duration)
        {
            mDuration = Move(Duration);
        }

        /// \brief Retrieves the total duration of the effect.
        ///
        /// \return The effect duration.
        ZYPHRYON_INLINE ConstRef<StatInput> GetDuration() const
        {
            return mDuration;
        }

        /// \brief Sets the period between effect applications.
        ///
        /// \param Period The period to assign.
        ZYPHRYON_INLINE void SetPeriod(StatInput Period)
        {
            mPeriod = Move(Period);
        }

        /// \brief Retrieves the period between effect applications.
        ///
        /// \return The effect period.
        ZYPHRYON_INLINE ConstRef<StatInput> GetPeriod() const
        {
            return mPeriod;
        }

        /// \brief Sets the maximum number of stacks for the effect.
        ///
        /// \param Limit The stack limit to assign.
        ZYPHRYON_INLINE void SetLimit(UInt16 Limit)
        {
            mLimit = Limit;
        }

        /// \brief Retrieves the maximum number of stacks for the effect.
        ///
        /// \return The effect stack limit.
        ZYPHRYON_INLINE UInt16 GetLimit() const
        {
            return mLimit;
        }

        /// \brief Sets the category of the effect archetype.
        ///
        /// \param Category The category to assign.
        ZYPHRYON_INLINE void SetCategory(EffectCategory Category)
        {
            mCategory = Category;
        }

        /// \brief Retrieves the category of the effect archetype.
        ///
        /// \return The effect category.
        ZYPHRYON_INLINE EffectCategory GetCategory() const
        {
            return mCategory;
        }

        /// \brief Sets the expiration policy for this effect.
        ///
        /// \param Policy The expiration policy to assign.
        ZYPHRYON_INLINE void SetExpiration(EffectExpiration Policy)
        {
            mExpiration = Policy;
        }

        /// \brief Retrieves the expiration policy of this effect.
        ///
        /// \return The effect expiration policy.
        ZYPHRYON_INLINE EffectExpiration GetExpiration() const
        {
            return mExpiration;
        }

        /// \brief Sets the refresh policy for this effect.
        ///
        /// \param Policy The refresh policy to assign.
        ZYPHRYON_INLINE void SetRefresh(EffectRefresh Policy)
        {
            mRefresh = Policy;
        }

        /// \brief Retrieves the refresh policy of this effect.
        ///
        /// \return The effect refresh policy.
        ZYPHRYON_INLINE EffectRefresh GetRefresh() const
        {
            return mRefresh;
        }

        /// \brief Sets the resolution policy for this effect.
        ///
        /// \param Policy The resolution policy to assign.
        ZYPHRYON_INLINE void SetResolution(EffectResolution Policy)
        {
            mResolution = Policy;
        }

        /// \brief Retrieves the resolution policy of this effect.
        ///
        /// \return The effect resolution policy.
        ZYPHRYON_INLINE EffectResolution GetResolution() const
        {
            return mResolution;
        }

        /// \brief Sets the scaling policy for this effect.
        ///
        /// \param Policy The scaling policy to assign.
        ZYPHRYON_INLINE void SetStack(EffectStack Policy)
        {
            mStack = Policy;
        }

        /// \brief Retrieves the scaling policy of this effect.
        ///
        /// \return The effect scaling policy.
        ZYPHRYON_INLINE EffectStack GetStack() const
        {
            return mStack;
        }

        /// \brief Sets the stat modifiers associated with this effect archetype.
        ///
        /// \param Modifiers A list of stat modifiers to assign.
        ZYPHRYON_INLINE void SetBonuses(ConstSpan<StatModifier> Modifiers)
        {
            mBonuses.assign(Modifiers.begin(), Modifiers.end());
        }

        /// \brief Retrieves the stat modifiers associated with this effect archetype.
        ///
        /// \return A list of stat modifiers.
        ZYPHRYON_INLINE ConstSpan<StatModifier> GetBonuses() const
        {
            return mBonuses;
        }

        /// \brief Iterates over all dependencies referenced by this archetype.
        ///
        /// \param Action The action to apply to each dependency.
        /// \param Origin The origin context of dependencies to consider.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatOrigin Origin) const
        {
            mDuration.Traverse(Action, Origin);
            mPeriod.Traverse(Action, Origin);

            for (ConstRef<StatModifier> Modifier : mBonuses)
            {
                Modifier.Traverse(Action, Origin);
            }
        }

        /// \brief Loads the effect archetype data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        void Load(TOMLSection Section);

        /// \brief Saves the effect archetype data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        void Save(TOMLSection Section) const;

        /// \brief Generates a hash value for the effect archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the effect archetype.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mHandle.Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        EffectHandle                      mHandle;
        Str8                              mName;
        StatInput                         mDuration;
        StatInput                         mPeriod;
        UInt16                            mLimit;
        EffectCategory                    mCategory;            // TODO: Merge Policy
        EffectExpiration                  mExpiration;          // TODO: Merge Policy
        EffectRefresh                     mRefresh;             // TODO: Merge Policy
        EffectResolution                  mResolution;          // TODO: Merge Policy
        EffectStack                       mStack;               // TODO: Merge Policy
        Vector<StatModifier, kMaxBonuses> mBonuses;

        // TODO: Conditions (Has, Not, All, Any => Apply on Stat&Tags)
        // TODO: Categories (Tags?)
    };
}