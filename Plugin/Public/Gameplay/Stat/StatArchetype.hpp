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

#include "Gameplay/Stat/StatInput.hpp"

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
              mKind     { StatKind::Attribute },
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
        ZYPHRYON_INLINE void SetHandle(Stat Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique handle of this stat archetype.
        ///
        /// \return The stat handle.
        ZYPHRYON_INLINE Stat GetHandle() const
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

        /// \brief Sets the kind of the stat archetype.
        ///
        /// \param Kind The kind to assign.
        ZYPHRYON_INLINE void SetKind(StatKind Kind)
        {
            mKind = Kind;
        }

        /// \brief Retrieves the kind of the stat archetype.
        ///
        /// \return The stat kind.
        ZYPHRYON_INLINE StatKind GetKind() const
        {
            return mKind;
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
        ZYPHRYON_INLINE void SetFormula(Ptr<StatFormula> Formula)
        {
            mFormula = Formula;
        }

        /// \brief Retrieves the formula used to calculate this stat's final value.
        ///
        /// \return The stat formula.
        ZYPHRYON_INLINE Ptr<StatFormula> GetFormula() const
        {
            return mFormula;
        }

        /// \brief Calculates the effective stat value using the provided source context.
        ///
        /// \param Source     The source context to retrieve stat values from.
        /// \param Flat       The flat modifier to apply to the base value.
        /// \param Additive   The additive modifier to apply to the base value.
        /// \param Multiplier The multiplier to apply to the base value.
        /// \return The calculated and clamped stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Source, Real32 Flat, Real32 Additive, Real32 Multiplier) const
        {
            Real32 Result;

            if (!mFormula)
            {
                Result = StatFormula::Default(mBase.Resolve(Source), Flat, Additive, Multiplier);
            }
            else
            {
                Result = mFormula->Calculate(Source, mBase.Resolve(Source), Flat, Additive, Multiplier);
            }
            return Clamp(Result, mMinimum.Resolve(Source), mMaximum.Resolve(Source));
        }

        /// \brief Iterates over all dependencies referenced by this archetype.
        ///
        /// \param Action The action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            // Iterate over the base stat input.
            mBase.Traverse(Action);

            // Iterate over the min and max stat inputs.
            mMinimum.Traverse(Action);
            mMaximum.Traverse(Action);

            // Iterate over the formula dependencies, if a formula is defined.
            if (mFormula)
            {
                mFormula->Traverse(Action);
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
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mHandle.GetID();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Stat             mHandle;
        StatKind         mKind;
        Str8             mName;
        StatInput        mBase;
        StatInput        mMinimum;
        StatInput        mMaximum;
        Ptr<StatFormula> mFormula;
    };
}