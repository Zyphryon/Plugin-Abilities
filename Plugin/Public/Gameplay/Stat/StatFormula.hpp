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
    /// \brief Defines a formula for calculating the final value of a stat.
    class StatFormula : public Trackable
    {
    public:

        /// \brief Maximum number of dependencies a formula can have.
        static constexpr UInt32 kMaxDependencies = 8;

        /// \brief Structure representing the components of a stat calculation.
        struct Computation final
        {
            /// \brief Constructs a `Computation` instance with the given parameters.
            ///
            /// \param Base       The base value of the stat.
            /// \param Flat       The flat modifier to be added to the base.
            /// \param Additive   The additive percentage modifier (e.g., 0.2 for +20%).
            /// \param Multiplier The multiplicative factor to apply (e.g., 1.5 for +50%).
            ZYPHRYON_INLINE Computation(Real32 Base, Real32 Flat, Real32 Additive, Real32 Multiplier)
                : Base       { Base },
                  Flat       { Flat },
                  Additive   { Additive },
                  Multiplier { Multiplier }
            {
            }

            /// \brief Populates the snapshots of dependent stat values from the source context.
            ///
            /// \param Target       The context providing access to stats.
            /// \param Dependencies List of stat handles that this formula depends on.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Target, ConstSpan<StatHandle> Dependencies)
            {
                for (const StatHandle Dependency : Dependencies)
                {
                    Values.push_back(Target.GetStat(Dependency));
                }
            }

            /// \brief The base value of the stat.
            Real32                           Base;

            /// \brief The flat modifier to be added to the base.
            Real32                           Flat;

            /// \brief The additive percentage modifier (e.g., 0.2 for +20%).
            Real32                           Additive;

            /// \brief The multiplicative factor to apply (e.g., 1.5 for +50%).
            Real32                           Multiplier;

            /// \brief Snapshots of dependent stat values at the time of computation.
            Vector<Real32, kMaxDependencies> Values;
        };

        /// \brief Type alias for a function that computes a stat value from a `Computation`.
        using Calculator = Delegate<Real32(ConstRef<Computation>), DelegateInlineSize::Smallest>;

    public:

        /// \brief Constructs a formula with the specified calculation function.
        ///
        /// \param Calculator The function used to calculate the stat value.
        ZYPHRYON_INLINE StatFormula(AnyRef<Calculator> Calculator)
            : mCalculator { Move(Calculator) }
        {
        }

        /// \brief Calculates the final value of a stat based on its components and context.
        ///
        /// \param Computation The components required for the calculation.
        /// \return The calculated final value of the stat.
        ZYPHRYON_INLINE Real32 Calculate(ConstRef<Computation> Computation) const
        {
            return mCalculator(Computation);
        }

        /// \brief Retrieves a list of stat handles that this formula depends on.
        ///
        /// \return A constant span of dependent stat handles.
        ZYPHRYON_INLINE ConstSpan<StatHandle> GetDependencies() const
        {
            return mDependencies;
        }

    protected:

        /// \brief Adds a dependency stat handle to this formula.
        ///
        /// \param Dependency The stat handle that this formula depends on.
        ZYPHRYON_INLINE void AddDependency(StatHandle Dependency)
        {
            LOG_ASSERT(mDependencies.size() < kMaxDependencies, "Exceeded maximum number of dependencies for formula.");

            mDependencies.push_back(Dependency);
        }

    public:

        /// \brief Default formula implementation that applies a standard calculation.
        ///
        /// \param Base       The base value of the stat.
        /// \param Flat       The flat modifier to be added to the base.
        /// \param Additive   The additive percentage modifier (e.g., 0.2 for +20%).
        /// \param Multiplier The multiplicative factor to apply (e.g., 1.5 for +50%).
        /// \return The calculated final value using the default formula.
        ZYPHRYON_INLINE static constexpr Real32 Default(Real32 Base, Real32 Flat, Real32 Additive, Real32 Multiplier)
        {
            return (Base + Flat) * (1.0f + Additive) * Multiplier;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Calculator                           mCalculator;
        Vector<StatHandle, kMaxDependencies> mDependencies;
    };

    /// \brief Helper class to simplify creating custom stat formulas by inheriting and implementing the `Compute` method.
    ///
    /// \tparam Impl The derived class implementing the `Compute` method.
    template<typename Impl>
    class AbstractStatFormula : public StatFormula
    {
    public:

        /// \brief Default constructor that sets up the calculator to use the derived class's `Compute` method.
        ZYPHRYON_INLINE AbstractStatFormula()
            : StatFormula(Calculator::Create<&Impl::Compute>(static_cast<Ptr<Impl>>(this)))
        {
        }
    };
}