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
    /// \brief Enumeration defining how an effect is applied.
    enum class EffectApplication : UInt8
    {
        Instant,     ///< The effect is applied instantly and does not persist.
        Temporary,   ///< The effect lasts for a specified duration before expiring.
        Permanent,   ///< The effect lasts indefinitely until explicitly removed.
    };

    /// \brief Enumeration defining how an effect stack expires.
    enum class EffectExpiration : UInt8
    {
        All,         ///< All stacks of the effect expire simultaneously.
        Single,      ///< Only one stack of the effect expires at a time.
        Tick,        ///< Only one stack of the effect expires per tick.
    };

    /// \brief Enumeration defining how an effect refreshes when reapplied.
    enum class EffectRefresh : UInt8
    {
        Keep,        ///< Keeps the remaining duration of the existing effect.
        Replace,     ///< Replaces the remaining duration with the new effect's duration.
        Longest,     ///< Keeps the longer duration between the existing and new effect.
        Extend,      ///< Extends the existing effect's duration by the new effect's duration.
    };

    /// \brief Enumeration defining how an effect resolves when reapplied.
    enum class EffectResolution : UInt8
    {
        Additive,    ///< The intensities of the old and new effects are summed.
        Replace,     ///< The intensity of the new effect replaces the old one entirely.
        Highest,     ///< Only the effect with the highest intensity is kept or merge together if equal.
        Lowest,      ///< Only the effect with the lowest intensity is kept or merge together if equal.
        Average,     ///< The intensities of the old and new effects are averaged.
    };

    /// \brief Enumeration defining how multiple stacks of an effect interact.
    enum class EffectStack : UInt8
    {
        Linear,      ///< Each stack multiplies the effect's intensity.
        Exponential, ///< Each stack increases the effect's intensity exponentially.
        Diminish,    ///< Each stack increases the effect's intensity but with diminishing returns.
        Independent, ///< Each stack is independent and does not affect intensity.
    };
}