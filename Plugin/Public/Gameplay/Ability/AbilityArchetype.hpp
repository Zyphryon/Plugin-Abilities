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

#include "Gameplay/Ability/Ability.hpp"
#include "Gameplay/Ability/AbilityCooldown.hpp"
#include "Gameplay/Ability/AbilityCost.hpp"
#include "Gameplay/Ability/AbilityTypes.hpp"
#include "Gameplay/Ability/AbilityTarget.hpp"
#include "Gameplay/Effect/EffectSpec.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of an ability, including its properties and behavior policies.
    class AbilityArchetype final
    {
    public:

        /// \brief Maximum number of effects an ability can have.
        static constexpr UInt32 kMaxEffects = 4; // TODO: Macro Configurable

    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE AbilityArchetype()
            : mKind { AbilityKind::Active }
        {
        }

        /// \brief Constructs an ability archetype by loading data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        ZYPHRYON_INLINE AbilityArchetype(TOMLSection Section)
        {
            Load(Section);
        }

        /// \brief Destruct an ability archetype.
        ZYPHRYON_INLINE ~AbilityArchetype()
        {
            mHandle.Reset();
        }

        /// \brief Checks if the ability archetype has a valid handle.
        ///
        /// \return `true` if the ability archetype is valid, `false` otherwise.
        ZYPHRYON_INLINE Bool IsValid() const
        {
            return mHandle.IsValid();
        }

        /// \brief Sets the unique handle for this ability archetype.
        ///
        /// \param Handle The unique ability handle to assign.
        ZYPHRYON_INLINE void SetHandle(Ability Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the unique handle of this ability archetype.
        ///
        /// \return The ability handle.
        ZYPHRYON_INLINE Ability GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the kind of the ability archetype.
        ///
        /// \param Kind The ability kind to assign.
        ZYPHRYON_INLINE void SetKind(AbilityKind Kind)
        {
            mKind = Kind;
        }

        /// \brief Retrieves the kind of the ability archetype.
        ///
        /// \return The ability kind.
        ZYPHRYON_INLINE AbilityKind GetKind() const
        {
            return mKind;
        }

        /// \brief Sets the category of the ability archetype.
        ///
        /// \param Category The category to assign.
        ZYPHRYON_INLINE void SetCategory(AnyRef<TokenFamily> Category)
        {
            mCategory = Category;
        }

        /// \brief Retrieves the category of the ability archetype.
        ///
        /// \return The effect category.
        ZYPHRYON_INLINE ConstRef<TokenFamily> GetCategory() const
        {
            return mCategory;
        }

        /// \brief Sets the name of the ability archetype.
        ///
        /// \param Name The name to assign.
        ZYPHRYON_INLINE void SetName(ConstStr8 Name)
        {
            mName = Name;
        }

        /// \brief Retrieves the name of the ability archetype.
        ///
        /// \return The ability name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            return mName;
        }

        /// \brief Sets the cooldown configuration for the ability.
        ///
        /// \param Cooldown The cooldown configuration to assign.
        ZYPHRYON_INLINE void SetCooldown(AnyRef<AbilityCooldown> Cooldown)
        {
            mCooldown = Move(Cooldown);
        }

        /// \brief Retrieves the cooldown configuration for the ability.
        ///
        /// \return The ability cooldown configuration.
        ZYPHRYON_INLINE ConstRef<AbilityCooldown> GetCooldown() const
        {
            return mCooldown;
        }

        /// \brief Sets the cost configuration for the ability.
        ///
        /// \param Cost The cost configuration to assign.
        ZYPHRYON_INLINE void SetCost(AnyRef<AbilityCost> Cost)
        {
            mCost = Move(Cost);
        }

        /// \brief Retrieves the cost configuration for the ability.
        ///
        /// \return The ability cost configuration.
        ZYPHRYON_INLINE ConstRef<AbilityCost> GetCost() const
        {
            return mCost;
        }

        /// \brief Sets the target configuration for the ability.
        ///
        /// \param Target The target configuration to assign.
        ZYPHRYON_INLINE void SetTarget(AnyRef<AbilityTarget> Target)
        {
            mTarget = Move(Target);
        }

        /// \brief Retrieves the target configuration for the ability.
        ///
        /// \return The ability target configuration.
        ZYPHRYON_INLINE ConstRef<AbilityTarget> GetTarget() const
        {
            return mTarget;
        }

        /// \brief Sets the effects associated with this ability archetype.
        ///
        /// \param Effects A span of effect specifications to assign.
        ZYPHRYON_INLINE void SetEffects(ConstSpan<EffectSpec> Effects)
        {
            mEffects.assign(Effects.begin(), Effects.end());
        }

        /// \brief Retrieves the effects associated with this ability archetype.
        ///
        /// \return A span of effect specifications.
        ZYPHRYON_INLINE ConstSpan<EffectSpec> GetEffects() const
        {
            return mEffects;
        }

        /// \brief Iterates over all dependencies referenced by this archetype.
        ///
        /// \param Action The action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            // Traverse cooldown inputs.
            mCooldown.Traverse(Action);

            // Traverse cost inputs.
            mCost.Traverse(Action);

            // Traverse effect specifications.
            for (ConstRef<EffectSpec> Effect : mEffects)
            {
                Effect.Traverse(Action);
            }
        }

        /// \brief Loads the ability archetype data from a TOML section.
        ///
        /// \param Section The TOML section to load from.
        void Load(TOMLSection Section);

        /// \brief Saves the ability archetype data to a TOML section.
        ///
        /// \param Section The TOML section to save to.
        void Save(TOMLSection Section) const;

        /// \brief Generates a hash value for the ability archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the ability archetype.
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mHandle.Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Ability                         mHandle;
        AbilityKind                     mKind;
        TokenFamily                     mCategory;
        Str8                            mName;
        AbilityCooldown                 mCooldown;
        AbilityCost                     mCost;
        AbilityTarget                   mTarget;
        Vector<EffectSpec, kMaxEffects> mEffects;

        // TODO: Requirements?
        // TODO: Cast Time?
        // TODO: Range?
    };
}