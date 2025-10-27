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

#include <Zyphryon.Base/Base.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the fundamental nature of a stat.
    enum class StatKind : UInt8
    {
        Attribute,  ///< A derived statistic calculated from formulas.
        Resource,   ///< A depletable resource.
    };

    /// \brief Defines how a stat's value is resolved.
    enum class StatMode : UInt8
    {
        Snapshot,   ///< The stat is resolved once and cached.
        Dynamic,    ///< The stat is resolved dynamically in real-time.
    };

    /// \brief Defines how a stat modifier affects a stat's value.
    enum class StatModifier : UInt8
    {
        Add,        ///< Adds or subtracts a flat value to/from the stat.
        Percent,    ///< Adds or subtracts a percentage to/from the stat (e.g., +0.2 for +20%).
        Scale,      ///< Multiplies the stat by a factor (e.g., x1.5 for 50% increase, or x0.5 for 50% decrease).
        Set,        ///< Overrides the stat value entirely.
    };

    /// \brief Defines the scope of a stat, indicating whether it applies to the source or target context.
    enum class StatScope : UInt8
    {
        Source,     ///< The stat originates from the source context.
        Target,     ///< The stat originates from the target context.
    };
}