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

#include "Gameplay/Stat/Stat.hpp"
#include "Gameplay/Stat/StatTypes.hpp"
#include "Gameplay/Token/Token.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a formula used to calculate the effective value of a stat based on dependencies.
    class StatFormula final
    {
    public:

        /// \brief Maximum number of stats in a computation snapshot.
        static constexpr UInt32 kMaxStats  = 10;   // TODO: Macro Configurable

        /// \brief Maximum number of tokens in a computation snapshot.
        static constexpr UInt32 kMaxTokens = 4;    // TODO: Macro Configurable

        /// \brief Structure representing the dependency graph for a stat calculation.
        struct Graph final
        {
            /// \brief Default constructor, initializes an empty graph.
            ZYPHRYON_INLINE Graph()
                : mScopes { }
            {
            }

            /// \brief Adds a source stat dependency to the graph.
            ///
            /// \param Dependency The stat handle that is a source dependency.
            ZYPHRYON_INLINE void AddSourceDependency(Stat Dependency)
            {
                LOG_ASSERT(mStats.size() < kMaxStats, "Exceeded maximum number of dependencies.");

                mScopes[mStats.size()] = StatScope::Source;
                mStats.emplace_back(Dependency);
            }

            /// \brief Adds a source token dependency to the graph.
            ///
            /// \param Dependency The token handle that is a source dependency.
            ZYPHRYON_INLINE void AddSourceDependency(Token Dependency)
            {
                LOG_ASSERT(mTokens.size() < kMaxTokens, "Exceeded maximum number of dependencies.");

                mScopes[kMaxStats + mTokens.size()] = StatScope::Source;
                mTokens.emplace_back(Dependency);
            }

            /// \brief Adds a target stat dependency to the graph.
            ///
            /// \param Dependency The stat handle that is a target dependency.
            ZYPHRYON_INLINE void AddTargetDependency(Stat Dependency)
            {
                LOG_ASSERT(mStats.size() < kMaxStats, "Exceeded maximum number of dependencies.");

                mScopes[mStats.size()] = StatScope::Target;
                mStats.emplace_back(Dependency);
            }

            /// \brief Adds a target token dependency to the graph.
            ///
            /// \param Dependency The token handle that is a target dependency.
            ZYPHRYON_INLINE void AddTargetDependency(Token Dependency)
            {
                LOG_ASSERT(mTokens.size() < kMaxTokens, "Exceeded maximum number of dependencies.");

                mScopes[kMaxStats + mTokens.size()] = StatScope::Target;
                mTokens.emplace_back(Dependency);
            }

            /// \brief Traverses all dependencies in the graph, invoking the provided action for each.
            ///
            /// \param Action The action to invoke for each dependency.
            template<typename Function>
            ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
            {
                for (const Stat Dependency : mStats)
                {
                    Action(Dependency);
                }

                for (const Token Dependency : mTokens)
                {
                    Action(Dependency);
                }
            }

            /// \brief Traverses dependencies in the graph filtered by scope, invoking the provided action for each.
            ///
            /// \param Action The action to invoke for each dependency.
            /// \param Scope  The scope to filter dependencies by.
            template<typename Function>
            ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatScope Scope) const
            {
                for (UInt32 Index = 0; Index < mStats.size(); ++Index)
                {
                    if (mScopes[Index] == Scope)
                    {
                        Action(mStats[Index]);
                    }
                }

                for (UInt32 Index = 0; Index < mTokens.size(); ++Index)
                {
                    if (mScopes[Index + kMaxStats] == Scope)
                    {
                        Action(mTokens[Index]);
                    }
                }
            }

            /// \brief Array of scopes corresponding to each stat and token in the graph.
            Array<StatScope, kMaxStats + kMaxTokens> mScopes;

            /// \brief Arrays of stats and tokens involved in the graph.
            Vector<Stat,     kMaxStats>              mStats;

            /// \brief Arrays of tokens involved in the graph.
            Vector<Token,    kMaxTokens>             mTokens;
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
                  Additive   { Additive   },
                  Multiplier { Multiplier },
                  Stats      { },
                  Tokens     { }
            {
            }

            /// \brief Populates the snapshots from a single context based on the dependencies.
            ///
            /// \param Source       The source context to retrieve stat values from.
            /// \param Dependencies The dependency graph defining which stats to retrieve.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Source, ConstRef<Graph> Dependencies)
            {
                for (UInt32 Index = 0; Index < Dependencies.mStats.size(); ++Index)
                {
                    Stats[Index] = Source.GetStat(Dependencies.mStats[Index]);
                }

                for (UInt32 Index = 0; Index < Dependencies.mTokens.size(); ++Index)
                {
                    Tokens[Index] = Source.GetToken(Dependencies.mTokens[Index]);
                }
            }

            /// \brief Populates the snapshots from both source and target contexts based on the dependencies.
            ///
            /// \param Source       The source context to retrieve stat values from.
            /// \param Target       The target context to retrieve stat values from.
            /// \param Dependencies The dependency graph defining which stats to retrieve.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Source, ConstRef<Context> Target, ConstRef<Graph> Dependencies)
            {
                for (UInt32 Index = 0; Index < Dependencies.mStats.size(); ++Index)
                {
                    if (Dependencies.mScopes[Index] == StatScope::Source)
                    {
                        Stats[Index] = Source.GetStat(Dependencies.mStats[Index]);
                    }
                    else
                    {
                        Stats[Index] = Target.GetStat(Dependencies.mStats[Index]);
                    }
                }

                for (UInt32 Index = 0; Index < Dependencies.mTokens.size(); ++Index)
                {
                    if (Dependencies.mScopes[Index + kMaxStats] == StatScope::Source)
                    {
                        Tokens[Index] = Source.GetToken(Dependencies.mTokens[Index]);
                    }
                    else
                    {
                        Tokens[Index] = Target.GetToken(Dependencies.mTokens[Index]);
                    }
                }
            }

            /// \brief The base value of the stat.
            Real32                    Base;

            /// \brief The flat addition to apply to the base value.
            Real32                    Flat;

            /// \brief The percentage addition to apply to the base value.
            Real32                    Additive;

            /// \brief The multiplier to apply to the base value.
            Real32                    Multiplier;

            /// \brief Snapshot of stat values at the time of computation.
            Array<Real32, kMaxStats>  Stats;

            /// \brief Snapshot of token counters at the time of computation.
            Array<UInt32, kMaxTokens> Tokens;
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

        /// \brief Adds a source stat dependency to the formula.
        ///
        /// \param Dependency The stat handle that is a source dependency.
        ZYPHRYON_INLINE void AddSourceDependency(Stat Dependency)
        {
            mDependencies.AddSourceDependency(Dependency);
        }

        /// \brief Adds a target stat dependency to the formula.
        ///
        /// \param Dependency The stat handle that is a target dependency.
        ZYPHRYON_INLINE void AddSourceDependency(Token Dependency)
        {
            mDependencies.AddSourceDependency(Dependency);
        }

        /// \brief Adds a source token dependency to the formula.
        ///
        /// \param Dependency The token handle that is a source dependency.
        ZYPHRYON_INLINE void AddTargetDependency(Stat Dependency)
        {
            mDependencies.AddTargetDependency(Dependency);
        }

        /// \brief Adds a target token dependency to the formula.
        ///
        /// \param Dependency The token handle that is a target dependency.
        ZYPHRYON_INLINE void AddTargetDependency(Token Dependency)
        {
            mDependencies.AddTargetDependency(Dependency);
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

        /// \brief Traverses all dependencies in the formula, invoking the provided action for each.
        ///
        /// \param Action The action to invoke for each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            mDependencies.Traverse(Action);
        }

        /// \brief Traverses dependencies in the formula filtered by scope, invoking the provided action for each.
        ///
        /// \param Action The action to invoke for each dependency.
        /// \param Scope  The scope to filter dependencies by.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatScope Scope) const
        {
            mDependencies.Traverse(Action, Scope);
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

        Calculator mCalculator;
        Graph      mDependencies;
    };
}