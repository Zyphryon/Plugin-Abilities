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
#include "Gameplay/Stat/StatInput.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines a specification for an effect instance, including target, stack, and intensity.
    class EffectSpec final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE EffectSpec() = default;

        /// \brief Constructs an effect specification with specified parameters.
        ///
        /// \param Target    The effect handle representing the target effect.
        /// \param Stack     The stack input for the effect.
        /// \param Intensity The intensity input for the effect.
        ZYPHRYON_INLINE EffectSpec(EffectHandle Target, AnyRef<StatInput> Stack, AnyRef<StatInput> Intensity)
            : mTarget    { Target },
              mStack     { Move(Stack) },
              mIntensity { Move(Intensity) }
        {
        }

        /// \brief Constructs an effect specification by loading data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE EffectSpec(TOMLArray Array)
        {
            Load(Array);
        }

        /// \brief Sets the target effect handle for this specification.
        ///
        /// \param Handle The effect handle to set as the target.
        ZYPHRYON_INLINE void SetTarget(EffectHandle Handle)
        {
            mTarget = Handle;
        }

        /// \brief Retrieves the target effect handle for this specification.
        ///
        /// \return The effect handle that is the target of this specification.
        ZYPHRYON_INLINE EffectHandle GetTarget() const
        {
            return mTarget;
        }

        /// \brief Sets the stack input for this specification.
        ///
        /// \param Stack The stack input to assign.
        ZYPHRYON_INLINE void SetStack(AnyRef<StatInput> Stack)
        {
            mStack = Move(Stack);
        }

        /// \brief Retrieves the stack input for this specification.
        ///
        /// \return The stack input of this specification.
        ZYPHRYON_INLINE ConstRef<StatInput> GetStack() const
        {
            return mStack;
        }

        /// \brief Sets the intensity input for this specification.
        ///
        /// \param Intensity The intensity input to assign.
        ZYPHRYON_INLINE void SetIntensity(AnyRef<StatInput> Intensity)
        {
            mIntensity = Move(Intensity);
        }

        /// \brief Retrieves the intensity input for this specification.
        ///
        /// \return The intensity input of this specification.
        ZYPHRYON_INLINE ConstRef<StatInput> GetIntensity() const
        {
            return mIntensity;
        }

        /// \brief Iterates over all dependencies referenced by this specification.
        ///
        /// \param Action The action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            mStack.Traverse(Action);
            mIntensity.Traverse(Action);
        }

        /// \brief Loads the effect specification data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            mTarget    = Array.GetInteger(0);
            mStack.Load(Array.GetArray(1));
            mIntensity.Load(Array.GetArray(2));
        }

        /// \brief Saves the effect specification data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            Array.AddInteger(mTarget.GetID());
            mStack.Save(Array.AddArray());
            mIntensity.Save(Array.AddArray());
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        EffectHandle mTarget;
        StatInput    mStack;
        StatInput    mIntensity;
    };
}