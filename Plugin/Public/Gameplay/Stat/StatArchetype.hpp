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

#include "StatFormula.hpp"
#include "StatInput.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of a stat, including its base, min, max values and calculation formula.
    class StatArchetype final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE StatArchetype()
            : mHandle   { 0 },
              mCategory { StatCategory::Attribute },
              mBase     { 0.0f },
              mMinimum  { 0.0f },
              mMaximum  { 0.0f },
              mFormula  { nullptr }
        {
        }

        /// \brief Constructs a stat archetype by loading data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE StatArchetype(TOMLSection Section)
        {
            Load(Section);
        }

        /// \brief Destruct a stat archetype.
        ZYPHRYON_INLINE ~StatArchetype()
        {
            mHandle.Reset();
        }

        /// \brief Checks if the stat archetype has a valid handle.
        ///
        /// \return `true` if the stat archetype is valid, `false` otherwise.
        ZYPHRYON_INLINE Bool IsValid() const
        {
            return mHandle.IsValid();
        }

        /// \brief Sets the unique handle for this stat archetype.
        ///
        /// \param Handle The unique stat handle to assign.
        ZYPHRYON_INLINE void SetHandle(StatHandle Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique handle of this stat archetype.
        ///
        /// \return The stat handle.
        ZYPHRYON_INLINE StatHandle GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the name of the stat archetype.
        ///
        /// \param Name The name to assign.
        ZYPHRYON_INLINE void SetName(ConstStr8 Name)
        {
            mName = Name;
        }

        /// \brief Retrieves the name of the stat archetype.
        ///
        /// \return The stat name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            return mName;
        }

        /// \brief Sets the category of the stat archetype.
        ///
        /// \param Category The category to assign.
        ZYPHRYON_INLINE void SetCategory(StatCategory Category)
        {
            mCategory = Category;
        }

        /// \brief Retrieves the category of the stat archetype.
        ///
        /// \return The stat category.
        ZYPHRYON_INLINE StatCategory GetCategory() const
        {
            return mCategory;
        }

        /// \brief Sets the base value for this stat archetype.
        ///
        /// \param Base The base value to assign.
        ZYPHRYON_INLINE void SetBase(AnyRef<StatInput> Base)
        {
            mBase = Move(Base);
        }

        /// \brief Retrieves the base value of this stat archetype.
        ///
        /// \return The base value.
        ZYPHRYON_INLINE ConstRef<StatInput> GetBase() const
        {
            return mBase;
        }

        /// \brief Sets the minimum value for this stat archetype.
        ///
        /// \param Minimum The minimum value to assign.
        ZYPHRYON_INLINE void SetMinimum(AnyRef<StatInput> Minimum)
        {
            mMinimum = Move(Minimum);
        }

        /// \brief Gets the minimum value of this stat archetype.
        ///
        /// \return The minimum value.
        ZYPHRYON_INLINE ConstRef<StatInput> GetMinimum() const
        {
            return mMinimum;
        }

        /// \brief Sets the maximum value for this stat archetype.
        ///
        /// \param Maximum The maximum value to assign.
        ZYPHRYON_INLINE void SetMaximum(AnyRef<StatInput> Maximum)
        {
            mMaximum = Move(Maximum);
        }

        /// \brief Gets the maximum value of this stat archetype.
        ///
        /// \return The maximum value.
        ZYPHRYON_INLINE ConstRef<StatInput> GetMaximum() const
        {
            return mMaximum;
        }

        /// \brief Sets the formula used to calculate this stat's final value.
        ///
        /// \param Formula The formula to assign.
        ZYPHRYON_INLINE void SetFormula(ConstTracker<StatFormula> Formula)
        {
            mFormula = Formula;
        }

        /// \brief Retrieves the formula used to calculate this stat's final value.
        ///
        /// \return The stat formula.
        ZYPHRYON_INLINE ConstTracker<StatFormula> GetFormula() const
        {
            return mFormula;
        }

        /// \brief Calculates the final value of the stat based on the provided parameters and context.
        ///
        /// \param Target     The context providing access to other stats.
        /// \param Flat       The flat modifier to be added to the base value.
        /// \param Additive   The additive percentage modifier (e.g., 0.2 for +20%).
        /// \param Multiplier The multiplicative modifier (e.g., 1.5 for x1.5).
        /// \return The calculated and clamped final stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Target, Real32 Flat, Real32 Additive, Real32 Multiplier) const
        {
            Real32 Result;

            if (mFormula)
            {
                Result = mFormula->Calculate(Target, mBase.Resolve(Target), Flat, Additive, Multiplier);
            }
            else
            {
                Result = StatFormula::Default(mBase.Resolve(Target), Flat, Additive, Multiplier);
            }
            return Clamp(Result, mMinimum.Resolve(Target), mMaximum.Resolve(Target));
        }

        /// \brief Iterates over all dependencies referenced by this archetype.
        ///
        /// \note Duplicate dependencies may be visited multiple times.
        ///
        /// \tparam Function The type of the action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            mBase.Traverse(Action, StatOrigin::Target);
            mMinimum.Traverse(Action, StatOrigin::Target);
            mMaximum.Traverse(Action, StatOrigin::Target);

            if (mFormula)
            {
                for (const StatHandle Dependency : mFormula->GetDependencies())
                {
                    Action(Dependency);
                }
            }
        }

        /// \brief Loads the stat archetype data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        void Load(TOMLSection Section);

        /// \brief Saves the stat archetype data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        void Save(TOMLSection Section) const;

        /// \brief Generates a hash value for the stat archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the stat archetype.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mHandle.GetID();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        StatHandle           mHandle;
        Str8                 mName;
        StatCategory         mCategory;
        StatInput            mBase;
        StatInput            mMinimum;
        StatInput            mMaximum;
        Tracker<StatFormula> mFormula;
    };
}