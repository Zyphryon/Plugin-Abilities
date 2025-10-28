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

#include "Gameplay/Stat/StatArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents an instance of a stat with its modifiers and effective value.
    class StatData final
    {
    public:

        /// \brief Constructs a stat instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the stat's properties.
        ZYPHRYON_INLINE StatData(ConstRef<StatArchetype> Archetype)
            : mArchetype  { & Archetype },
              mFlat       { 0.0f },
              mAdditive   { 0.0f },
              mMultiplier { 1.0f },
              mEffective  { 0.0f }
        {
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
        }

        /// \brief Retrieves the current effective value of this stat without recalculating.
        ///
        /// \return The effective stat value.
        ZYPHRYON_INLINE Real32 GetEffective() const
        {
            return mEffective;
        }

        /// \brief Recalculates and updates the effective value based on the current modifiers and archetype formula.
        ///
        /// \param Target The context providing access to other stats if needed.
        /// \return The recalculated effective stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Resolve(ConstRef<Context> Target)
        {
            if (mArchetype->GetKind() == StatKind::Attribute)
            {
                mEffective = mArchetype->Calculate(Target, mFlat, mAdditive, mMultiplier);
            }
            return mEffective;
        }

        /// \brief Applies a modification to the stat based on the specified operator and amount.
        ///
        /// \param Target   The context providing access to other stats if needed.
        /// \param Modifier The type of modification to apply.
        /// \param Amount   The amount to modify the stat by.
        template<typename Context>
        ZYPHRYON_INLINE void Apply(ConstRef<Context> Target, StatModifier Modifier, Real32 Amount)
        {
            Modify<true>(Target, Modifier, Amount);
        }

        /// \brief Reverts a previously applied modification to the stat based on the specified operator and amount.
        ///
        /// \param Target   The context providing access to other stats if needed.
        /// \param Modifier The type of modification to revert.
        /// \param Amount   The amount to revert the stat by.
        template<typename Context>
        ZYPHRYON_INLINE void Revert(ConstRef<Context> Target, StatModifier Modifier, Real32 Amount)
        {
            Modify<false>(Target, Modifier, Amount);
        }

        /// \brief Checks if this stat instance corresponds to the given stat handle.
        ///
        /// \param Handle The stat handle to compare with.
        /// \return `true` if the stat instance matches the handle, `false` otherwise.
        ZYPHRYON_INLINE Bool operator==(Stat Handle) const
        {
            return mArchetype->GetHandle() == Handle;
        }

        /// \brief Checks if this stat instance corresponds to the given stat archetype.
        ///
        /// \param Archetype The stat archetype to compare with.
        /// \return `true` if the stat instance matches the archetype, `false` otherwise.
        ZYPHRYON_INLINE Bool operator==(ConstRef<StatArchetype> Archetype) const
        {
            return mArchetype->GetHandle() == Archetype.GetHandle();
        }

        /// \brief Computes a hash value for the stat instance based on its archetype.
        ///
        /// \return A hash value uniquely representing the stat instance.
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mArchetype->Hash();
        }

    private:

        /// \brief Modifies the stat based on the specified operator and amount, applying or reverting the change.
        ///
        /// \param Target    The context providing access to other stats if needed.
        /// \param Modifier  The type of modification to apply.
        /// \param Magnitude The amount to modify the stat by.
        template<Bool Apply, typename Context>
        void Modify(ConstRef<Context> Target, StatModifier Modifier, Real32 Magnitude)
        {
            switch (mArchetype->GetKind())
            {
            case StatKind::Attribute:
                switch (Modifier)
                {
                case StatModifier::Add:
                    mFlat += Apply ? Magnitude : -Magnitude;
                    break;
                case StatModifier::Percent:
                    mAdditive += Apply ? Magnitude : -Magnitude;
                    break;
                case StatModifier::Scale:
                    mMultiplier *= Apply ? Magnitude : 1.0f / Magnitude;
                    break;
                case StatModifier::Set:
                    if constexpr (Apply)
                    {
                        SetEffective(Target, Magnitude);
                    }
                    break;
                }
                break;
            case StatKind::Resource:
                if constexpr (Apply)
                {
                    switch (Modifier)
                    {
                    case StatModifier::Add:
                        SetEffective(Target, mEffective + Magnitude);
                        break;
                    case StatModifier::Percent:
                        SetEffective(Target, mEffective * (1.0f + Magnitude));
                        break;
                    case StatModifier::Scale:
                        SetEffective(Target, mEffective * Magnitude);
                        break;
                    case StatModifier::Set:
                        SetEffective(Target, Magnitude);
                        break;
                    }
                }
                break;
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        ConstPtr<StatArchetype> mArchetype;
        Real32                  mFlat;
        Real32                  mAdditive;
        Real32                  mMultiplier;
        Real32                  mEffective;
    };
}