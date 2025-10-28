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

#include "Gameplay/Token/TokenFamily.hpp"

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
            Any,        ///< Targets any valid entity.
            Category,   ///< Targets a category of entities.
            Area,       ///< Targets a specific area.
            None,       ///< No specific target, used for self-contained abilities.
        };

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE AbilityTarget()
            : mKind { Kind::Self }
        {
        }

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

        /// \brief Sets the requirement token family for this ability.
        ///
        /// \param Requirement The token family defining the requirements.
        ZYPHRYON_INLINE void SetRequirement(AnyRef<TokenFamily> Requirement)
        {
            mRequirement = Requirement;
        }

        /// \brief Retrieves the requirement token family for this ability.
        ///
        /// \return The token family defining the requirements.
        ZYPHRYON_INLINE ConstRef<TokenFamily> GetRequirement() const
        {
            return mRequirement;
        }

        /// \brief Loads the ability target data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE void Load(TOMLSection Section)
        {
            mKind = Section.GetEnum("Kind", Kind::Any);
            mRequirement.Load(Section.GetArray("Requirement"));
        }

        /// \brief Saves the ability target data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        ZYPHRYON_INLINE void Save(TOMLSection Section) const
        {
            Section.SetEnum("Kind", mKind);
            mRequirement.Save(Section.SetArray("Requirement"));
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Kind        mKind;
        TokenFamily mRequirement;
    };
}