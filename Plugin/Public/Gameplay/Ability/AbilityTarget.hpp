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

#include "Gameplay/Token/Token.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the target structure for an ability.
    class AbilityTarget final
    {
    public:

        /// \brief Enumerates the possible targets for abilities.
        enum class Kind : UInt8
        {
            Self,       ///< Targets the ability user themselves.
            Token,      ///< Targets a specific token.
            Any,        ///< Targets any valid entity.
            Area,       ///< Targets a specific area.
            None,       ///< No specific target, used for self-contained abilities.
        };

        /// \brief Maximum number of requirements an ability target can have.
        static constexpr UInt32 kMaxRequirements = 2; // TODO: Macro Configurable

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE AbilityTarget() = default;

        /// \brief Sets the kind of target for this ability.
        ///
        /// \param Kind The kind of target to set.
        ZYPHRYON_INLINE void SetKind(Kind Kind)
        {
            mKind = Kind;
        }

        /// \brief Retrieves the kind of target for this ability.
        ///
        /// \return The kind of target.
        ZYPHRYON_INLINE Kind GetKind() const
        {
            return mKind;
        }

        /// \brief Sets the requirements for this ability target.
        ///
        /// \param Requirements A constant span of tokens representing the requirements to set.
        ZYPHRYON_INLINE void SetRequirements(ConstSpan<Token> Requirements)
        {
            mRequirements.assign(Requirements.begin(), Requirements.end());
        }

        /// \brief Retrieves the requirements for this ability target.
        ///
        /// \return A constant span of tokens representing the requirements.
        ZYPHRYON_INLINE ConstSpan<Token> GetRequirements() const
        {
            return mRequirements;
        }

        /// \brief Loads the ability target data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE void Load(TOMLSection Section)
        {
            mKind = Enum::Cast(Section.GetString("Kind"), Kind::Any);

            if (const TOMLArray Requirements = Section.GetArray("Requirements"); !Requirements.IsEmpty())
            {
                mRequirements.resize(Requirements.GetSize());

                for (UInt32 Element = 0; Element < mRequirements.size(); ++Element)
                {
                    mRequirements[Element] = Requirements.GetInteger(Element);
                }
            }
        }

        /// \brief Saves the ability target data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        ZYPHRYON_INLINE void Save(TOMLSection Section) const
        {
            Section.SetString("Kind", Enum::GetName(mKind));

            for (TOMLArray Requirements = Section.SetArray("Requirements"); Token Requirement : mRequirements)
            {
                Requirements.AddInteger(Requirement.GetID());
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Kind                            mKind;
        Vector<Token, kMaxRequirements> mRequirements;
    };
}