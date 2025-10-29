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

#include "Gameplay/Stat/StatInput.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a Modifier that can be applied to a stat.
    class EffectModifier final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE EffectModifier() = default;

        /// \brief Constructs an effect modifier with specified parameters.
        ///
        /// \param Target    The target stat handle.
        /// \param Mode      The evaluation method for the modifier.
        /// \param Operation The operation to apply for the modifier.
        /// \param Magnitude The magnitude value for the modifier.
        ZYPHRYON_INLINE EffectModifier(Stat Target, StatMode Mode, StatOp Operation, AnyRef<StatInput> Magnitude)
            : mTarget    { Target },
              mMode      { Mode },
              mOperation { Operation },
              mMagnitude { Move(Magnitude) }

        {
        }

        /// \brief Sets the target stat handle for this modifier.
        ///
        /// \param Handle The stat handle to set as the target.
        ZYPHRYON_INLINE void SetTarget(Stat Handle)
        {
            mTarget = Handle;
        }

        /// \brief Retrieves the target stat handle for this modifier.
        ///
        /// \return The stat handle that is the target of this modifier.
        ZYPHRYON_INLINE Stat GetTarget() const
        {
            return mTarget;
        }

        /// \brief Sets the evaluation method for this modifier.
        ///
        /// \param Mode The evaluation method to assign.
        ZYPHRYON_INLINE void SetMode(StatMode Mode)
        {
            mMode = Mode;
        }

        /// \brief Retrieves the evaluation method for this modifier.
        ///
        /// \return The evaluation method of this modifier.
        ZYPHRYON_INLINE StatMode GetMode() const
        {
            return mMode;
        }

        /// \brief Sets the operation to apply for this modifier.
        ///
        /// \param Operation The operation to assign.
        ZYPHRYON_INLINE void SetOperation(StatOp Operation)
        {
            mOperation = Operation;
        }

        /// \brief Retrieves the operation to apply for this modifier.
        ///
        /// \return The operation of this modifier.
        ZYPHRYON_INLINE StatOp GetOperation() const
        {
            return mOperation;
        }

        /// \brief Sets the magnitude of the modifier.
        ///
        /// \param Magnitude The magnitude value to assign.
        ZYPHRYON_INLINE void SetMagnitude(AnyRef<StatInput> Magnitude)
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
        /// \param Scope  The scope context of dependencies to consider.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatScope Scope) const
        {
            mMagnitude.Traverse(Action, Scope);
        }

        /// \brief Loads the stat modifier data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            mTarget    = Array.GetInteger(0);
            mMode      = Array.GetEnum(1, StatMode::Snapshot);
            mOperation = Array.GetEnum(2, StatOp::Add);
            mMagnitude.Load(Array.GetArray(3));
        }

        /// \brief Saves the stat modifier data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            Array.AddInteger(mTarget.GetID());
            Array.AddEnum(mMode);
            Array.AddEnum(mOperation);
            mMagnitude.Save(Array);
        }

    public:

        /// \brief Creates a dynamic effect modifier.
        ///
        /// \param Target    The target stat handle.
        /// \param Operation The operation to apply for the modifier.
        /// \param Magnitude The magnitude value for the modifier.
        /// \return A new effect modifier instance configured as dynamic.
        static EffectModifier CreateDynamic(Stat Target, StatOp Operation, AnyRef<StatInput> Magnitude)
        {
            return EffectModifier(Target, StatMode::Dynamic, Operation, Move(Magnitude));
        }

        /// \brief Creates a snapshot effect modifier.
        ///
        /// \param Target    The target stat handle.
        /// \param Operation The operation to apply for the modifier.
        /// \param Magnitude The magnitude value for the modifier.
        /// \return A new effect modifier instance configured as snapshot.
        static EffectModifier CreateSnapshot(Stat Target, StatOp Operation, AnyRef<StatInput> Magnitude)
        {
            return EffectModifier(Target, StatMode::Snapshot, Operation, Move(Magnitude));
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Stat      mTarget;
        StatMode  mMode;
        StatOp    mOperation;
        StatInput mMagnitude;
    };
}