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
#include <Zyphryon.Base/Collection.hpp>
#include <Zyphryon.Base/Delegate.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a formula used to calculate the effective value of a stat based on dependencies.
    class StatEvaluator : public Trackable<StatEvaluator>
    {
    public:

        /// \brief Maximum number of dependencies a formula can have for source and target each.
        static constexpr UInt32 kMaxDependencies = 5;   // TODO: Macro Configurable

        /// \brief Structure representing the components of a stat calculation.
        struct Computation final
        {
            /// \brief Populates the snapshots of dependent stat values from the source and target context.
            ///
            /// \param Source             The context providing access to source stats.
            /// \param SourceDependencies The list of stat handles that this formula depends on from the source.
            /// \param Target             The context providing access to target stats.
            /// \param TargetDependencies The list of stat handles that this formula depends on from the target.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(
                ConstRef<Context> Source, ConstSpan<StatHandle> SourceDependencies,
                ConstRef<Context> Target, ConstSpan<StatHandle> TargetDependencies)
            {
                for (UInt32 Index = 0; Index < SourceDependencies.size(); ++Index)
                {
                    SourceSnapshot[Index] = Source.GetStat(SourceDependencies[Index]);
                }

                for (UInt32 Index = 0; Index < TargetDependencies.size(); ++Index)
                {
                    TargetSnapshot[Index] = Target.GetStat(TargetDependencies[Index]);
                }
            }

            /// \brief Snapshots of source stat values at the time of computation.
            Array<Real32, kMaxDependencies> SourceSnapshot;

            /// \brief Snapshots of target stat values at the time of computation.
            Array<Real32, kMaxDependencies> TargetSnapshot;
        };

        /// \brief Type alias for a function that computes a stat value from a `Computation`.
        using Calculator = Delegate<Real32(ConstRef<Computation>), DelegateInlineSize::Smallest>;

    public:

        /// \brief Constructs a formula with the specified calculation function.
        ///
        /// \param Calculator The function used to compute the stat value.
        ZYPHRYON_INLINE StatEvaluator(AnyRef<Calculator> Calculator)
            : mCalculator { Move(Calculator) }
        {
        }

        /// \brief Destructor for proper cleanup in derived classes.
        virtual ~StatEvaluator() = default;

        /// \brief Calculates the final value of a stat based on the provided source and target contexts.
        ///
        /// \param Source The source context containing source stat snapshots.
        /// \param Target The target context containing target stat snapshots.
        /// \return The calculated final value of the stat.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Context> Source, ConstRef<Context> Target) const
        {
            Computation Computation;
            Computation.Populate(Source, GetSourceDependencies(), Target, GetTargetDependencies());

            return mCalculator(Computation);
        }

        /// \brief Retrieves a list of stat handles that this formula depends on from the source context.
        ///
        /// \return A span of stat handles representing source dependencies.
        ZYPHRYON_INLINE ConstSpan<StatHandle> GetSourceDependencies() const
        {
            return mSourceDependencies;
        }

        /// \brief Retrieves a list of stat handles that this formula depends on from the target context.
        ///
        /// \return A span of stat handles representing target dependencies.
        ZYPHRYON_INLINE ConstSpan<StatHandle> GetTargetDependencies() const
        {
            return mTargetDependencies;
        }

    protected:

        /// \brief Adds a dependency stat handle to the source dependencies of this formula.
        ///
        /// \param Dependency The stat handle that this formula depends on from the source.
        ZYPHRYON_INLINE void AddSourceDependency(StatHandle Dependency)
        {
            LOG_ASSERT(mSourceDependencies.size() < kMaxDependencies, "Exceeded maximum number of dependencies for formula.");

            mSourceDependencies.push_back(Dependency);
        }

        /// \brief Adds a dependency stat handle to the target dependencies of this formula.
        ///
        /// \param Dependency The stat handle that this formula depends on from the target.
        ZYPHRYON_INLINE void AddTargetDependency(StatHandle Dependency)
        {
            LOG_ASSERT(mTargetDependencies.size() < kMaxDependencies, "Exceeded maximum number of dependencies for formula.");

            mTargetDependencies.push_back(Dependency);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Calculator                           mCalculator;
        Vector<StatHandle, kMaxDependencies> mSourceDependencies;
        Vector<StatHandle, kMaxDependencies> mTargetDependencies;
    };

    /// \brief A base class to simplify creating custom stat formulas with automatic dependency management.
    template<typename Impl>
    class AbstractStatEvaluator : public StatEvaluator
    {
    public:

        /// \brief Default constructor that sets up the formula using the derived class's `Compute` method.
        ///
        /// \note The derived class must implement a method with the signature:
        ///       `Real32 Compute(ConstRef<Computation>, ConstRef<Computation>) const;`
        ZYPHRYON_INLINE AbstractStatEvaluator()
            : StatEvaluator(Calculator::Create<&Impl::Compute>(static_cast<Ptr<Impl>>(this)))
        {
        }
    };
}