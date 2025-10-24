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

#include "StatArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents an instance of a stat with its modifiers and effective value.
    class Stat final
    {
    public:

        /// \brief Constructs a stat instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the stat's properties.
        ZYPHRYON_INLINE Stat(ConstRef<StatArchetype> Archetype)
            : mArchetype  { & Archetype },
              mFlat       { 0.0f },
              mAdditive   { 0.0f },
              mMultiplier { 1.0f },
              mEffective  { 0.0f },
              mDirty      { true }
        {
        }

        /// \brief Marks the stat as dirty, indicating that its effective value needs to be recalculated.
        ZYPHRYON_INLINE void SetDirty() const
        {
            mDirty = true;
        }

        /// \brief Checks if the stat's effective value is marked as dirty and needs recalculation.
        ///
        /// \return `true` if the stat is dirty, `false` otherwise.
        ZYPHRYON_INLINE Bool IsDirty() const
        {
            return mDirty;
        }

        /// \brief Retrieves the archetype associated with this stat instance.
        ///
        /// \return The stat archetype.
        ZYPHRYON_INLINE ConstPtr<StatArchetype> GetArchetype() const
        {
            return mArchetype;
        }

        /// \brief Sets the flat modifier for this stat.
        ///
        /// \param Flat The flat value to assign.
        ZYPHRYON_INLINE void SetFlat(Real32 Flat)
        {
            mFlat  = Flat;
            SetDirty();
        }

        /// \brief Retrieves the flat modifier of this stat.
        ///
        /// \return The flat modifier value.
        ZYPHRYON_INLINE Real32 GetFlat() const
        {
            return mFlat;
        }

        /// \brief Sets the additive modifier for this stat.
        ///
        /// \param Additive The additive value to assign.
        ZYPHRYON_INLINE void SetAdditive(Real32 Additive)
        {
            mAdditive = Additive;
            SetDirty();
        }

        /// \brief Retrieves the additive modifier of this stat.
        ///
        /// \return The additive modifier value.
        ZYPHRYON_INLINE Real32 GetAdditive() const
        {
            return mAdditive;
        }

        /// \brief Sets the multiplier modifier for this stat.
        ///
        /// \param Multiplier The multiplier value to assign.
        ZYPHRYON_INLINE void SetMultiplier(Real32 Multiplier)
        {
            mMultiplier = Multiplier;
            SetDirty();
        }

        /// \brief Retrieves the multiplier modifier of this stat.
        ///
        /// \return The multiplier modifier value.
        ZYPHRYON_INLINE Real32 GetMultiplier() const
        {
            return mMultiplier;
        }

        /// \brief Directly sets and clamps the effective value to min/max using the provided context.
        ///
        /// \param Target    The context providing access to other stats if needed.
        /// \param Effective The effective value to assign.
        template<typename Context>
        ZYPHRYON_INLINE void SetEffective(ConstRef<Context> Target, Real32 Effective)
        {
            const Real32 Minimum = mArchetype->GetMinimum().Resolve(Target);
            const Real32 Maximum = mArchetype->GetMaximum().Resolve(Target);

            mEffective = Clamp(Effective, Minimum, Maximum);
            mDirty     = false;
        }

        /// \brief Retrieves the current effective value of this stat without recalculating.
        ///
        /// \return The effective stat value.
        ZYPHRYON_INLINE Real32 GetEffective() const
        {
            return mEffective;
        }

        /// \brief Calculates and retrieves the effective value of this stat based on its modifiers and context.
        ///
        /// \param Target The context providing access to other stats if needed.
        /// \return The calculated effective stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Target) const
        {
            if (mDirty)
            {
                mDirty     = false;
                mEffective = mArchetype->Calculate(Target, mFlat, mAdditive, mMultiplier);
            }
            return mEffective;
        }

        /// \brief Applies a modification to the stat based on the specified operator and amount.
        ///
        /// \param Target   The context providing access to other stats if needed.
        /// \param Operator The type of modification to apply.
        /// \param Amount   The amount to modify the stat by.
        template<typename Context>
        ZYPHRYON_INLINE void Apply(ConstRef<Context> Target, StatOperator Operator, Real32 Amount)
        {
            Modify<true>(Target, Operator, Amount);
        }

        /// \brief Reverts a previously applied modification to the stat based on the specified operator and amount.
        ///
        /// \param Target   The context providing access to other stats if needed.
        /// \param Operator The type of modification to revert.
        /// \param Amount   The amount to revert the stat by.
        template<typename Context>
        ZYPHRYON_INLINE void Revert(ConstRef<Context> Target, StatOperator Operator, Real32 Amount)
        {
            Modify<false>(Target, Operator, Amount);
        }

        /// \brief Checks if this stat instance corresponds to the given stat handle.
        ZYPHRYON_INLINE constexpr Bool operator==(StatHandle Handle) const
        {
            return mArchetype->GetHandle() == Handle;
        }

        /// \brief Checks if this stat instance corresponds to the given stat archetype.
        ZYPHRYON_INLINE constexpr Bool operator==(ConstRef<StatArchetype> Archetype) const
        {
            return mArchetype->GetHandle() == Archetype.GetHandle();
        }

        /// \brief Computes a hash value for the stat instance based on its archetype.
        ///
        /// \return A hash value uniquely representing the stat instance.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mArchetype->Hash();
        }

    private:

        /// \brief Modifies the stat based on the specified operator and amount, applying or reverting the change.
        ///
        /// \param Target   The context providing access to other stats if needed.
        /// \param Operator The type of modification to apply.
        /// \param Amount   The amount to modify the stat by.
        template<Bool Apply, typename Context>
        void Modify(ConstRef<Context> Target, StatOperator Operator, Real32 Amount)
        {
            switch (mArchetype->GetCategory())
            {
            case StatCategory::Attribute:
                switch (Operator)
                {
                case StatOperator::Flat:
                    mFlat += (Apply ? Amount : -Amount);
                    break;
                case StatOperator::Additive:
                    mAdditive += (Apply ? Amount : -Amount);
                    break;
                case StatOperator::Multiplicative:
                    mMultiplier *= (Apply ? Amount : 1.0f / Amount);
                    break;
                case StatOperator::Divisive:
                    mMultiplier *= (Apply ? 1.0f / Amount : Amount);
                    break;
                case StatOperator::Replace:
                    if constexpr (Apply)
                    {
                        SetEffective(Target, Amount);
                    }
                    break;
                }
                break;
            case StatCategory::Resource:
                if constexpr (Apply)
                {
                    switch (Operator)
                    {
                    case StatOperator::Flat:
                        SetEffective(Target, mEffective + Amount);
                        break;
                    case StatOperator::Additive:
                        SetEffective(Target, mEffective * (1.0f + Amount));
                        break;
                    case StatOperator::Multiplicative:
                        SetEffective(Target, mEffective * Amount);
                        break;
                    case StatOperator::Divisive:
                        SetEffective(Target, mEffective * (1.0f / Amount));
                        break;
                    case StatOperator::Replace:
                        SetEffective(Target, Amount);
                        break;
                    }
                }
                break;
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        ConstPtr<StatArchetype> mArchetype;
        Real32                  mFlat;
        Real32                  mAdditive;
        Real32                  mMultiplier;
        mutable Real32          mEffective;
        mutable Bool            mDirty;         // TODO: Steal a bit from Archetype (Reduce 25% size)
    };
}