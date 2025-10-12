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

#include "Stat.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a modifier that can be applied to a stat, altering its value based on defined rules.
    class StatModifier final
    {
    public:

        /// \brief Enumeration defining the type of action the modifier performs on the stat.
        enum class Action : UInt8
        {
            Flat,           ///< Directly adds or subtracts a flat value to/from the stat.
            Additive,       ///< Adds or subtracts a percentage to/from the stat (e.g., +0.2 for +20%).
            Multiplicative, ///< Multiplies the stat by a factor (e.g., x1.5 for 50% increase).
            Divide,         ///< Divides the stat by a factor (e.g., /1.5 for 33% decrease).
            Override,       ///< Overrides the stat value entirely.
        };

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE StatModifier()
            : mTarget    { 0 },
              mAction    { Action::Flat },
              mMagnitude { 0.0f }
        {
        }

        /// \brief Constructs a stat modifier with specified target, action, and magnitude.
        ///
        /// \param Target    The stat handle that this modifier targets.
        /// \param Effect    The action type of the modifier (Flat, Additive, Multiplicative).
        /// \param Magnitude The magnitude of the modifier's effect.
        ZYPHRYON_INLINE StatModifier(StatHandle Target, Action Effect, AnyRef<StatProxy> Magnitude)
            : mTarget    { Target },
              mAction    { Effect },
              mMagnitude { Move(Magnitude) }
        {
            LOG_ASSERT(Target.IsValid(), "Target stat handle must be valid.");
        }

        /// \brief Retrieves the target stat handle that this modifier affects.
        ///
        /// \return The target stat handle.
        ZYPHRYON_INLINE StatHandle GetTarget() const
        {
            return mTarget;
        }

        /// \brief Retrieves the action type of this modifier.
        ///
        /// \return The action type (Flat, Additive, Multiplicative).
        ZYPHRYON_INLINE Action GetAction() const
        {
            return mAction;
        }

        /// \brief Retrieves the magnitude of this modifier's effect.
        ///
        /// \return The magnitude value.
        ZYPHRYON_INLINE ConstRef<StatProxy> GetMagnitude() const
        {
            return mMagnitude;
        }

        /// \brief Applies the modifier to the specified stat based on the target context and intensity.
        ///
        /// \param Target    The context providing access to stats.
        /// \param Stat      The stat to modify.
        /// \param Intensity A multiplier to scale the effect of the modifier.
        template<typename Context>
        ZYPHRYON_INLINE void Apply(ConstRef<Context> Target, Ref<Stat> Stat, Real32 Intensity) const
        {
            Modify<true>(Target, Stat, Intensity);
        }

        /// \brief Reverts the modifier's effect on the specified stat based on the target context and intensity.
        ///
        /// \param Target    The context providing access to stats.
        /// \param Stat      The stat to revert.
        /// \param Intensity A multiplier to scale the effect of the modifier.
        template<typename Context>
        ZYPHRYON_INLINE void Revert(ConstRef<Context> Target, Ref<Stat> Stat, Real32 Intensity) const
        {
            Modify<false>(Target, Stat, Intensity);
        }

        /// \brief Loads the modifier data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        void Load(TOMLArray Array);

        /// \brief Saves the modifier data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        void Save(TOMLArray Array) const;

    private:

        /// \brief Modifies the stat based on the action type and whether to apply or revert the effect.
        ///
        /// \param Target    The context providing access to stats.
        /// \param Stat      The stat to modify.
        /// \param Intensity A multiplier to scale the effect of the modifier.
        template<Bool Apply, typename Context>
        ZYPHRYON_INLINE void Modify(ConstRef<Context> Target, Ref<Stat> Stat, Real32 Intensity) const
        {
            const Real32 Amount = mMagnitude.Resolve(Target) * Intensity;

            switch (Stat.GetArchetype()->GetCategory())
            {
            case StatCategory::Resource:
                switch (mAction)
                {
                case Action::Flat:
                    Stat.SetEffective(Target, Stat.GetEffective() + (Apply ? Amount : -Amount));
                    break;
                case Action::Additive:
                    Stat.SetEffective(Target, Stat.GetEffective() * (Apply ? 1.0f + Amount : 1.0f / (1.0f + Amount)));
                    break;
                case Action::Multiplicative:
                    Stat.SetEffective(Target, Stat.GetEffective() * (Apply ? Amount : 1.0f / Amount));
                    break;
                case Action::Divide:
                    Stat.SetEffective(Target, Stat.GetEffective() * (Apply ? 1.0f / Amount : Amount));
                    break;
                case Action::Override:
                    if constexpr (Apply)
                    {
                        Stat.SetEffective(Target, Amount);
                    }
                    break;
                }
                break;
            case StatCategory::Formula:
                switch (mAction)
                {
                case Action::Flat:
                    Stat.SetFlat(Stat.GetFlat() + (Apply ? Amount : -Amount));
                    break;
                case Action::Additive:
                    Stat.SetAdditive(Stat.GetAdditive() + (Apply ? Amount : -Amount));
                    break;
                case Action::Multiplicative:
                    Stat.SetMultiplier(Stat.GetMultiplier() * (Apply ? Amount : 1.0f / Amount));
                    break;
                case Action::Divide:
                    Stat.SetMultiplier(Stat.GetMultiplier() * (Apply ? 1.0f / Amount : Amount));
                    break;
                case Action::Override:
                    if constexpr (Apply)
                    {
                        Stat.SetEffective(Target, Amount);
                    }
                    break;
                }
                break;
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        StatHandle mTarget;
        Action     mAction;
        StatProxy  mMagnitude;
    };
}