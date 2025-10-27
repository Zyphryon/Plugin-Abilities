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

#include "Gameplay/Ability/AbilityRepository.hpp"
#include "Gameplay/Ability/AbilitySet.hpp"
#include "Gameplay/Effect/EffectRepository.hpp"
#include "Gameplay/Effect/EffectSet.hpp"
#include "Gameplay/Stat/StatRepository.hpp"
#include "Gameplay/Stat/StatSet.hpp"
#include "Gameplay/Token/TokenRepository.hpp"
#include "Gameplay/Token/TokenSet.hpp"
#include <Zyphryon.Scene/Entity.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Encapsulates a collection of tokens, stats, effects, and abilities for an entity.
    class Arsenal final
    {
        // TODO: Concurrency and Dynamic Modifiers.

    public:

        /// \brief Default constructor, initializes an empty arsenal.
        ZYPHRYON_INLINE Arsenal() = default;

        /// \brief Constructs an arsenal for a specific entity.
        ///
        /// \param Actor The entity associated with the arsenal.
        ZYPHRYON_INLINE Arsenal(Scene::Entity Actor)
            : mActor { Actor }
        {
        }

        /// \brief Advances the state of the arsenal by the given time interval.
        ///
        /// \param Time The time interval to advance.
        void Tick(ConstRef<Time> Time);

        /// \brief Grants an ability to the arsenal.
        ///
        /// \param Handle The handle of the ability to grant.
        ZYPHRYON_INLINE void Grant(Ability Handle)
        {
            ConstRef<AbilityArchetype> Archetype = AbilityRepository::Instance().Get(Handle);
            mAbilities.Insert(Archetype);
        }

        /// \brief Revokes an ability from the arsenal.
        ///
        /// \param Handle The handle of the ability to revoke.
        ZYPHRYON_INLINE void Revoke(Ability Handle)
        {
            mAbilities.Remove(Handle);
        }

        /// \brief Inserts a token into the arsenal with a specified count.
        ///
        /// \param Name  The name of the token to insert.
        /// \param Count The count of the token to insert.
        ZYPHRYON_INLINE void InsertToken(ConstStr8 Name, UInt32 Count = 1)
        {
            const Token Token = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to insert unknown token '{}' into arsenal.", Name);

            InsertToken(Token, Count);
        }

        /// \brief Inserts a token into the arsenal with a specified count.
        ///
        /// \param Handle The handle of the token to insert.
        /// \param Count  The count of the token to insert.
        ZYPHRYON_INLINE void InsertToken(Token Handle, UInt32 Count = 1)
        {
            // Notify listeners of the impending stat change.
            Notify(Handle);

            // Insert the token into the arsenal.
            mTokens.Insert(Handle, Count);
        }

        /// \brief Removes a token from the arsenal with a specified count.
        ///
        /// \param Name  The name of the token to remove.
        /// \param Count The count of the token to remove.
        ZYPHRYON_INLINE void RemoveToken(ConstStr8 Name, UInt32 Count = 1)
        {
            const Token Token = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to remove an unknown token '{}' from arsenal.", Name);

            RemoveToken(Token, Count);
        }

        /// \brief Removes a token from the arsenal with a specified count.
        ///
        /// \param Handle The handle of the token to remove.
        /// \param Count  The count of the token to remove.
        ZYPHRYON_INLINE void RemoveToken(Token Handle, UInt32 Count = 1)
        {
            // Notify listeners of the impending stat change.
            Notify(Handle);

            // Remove the token from the arsenal.
            mTokens.Remove(Handle, Count);
        }

        /// \brief Applies a modifier to the arsenal with a specified magnitude.
        ///
        /// \note Bypasses effect systems and directly modifies stats.
        ///
        /// \param Archetype The archetype of the stat to modify.
        /// \param Operator  The operator to apply.
        /// \param Magnitude The magnitude to apply with the modifier.
        void ApplyModifier(Stat Archetype, StatModifier Operator, Real32 Magnitude);

        /// \brief Reverts a modifier from the arsenal with a specified magnitude.
        ///
        /// \note Bypasses effect systems and directly modifies stats.
        ///
        /// \param Archetype The archetype of the stat to modify.
        /// \param Operator  The operator to revert.
        /// \param Magnitude The magnitude to revert with the modifier.
        void RevertModifier(Stat Archetype, StatModifier Operator, Real32 Magnitude);

        /// \brief Applies an effect to the arsenal using the current entity as the instigator.
        ///
        /// \param Specification The specification of the effect to apply.
        /// \param Timestamp     The timestamp of the effect application.
        /// \return A handle to the applied effect.
        ZYPHRYON_INLINE Effect ApplyEffect(ConstRef<EffectSpec> Specification, Real64 Timestamp = Time::Elapsed())
        {
            return ApplyEffect(Scene::Entity(), Specification, Timestamp);
        }

        /// \brief Applies an effect to the arsenal.
        ///
        /// \param Instigator    The entity that is applying the effect.
        /// \param Specification The specification of the effect to apply.
        /// \param Timestamp     The timestamp of the effect application.
        /// \return A handle to the applied effect.
        Effect ApplyEffect(Scene::Entity Instigator, ConstRef<EffectSpec> Specification, Real64 Timestamp = Time::Elapsed());

        /// \brief Reverts an effect applied to the arsenal.
        ///
        /// \param Handle The handle of the effect to revert.
        void RevertEffect(Effect Handle);

        /// \brief Retrieves the effective value of a stat by its handle.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return The effective value of the stat.
        ZYPHRYON_INLINE Real32 GetStat(Stat Handle) const
        {
            ConstRef<StatArchetype> Archetype = StatRepository::Instance().Get(Handle);

            if (const ConstPtr<StatData> Stat = mStats.TryGet(Handle); Stat)
            {
                return Stat->Calculate(* this);
            }
            return Archetype.Calculate(* this, 0.0f, 0.0f, 1.0f);
        }

        /// \brief Retrieves the count of a token by its name.
        ///
        /// \param Name The name of the token to query.
        /// \return The count of the specified token.
        ZYPHRYON_INLINE UInt32 GetToken(ConstStr8 Name) const
        {
            const Token Handle = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Handle.IsEmpty(), "Attempted to query unknown token '{}' in arsenal.", Name);

            return mTokens.Count(Handle);
        }

        /// \brief Retrieves the count of a specific token in the arsenal.
        ///
        /// \param Handle The token handle to query.
        /// \return The count of the specified token.
        ZYPHRYON_INLINE UInt32 GetToken(Token Handle) const
        {
            return mTokens.Count(Handle);
        }

        /// \brief Checks if the arsenal has a specific token by its name.
        ///
        /// \param Name The name of the token to check.
        /// \return `true` if the token exists in the arsenal, `false` otherwise.
        ZYPHRYON_INLINE Bool HasToken(ConstStr8 Name) const
        {
            return GetToken(Name) > 0;
        }

        /// \brief Checks if the arsenal has a specific token.
        ///
        /// \param Handle The token handle to check.
        /// \return `true` if the token exists in the arsenal, `false` otherwise.
        ZYPHRYON_INLINE Bool HasToken(Token Handle) const
        {
            return GetToken(Handle) > 0;
        }

        /// \brief Traverses each ability in the ability set and applies the given action.
        ///
        /// \param Action The action to apply to each ability.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachAbility(AnyRef<Function> Action)
        {
            mAbilities.Traverse(Action);
        }

        /// \brief Traverses each token in the token set and applies the given action.
        ///
        /// \param Action The action to apply to each token.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachToken(AnyRef<Function> Action)
        {
            mTokens.Traverse(Action);
        }

        /// \brief Traverses each effect in the effect set and applies the given action.
        ///
        /// \param Action The action to apply to each effect.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachEffect(AnyRef<Function> Action)
        {
            mEffects.Traverse(Action);
        }

    private:

        /// \brief Notifies dependant stats of a change in a specific stat.
        ///
        /// \param Dependant The dependant stat or token to notify.
        template<typename Type>
        ZYPHRYON_INLINE void Notify(Type Dependant)
        {
            // TODO: Improve this flow, avoiding repeated calculations when multiple stats depend on the same child.
            StatRepository::Instance().NotifyDependency(Dependant, [this](Stat Dependency)
            {
                const ConstPtr<StatData> Child = mStats.TryGet(Dependency);

                Real32 Value;

                if (Child)
                {
                    Child->SetDirty();

                    Value = Child->Calculate(* this);
                }
                else
                {
                    Value = StatRepository::Instance().Get(Dependency).Calculate(* this, 0.0f, 0.0f, 1.0f);
                }

                mStats.Publish(Dependency, Value);
            });
        }

        /// \brief Retrieves the source arsenal for a given actor entity.
        ///
        /// \param Actor The actor entity to retrieve the arsenal for.
        /// \return A reference to the arsenal of the actor, or this arsenal if the actor is invalid.
        ZYPHRYON_INLINE Ref<Arsenal> GetSource(Scene::Entity Actor)
        {
            return Actor.IsValid() ? Actor.Get<Arsenal>() : (* this);
        }

        /// \brief Applies effect modifiers to the arsenal.
        ///
        /// \param Effect The effect whose modifiers are to be applied.
        void ApplyEffectModifiers(Ref<EffectData> Effect);

        /// \brief Reloads effect modifiers in the arsenal.
        ///
        /// \param Effect The effect whose modifiers are to be reloaded.
        /// \param Slot   The slot index of the modifier to reload.
        /// \param Value  The new value for the modifier.
        void ReloadEffectModifier(Ref<EffectData> Effect, UInt16 Slot, Real32 Value);

        /// \brief Reverts effect modifiers from the arsenal.
        ///
        /// \param Effect The effect whose modifiers are to be reverted.
        void RevertEffectModifiers(ConstRef<EffectData> Effect);

        /// \brief Handles the ticking of a timed effect.
        ///
        /// \param Time   The time interval for the tick.
        /// \param Effect The effect to tick.
        /// \return `true` if the effect should stop ticking, `false` otherwise.
        Bool OnTickEffect(ConstRef<Time> Time, Ref<EffectData> Effect);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Scene::Entity mActor;       // TODO: Investigate to remove from here?
        StatSet       mStats;
        TokenSet      mTokens;
        EffectSet     mEffects;
        AbilitySet    mAbilities;
    };
}