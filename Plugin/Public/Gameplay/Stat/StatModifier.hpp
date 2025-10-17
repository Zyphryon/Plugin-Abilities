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

#include "StatInput.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a modifier that can be applied to a stat.
    class StatModifier final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE StatModifier()
            : mTarget     { 0 },
              mEvaluation { StatEvaluation::Snapshot },
              mOperator   { StatOperator::Flat },
              mMagnitude  { 0.0f }
        {
        }

        /// \brief Constructs a stat modifier with specified parameters.
        ///
        /// \param Target     The target stat handle.
        /// \param Evaluation The evaluation method for the modifier.
        /// \param Operator   The operator type for the modifier.
        /// \param Magnitude  The magnitude of the modifier.
        ZYPHRYON_INLINE StatModifier(StatHandle Target, StatEvaluation Evaluation, StatOperator Operator, StatInput Magnitude)
            : mTarget     { Target },
              mEvaluation { Evaluation },
              mOperator   { Operator },
              mMagnitude  { Move(Magnitude) }
        {
        }

        /// \brief Sets the target stat handle for this modifier.
        ///
        /// \param Handle The stat handle to set as the target.
        ZYPHRYON_INLINE void SetTarget(StatHandle Handle)
        {
            mTarget = Handle;
        }

        /// \brief Retrieves the target stat handle for this modifier.
        ///
        /// \return The stat handle that is the target of this modifier.
        ZYPHRYON_INLINE StatHandle GetTarget() const
        {
            return mTarget;
        }

        /// \brief Sets the evaluation method for this modifier.
        ///
        /// \param Evaluation The evaluation method to assign.
        ZYPHRYON_INLINE void SetEvaluation(StatEvaluation Evaluation)
        {
            mEvaluation = Evaluation;
        }

        /// \brief Retrieves the evaluation method for this modifier.
        ///
        /// \return The evaluation method of this modifier.
        ZYPHRYON_INLINE StatEvaluation GetEvaluation() const
        {
            return mEvaluation;
        }

        /// \brief Sets the operator type for this modifier.
        ///
        /// \param Operator The operator type to assign.
        ZYPHRYON_INLINE void SetOperator(StatOperator Operator)
        {
            mOperator = Operator;
        }

        /// \brief Retrieves the operator type for this modifier.
        ///
        /// \return The operator type of this modifier.
        ZYPHRYON_INLINE StatOperator GetOperator() const
        {
            return mOperator;
        }

        /// \brief Sets the magnitude of the modifier.
        ///
        /// \param Magnitude The magnitude value to assign.
        ZYPHRYON_INLINE void SetMagnitude(StatInput Magnitude)
        {
            mMagnitude = Move(Magnitude);
        }

        /// \brief Retrieves the magnitude of the modifier.
        ///
        /// \return The magnitude value of the modifier.
        ZYPHRYON_INLINE ConstRef<StatInput> GetMagnitude() const
        {
            return mMagnitude;
        }

        /// \brief Iterates over all dependencies referenced by the modifier.
        ///
        /// \param Action The action to apply to each dependency.
        /// \param Origin The origin context of dependencies to consider.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatOrigin Origin) const
        {
            mMagnitude.Traverse(Action, Origin);
        }

        /// \brief Loads the stat modifier data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            mTarget     = Array.GetInteger(0);
            mEvaluation = Enum::Cast(Array.GetString(1), StatEvaluation::Snapshot);
            mOperator   = Enum::Cast(Array.GetString(2), StatOperator::Flat);
            mMagnitude.Load(Array.GetArray(3));
        }

        /// \brief Saves the stat modifier data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            Array.AddInteger(mTarget.GetID());
            Array.AddString(Enum::GetName(mEvaluation));
            Array.AddString(Enum::GetName(mOperator));
            mMagnitude.Save(Array);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        StatHandle     mTarget;
        StatEvaluation mEvaluation;
        StatOperator   mOperator;
        StatInput      mMagnitude;
    };
}