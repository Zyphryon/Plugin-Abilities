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

#include "StatHandle.hpp"
#include "StatPolicies.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a formula used to calculate the effective value of a stat based on dependencies.
    class StatFormula final
    {
        // TODO: Allow to query tokens

    public:

        /// \brief Maximum number of dependencies a formula can have for source and target each.
        static constexpr UInt32 kMaxDependencies = 10;   // TODO: Macro Configurable

        /// \brief Maximum number of tokens in a computation snapshot.
        static constexpr UInt32 kMaxTokens       = 4;    // TODO: Macro Configurable

        /// \brief Structure representing a dependency on another stat.
        ///
        /// \note We steal 1 bit from the StatHandle to store the origin context.
        struct Dependency final
        {
            /// \brief Constructs a dependency with the specified stat handle and origin.
            ///
            /// \param Handle The handle of the dependent stat.
            /// \param Origin The origin context of the dependent stat.
            ZYPHRYON_INLINE Dependency(StatHandle Handle, StatOrigin Origin)
                : mPackage { static_cast<UInt16>(Handle.GetID() | static_cast<UInt16>(Origin) << 15) }
            {
            }

            /// \brief Retrieves the stat handle of the dependency.
            ///
            /// \return The stat handle.
            ZYPHRYON_INLINE StatHandle GetHandle() const
            {
                return StatHandle(GetBit(mPackage, 0, 0x7FFF));
            }

            /// \brief Retrieves the origin context of the dependency.
            ///
            /// \return The stat origin.
            ZYPHRYON_INLINE StatOrigin GetOrigin() const
            {
                return static_cast<StatOrigin>(GetBit(mPackage, 15, 0x8000));
            }

        private:

            // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
            // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

            UInt16 mPackage;
        };

        /// \brief Structure representing the components of a stat calculation.
        struct Computation final
        {
            /// \brief Default constructor, all members are undefined.
            ZYPHRYON_INLINE Computation() = default;

            /// \brief Constructs a `Computation` instance with the given parameters.
            ///
            /// \param Base       The base value of the stat.
            /// \param Flat       The flat addition to apply to the base value.
            /// \param Additive   The percentage addition to apply to the base value.
            /// \param Multiplier The multiplier to apply to the base value.
            ZYPHRYON_INLINE Computation(Real32 Base, Real32 Flat, Real32 Additive, Real32 Multiplier)
                : Base       { Base },
                  Flat       { Flat },
                  Additive   { Additive },
                  Multiplier { Multiplier },
                  Snapshot   { }
            {
            }

            /// \brief Populates the snapshots from a single context based on the dependencies.
            ///
            /// \param Source       The source context to retrieve stat values from.
            /// \param Dependencies The list of dependencies to populate snapshots for.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Source, ConstSpan<Dependency> Dependencies)
            {
                for (UInt32 Index = 0; Index < Dependencies.size(); ++Index)
                {
                    Snapshot[Index] = Source.GetStat(Dependencies[Index].GetHandle());
                }
            }

            /// \brief Populates the snapshots from both source and target contexts based on the dependencies.
            ///
            /// \param Source       The source context to retrieve stat values from.
            /// \param Target       The target context to retrieve stat values from.
            /// \param Dependencies The list of dependencies to populate snapshots for.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Source, ConstRef<Context> Target, ConstSpan<Dependency> Dependencies)
            {
                for (UInt32 Index = 0; Index < Dependencies.size(); ++Index)
                {
                    ConstRef<Dependency> Dependency = Dependencies[Index];

                    if (Dependency.GetOrigin() == StatOrigin::Source)
                    {
                        Snapshot[Index] = Source.GetStat(Dependency.GetHandle());
                    }
                    else
                    {
                        Snapshot[Index] = Target.GetStat(Dependency.GetHandle());
                    }
                }
            }

            /// \brief The base value of the stat.
            Real32                          Base;

            /// \brief The flat addition to apply to the base value.
            Real32                          Flat;

            /// \brief The percentage addition to apply to the base value.
            Real32                          Additive;

            /// \brief The multiplier to apply to the base value.
            Real32                          Multiplier;

            /// \brief Snapshot of source stat values at the time of computation.
            Array<Real32, kMaxDependencies> Snapshot;
        };

        /// \brief Type alias for a function that computes a stat value from a `Computation`.
        using Calculator = Delegate<Real32(ConstRef<Computation>), DelegateInlineSize::Smallest>;

    public:

        /// \brief Default constructor, initializes with an empty calculator.
        ZYPHRYON_INLINE StatFormula() = default;

        /// \brief Constructs a formula with the specified calculation function.
        ///
        /// \param Calculator The function used to compute the stat value.
        ZYPHRYON_INLINE StatFormula(AnyRef<Calculator> Calculator)
            : mCalculator { Move(Calculator) }
        {
        }

        /// \brief Sets the calculation function for this formula.
        ///
        /// \param Calculator The function used to compute the stat value.
        ZYPHRYON_INLINE void SetCalculator(AnyRef<Calculator> Calculator)
        {
            mCalculator = Move(Calculator);
        }

        /// \brief Adds a dependency stat handle to the source dependencies of this formula.
        ///
        /// \param Dependency The stat handle that this formula depends on from the source.
        ZYPHRYON_INLINE void AddSourceDependency(StatHandle Dependency)
        {
            LOG_ASSERT(mDependencies.size() < kMaxDependencies, "Exceeded maximum number of dependencies.");

            mDependencies.emplace_back(Dependency, StatOrigin::Source);
        }

        /// \brief Adds a dependency stat handle to the target dependencies of this formula.
        ///
        /// \param Dependency The stat handle that this formula depends on from the target.
        ZYPHRYON_INLINE void AddTargetDependency(StatHandle Dependency)
        {
            LOG_ASSERT(mDependencies.size() < kMaxDependencies, "Exceeded maximum number of dependencies.");

            mDependencies.emplace_back(Dependency, StatOrigin::Target);
        }

        /// \brief Calculates the effective stat value using the provided source context.
        ///
        /// \param Source     The source context to retrieve stat values from.
        /// \param Base       The base value of the stat.
        /// \param Flat       The flat addition to apply to the base value.
        /// \param Additive   The percentage addition to apply to the base value.
        /// \param Multiplier The multiplier to apply to the base value.
        /// \return The calculated stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Source, Real32 Base, Real32 Flat, Real32 Additive, Real32 Multiplier) const
        {
            Computation Computation(Base, Flat, Additive, Multiplier);
            Computation.Populate(Source, mDependencies);

            return mCalculator(Computation);
        }

        /// \brief Calculates the effective stat value using the provided source context.
        ///
        /// \param Source The source context to retrieve stat values from.
        /// \return The calculated stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Source) const
        {
            Computation Computation;
            Computation.Populate(Source, mDependencies);

            return mCalculator(Computation);
        }

        /// \brief Calculates the effective stat value using the provided source and target contexts.
        ///
        /// \param Source The source context to retrieve stat values from.
        /// \param Target The target context to retrieve stat values from.
        /// \return The calculated stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Source, ConstRef<Context> Target) const
        {
            Computation Computation;
            Computation.Populate(Source, Target, mDependencies);

            return mCalculator(Computation);
        }

        /// \brief Iterates over all dependencies referenced by this formula.
        ///
        /// \param Action The action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<Dependency> Dependency : mDependencies)
            {
                Action(Dependency.GetHandle());
            }
        }

        /// \brief Iterates over all dependencies referenced by this formula.
        ///
        /// \param Action The action to apply to each dependency.
        /// \param Origin The origin context of dependencies to consider.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatOrigin Origin) const
        {
            for (ConstRef<Dependency> Dependency : mDependencies)
            {
                if (Dependency.GetOrigin() == Origin)
                {
                    Action(Dependency.GetHandle());
                }
            }
        }

    public:

        /// \brief Default formula implementation that applies a standard calculation.
        ///
        /// \param Base       The base value of the stat.
        /// \param Flat       The flat addition to apply to the base value.
        /// \param Additive   The percentage addition to apply to the base value.
        /// \param Multiplier The multiplier to apply to the base value.
        /// \return The calculated final value using the default formula.
        ZYPHRYON_INLINE static constexpr Real32 Default(Real32 Base, Real32 Flat, Real32 Additive, Real32 Multiplier)
        {
            return (Base + Flat) * (1.0f + Additive) * Multiplier;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Calculator                           mCalculator;
        Vector<Dependency, kMaxDependencies> mDependencies;
    };
}