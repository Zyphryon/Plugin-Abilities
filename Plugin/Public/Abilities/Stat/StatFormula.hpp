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

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

inline namespace Gameplay
{
    /// \brief Defines a formula for calculating the final value of a stat.
    class StatFormula : public Trackable
    {
    public:

        /// \brief Maximum number of dependencies a formula can have.
        static constexpr UInt32 kMaxDependencies = 8;

        /// \brief Structure representing the components of a stat calculation.
        struct Computation
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
            /// \param Source       The source context providing access to stats.
            /// \param Dependencies List of stat handles that this formula depends on.
            template<typename Context>
            ZYPHRYON_INLINE void Populate(ConstRef<Context> Source, List<StatHandle> Dependencies)
            {
                for (const StatHandle Dependency : Dependencies)
                {
                    Values.push_back(Source.GetStat(Dependency));
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

    public:

        /// \brief Calculates the final value of a stat based on its components and context.
        ///
        /// \param Computation The components required for the calculation.
        /// \return The calculated final value of the stat.
        virtual Real32 Calculate(ConstRef<Computation> Computation) const = 0;

        /// \brief Retrieves a list of stat handles that this formula depends on.
        ///
        /// \return A list of dependent stat handles.
        virtual List<StatHandle> GetDependencies() const = 0;

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
    };
}