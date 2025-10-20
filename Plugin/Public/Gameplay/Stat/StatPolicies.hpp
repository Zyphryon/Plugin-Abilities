// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2025 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the category of a stat.
    enum class StatCategory : UInt8
    {
        Attribute,      ///< A derived statistic calculated from formulas.
        Resource,       ///< A depletable resource.
    };

    /// \brief Defines how a stat is evaluated.
    enum class StatEvaluation : UInt8
    {
        Snapshot,       ///< The stat is resolved using a snapshot of values.
        Live,           ///< The stat is resolved using live, real-time values.
    };

    /// \brief Defines the operation applied to a stat.
    enum class StatOperator : UInt8
    {
        Flat,           ///< Directly adds or subtracts a flat value to/from the stat.
        Additive,       ///< Adds or subtracts a percentage to/from the stat (e.g., +0.2 for +20%).
        Multiplicative, ///< Multiplies the stat by a factor (e.g., x1.5 for 50% increase).
        Divisive,       ///< Divides the stat by a factor (e.g., /1.5 for 33% decrease).
        Replace,        ///< Overrides the stat value entirely.
    };

    /// \brief Defines the origin context of a stat.
    enum class StatOrigin : UInt8
    {
        Source,         ///< The stat originates from the source context.
        Target,         ///< The stat originates from the target context.
    };
}