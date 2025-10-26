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

#include "Gameplay/Stat/StatInput.hpp"
#include "Gameplay/Token/Token.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the cooldown structure for an ability.
    class AbilityCooldown final
    {
    public:

        /// \brief Defines the scope of the cooldown's effect.
        enum class Influence : UInt8
        {
            Individual, ///< Affects only the specific ability.
            Category,   ///< Affects all abilities in the same category.
        };

        /// \brief Defines the mechanism by which the cooldown is applied.
        enum class Mechanism : UInt8
        {
            Timer,      ///< Cooldown is based on a timer duration.
            Charges,    ///< Cooldown is based on a limited number of charges.
        };

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE AbilityCooldown() = default;

        /// \brief Constructs an ability cooldown with specified parameters.
        ///
        /// \param Influence The influence type of the cooldown.
        /// \param Mechanism The mechanism type of the cooldown.
        /// \param Category  The category token associated with the cooldown.
        /// \param Cooldown  The cooldown duration or charge replenishment rate.
        /// \param Limit     The maximum number of charges for the cooldown mechanism.
        ZYPHRYON_INLINE AbilityCooldown(Influence Influence, Mechanism Mechanism, Token Category, AnyRef<StatInput> Cooldown, AnyRef<StatInput> Limit)
            : mInfluence  { Influence },
              mMechanism  { Mechanism },
              mCategory   { Category },
              mCooldown   { Move(Cooldown) },
              mLimit      { Move(Limit) }
        {
        }

        /// \brief Retrieves the influence type of the cooldown.
        ///
        /// \return The influence type.
        ZYPHRYON_INLINE Influence GetInfluence() const
        {
            return mInfluence;
        }

        /// \brief Retrieves the mechanism type of the cooldown.
        ///
        /// \return The mechanism type.
        ZYPHRYON_INLINE Mechanism GetMechanism() const
        {
            return mMechanism;
        }

        /// \brief Retrieves the category token associated with the cooldown.
        ///
        /// \return The category token.
        ZYPHRYON_INLINE Token GetCategory() const
        {
            return mCategory;
        }

        /// \brief Retrieves the cooldown duration or charge replenishment rate.
        ///
        /// \return The cooldown duration or charge replenishment rate.
        ZYPHRYON_INLINE ConstRef<StatInput> GetCooldown() const
        {
            return mCooldown;
        }

        /// \brief Retrieves the maximum number of charges for the cooldown mechanism.
        ///
        /// \return The maximum number of charges.
        ZYPHRYON_INLINE ConstRef<StatInput> GetLimit() const
        {
            return mLimit;
        }

        /// \brief Traverses all inputs, applying the provided action to each component.
        ///
        /// \param Action The action to apply during traversal.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            // Traverse the cooldown input.
            mCooldown.Traverse(Action);

            // Traverse the limit input.
            mLimit.Traverse(Action);
        }

        /// \brief Loads the ability cooldown data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE void Load(TOMLSection Section)
        {
            mInfluence = Enum::Cast(Section.GetString("Influence"), Influence::Individual);
            mMechanism = Enum::Cast(Section.GetString("Mechanism"), Mechanism::Timer);
            mCategory  = Section.GetInteger("Category");
            mCooldown.Load(Section.GetArray("Cooldown"));

            if (mMechanism == Mechanism::Charges)
            {
                mLimit.Load(Section.GetArray("Limit"));
            }
        }

        /// \brief Saves the ability cooldown data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        ZYPHRYON_INLINE void Save(TOMLSection Section) const
        {
            Section.SetString("Influence", Enum::GetName(mInfluence));
            Section.SetString("Mechanism", Enum::GetName(mMechanism));

            if (mInfluence == Influence::Category)
            {
                Section.SetInteger("Category", mCategory.GetID());
            }

            mCooldown.Save(Section.SetArray("Cooldown"));

            if (mMechanism == Mechanism::Charges)
            {
                mLimit.Save(Section.SetArray("Limit"));
            }
        }

    public:

        /// \brief Creates a time-based ability cooldown.
        ///
        /// \param Influence The influence type of the cooldown.
        /// \param Category  The category token associated with the cooldown.
        /// \param Cooldown  The cooldown duration.
        /// \return The constructed ability cooldown.
        ZYPHRYON_INLINE static AbilityCooldown CreateTimeBased(Influence Influence, Token Category, AnyRef<StatInput> Cooldown)
        {
            return AbilityCooldown { Influence, Mechanism::Timer, Category, Move(Cooldown), StatInput() };
        }

        /// \brief Creates a charge-based ability cooldown.
        ///
        /// \param Influence The influence type of the cooldown.
        /// \param Category  The category token associated with the cooldown.
        /// \param Cooldown  The charge replenishment rate.
        /// \param Limit     The maximum number of charges.
        /// \return The constructed ability cooldown.
        ZYPHRYON_INLINE static AbilityCooldown CreateChargeBased(Influence Influence, Token Category, AnyRef<StatInput> Cooldown, AnyRef<StatInput> Limit)
        {
            return AbilityCooldown { Influence, Mechanism::Charges, Category, Move(Cooldown), Move(Limit) };
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Influence  mInfluence;
        Mechanism  mMechanism;
        Token      mCategory;
        StatInput  mCooldown;
        StatInput  mLimit;
    };
}