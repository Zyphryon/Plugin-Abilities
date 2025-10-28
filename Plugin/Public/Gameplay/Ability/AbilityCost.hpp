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
    /// \brief Defines the cost structure for an ability.
    class AbilityCost final
    {
    public:

        /// \brief Maximum number of inputs an ability cost can have.
        static constexpr UInt32 kMaxInput = 3; // TODO: Macro Configurable

        /// \brief Structure representing a cost input for an ability.
        struct Input
        {
            /// \brief The stat handle representing the target of the ability cost.
            Stat      Target;

            /// \brief The input value or formula used to calculate the cost.
            StatInput Cost;

            /// \brief Default constructor.
            ZYPHRYON_INLINE Input() = default;

            /// \brief Constructs an input with the specified target handle and cost input.
            ///
            /// \param Target The stat handle representing the target of the ability cost.
            /// \param Cost   The input value or formula used to calculate the cost.
            ZYPHRYON_INLINE Input(Stat Target, AnyRef<StatInput> Cost)
                : Target { Target },
                  Cost   { Move(Cost) }
            {
            }

            /// \brief Constructs an input by loading data from a TOML section.
            ///
            /// \param Section The TOML section to load from.
            ZYPHRYON_INLINE Input(TOMLSection Section)
            {
                Load(Section);
            }

            /// \brief Checks if the source context can afford the cost defined by this input.
            ///
            /// \param Source The context used to evaluate the cost.
            /// \return `true` if the cost can be afforded, `false` otherwise.
            template<typename Context>
            ZYPHRYON_INLINE Bool CanAfford(ConstRef<Context> Source) const
            {
                return Source.GetStat(Target) >= Cost.Resolve(Source);
            }

            /// \brief Traverses the cost input, applying the provided action to each component.
            ///
            /// \param Action The action to apply during traversal.
            template<typename Function>
            ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
            {
                Cost.Traverse(Action);
            }

            /// \brief Loads the input data from a TOML section.
            ///
            /// \param Section The TOML section to load from.
            ZYPHRYON_INLINE void Load(TOMLSection Section)
            {
                Target = Section.GetInteger("Target");
                Cost.Load(Section.GetArray("Cost"));
            }

            /// \brief Saves the input data to a TOML section.
            ///
            /// \param Section The TOML section to save to.
            ZYPHRYON_INLINE void Save(TOMLSection Section) const
            {
                Section.SetInteger("Target", Target.GetID());
                Cost.Save(Section.SetArray("Cost"));
            }
        };

    public:

        /// \brief Sets the list of inputs for the ability cost.
        ///
        /// \param Inputs A span of input definitions to assign.
        ZYPHRYON_INLINE void SetInputs(ConstSpan<Input> Inputs)
        {
            mInputs.assign(Inputs.begin(), Inputs.end());
        }

        /// \brief Retrieves the list of inputs for the ability cost.
        ///
        /// \return A span of input definitions.
        ZYPHRYON_INLINE ConstSpan<Input> GetInputs() const
        {
            return mInputs;
        }

        /// \brief Checks if the source context can afford the total cost defined by all inputs.
        ///
        /// \param Source The context used to evaluate the total cost.
        /// \return `true` if the total cost can be afforded, `false` otherwise
        template<typename Context>
        ZYPHRYON_INLINE Bool CanAfford(ConstRef<Context> Source) const
        {
            for (ConstRef<Input> Input : mInputs)
            {
                if (!Input.CanAfford(Source))
                {
                    return false;
                }
            }
            return true;
        }

        /// \brief Traverses all inputs, applying the provided action to each component.
        ///
        /// \param Action The action to apply during traversal.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<Input> Input : mInputs)
            {
                Input.Traverse(Action);
            }
        }

        /// \brief Loads the ability cost data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            for (UInt32 Element = 0; Element < Array.GetSize(); ++Element)
            {
                mInputs.emplace_back(Array.GetSection(Element));
            }
        }

        /// \brief Saves the ability cost data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            for (ConstRef<Input> Input : mInputs)
            {
                Input.Save(Array.AddSection());
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Vector<Input, kMaxInput> mInputs;
    };
}