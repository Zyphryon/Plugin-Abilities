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

    /// \brief Defines the policy rules for effect behavior.
    class EffectPolicy final
    {
    public:

        /// \brief Default constructor, initializes policies to default values.
        ZYPHRYON_INLINE EffectPolicy()
            : mPolicies { 0 }
        {
        }

        /// \brief Sets the application policy for this effect.
        ///
        /// \param Policy The application policy to assign.
        ZYPHRYON_INLINE void SetApplication(EffectApplication Policy)
        {
            mPolicies = SetBit(mPolicies, 0, 0b111, Enum::Cast(Policy));
        }

        /// \brief Retrieves the application policy of this effect.
        ///
        /// \return The effect application policy.
        ZYPHRYON_INLINE EffectApplication GetApplication() const
        {
            return static_cast<EffectApplication>(GetBit(mPolicies, 0, 0b111));
        }

        /// \brief Sets the expiration policy for this effect.
        ///
        /// \param Policy The expiration policy to assign.
        ZYPHRYON_INLINE void SetExpiration(EffectExpiration Policy)
        {
            mPolicies = SetBit(mPolicies, 3, 0b111, Enum::Cast(Policy));
        }

        /// \brief Retrieves the expiration policy of this effect.
        ///
        /// \return The effect expiration policy.
        ZYPHRYON_INLINE EffectExpiration GetExpiration() const
        {
            return static_cast<EffectExpiration>(GetBit(mPolicies, 3, 0b111));
        }

        /// \brief Sets the refresh policy for this effect.
        ///
        /// \param Policy The refresh policy to assign.
        ZYPHRYON_INLINE void SetRefresh(EffectRefresh Policy)
        {
            mPolicies = SetBit(mPolicies, 6, 0b111, Enum::Cast(Policy));
        }

        /// \brief Retrieves the refresh policy of this effect.
        ///
        /// \return The effect refresh policy.
        ZYPHRYON_INLINE EffectRefresh GetRefresh() const
        {
            return static_cast<EffectRefresh>(GetBit(mPolicies, 6, 0b111));
        }

        /// \brief Sets the resolution policy for this effect.
        ///
        /// \param Policy The resolution policy to assign.
        ZYPHRYON_INLINE void SetResolution(EffectResolution Policy)
        {
            mPolicies = SetBit(mPolicies, 9, 0b111, Enum::Cast(Policy));
        }

        /// \brief Retrieves the resolution policy of this effect.
        ///
        /// \return The effect resolution policy.
        ZYPHRYON_INLINE EffectResolution GetResolution() const
        {
            return static_cast<EffectResolution>(GetBit(mPolicies, 9, 0b111));
        }

        /// \brief Sets the scaling policy for this effect.
        ///
        /// \param Policy The scaling policy to assign.
        ZYPHRYON_INLINE void SetStack(EffectStack Policy)
        {
            mPolicies = SetBit(mPolicies, 12, 0b111, Enum::Cast(Policy));
        }

        /// \brief Retrieves the scaling policy of this effect.
        ///
        /// \return The effect scaling policy.
        ZYPHRYON_INLINE EffectStack GetStack() const
        {
            return static_cast<EffectStack>(GetBit(mPolicies, 12, 0b111));
        }

        /// \brief Loads effect policies from a TOML section.
        ///
        /// \param Section The TOML section containing effect policy data.
        ZYPHRYON_INLINE void Load(TOMLSection Section)
        {
            const EffectApplication Application = Section.GetEnum("Application", EffectApplication::Temporary);
            SetApplication(Application);

            if (Application == EffectApplication::Temporary)
            {
                SetExpiration(Section.GetEnum("Expiration", EffectExpiration::Single));
                SetRefresh(Section.GetEnum("Refresh", EffectRefresh::Replace));
                SetResolution(Section.GetEnum("Resolution", EffectResolution::Additive));
                SetStack(Section.GetEnum("Stack", EffectStack::Linear));
            }
        }

        /// \brief Saves effect policies to a TOML section.
        ///
        /// \param Section The TOML section to save effect policy data into.
        ZYPHRYON_INLINE void Save(TOMLSection Section) const
        {
            const EffectApplication Application = GetApplication();
            Section.SetEnum("Application", Application);

            if (Application == EffectApplication::Temporary)
            {
                Section.SetEnum("Expiration", GetExpiration());
                Section.SetEnum("Refresh", GetRefresh());
                Section.SetEnum("Resolution", GetResolution());
                Section.SetEnum("Stack", GetStack());
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        UInt16 mPolicies;
    };
}