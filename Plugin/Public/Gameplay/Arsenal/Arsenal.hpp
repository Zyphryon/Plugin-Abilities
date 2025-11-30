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

#include "Gameplay/Ability/AbilityRepository.hpp"
#include "Gameplay/Ability/AbilitySet.hpp"
#include "Gameplay/Cue/CueRepository.hpp"
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

        /// \brief Constructs an arsenal for the specified actor entity.
        ///
        /// \param Actor The entity that owns this arsenal.
        ZYPHRYON_INLINE Arsenal(Scene::Entity Actor)
            : mActor { Actor }
        {
        }

        /// \brief Advances the state of the arsenal based on the elapsed time.
        ///
        /// \param Time The current time reference.
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

        /// \brief Inserts a token into the arsenal.
        ///
        /// \param Name  The name of the token to insert.
        /// \param Count The number of tokens to insert (default is 1).
        ZYPHRYON_INLINE void InsertToken(ConstStr8 Name, UInt32 Count = 1)
        {
            const Token Token = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to insert unknown token '{}' into arsenal.", Name);

            InsertToken(Token, Count);
        }

        /// \brief Inserts a token into the arsenal.
        ///
        /// \param Handle The handle of the token to insert.
        /// \param Count  The number of tokens to insert (default is 1).
        ZYPHRYON_INLINE void InsertToken(Token Handle, UInt32 Count = 1)
        {
            NotifyDependencies(Handle);

            mTokens.Insert(Handle, Count);
        }

        /// \brief Removes a token from the arsenal.
        ///
        /// \param Name  The name of the token to remove.
        /// \param Count The number of tokens to remove (default is 1).
        ZYPHRYON_INLINE void RemoveToken(ConstStr8 Name, UInt32 Count = 1)
        {
            const Token Token = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to remove an unknown token '{}' from arsenal.", Name);

            RemoveToken(Token, Count);
        }

        /// \brief Removes a token from the arsenal.
        ///
        /// \param Handle The handle of the token to remove.
        /// \param Count  The number of tokens to remove (default is 1).
        ZYPHRYON_INLINE void RemoveToken(Token Handle, UInt32 Count = 1)
        {
            NotifyDependencies(Handle);

            mTokens.Remove(Handle, Count);
        }

        /// \brief Applies an effect modifier to the arsenal.
        ///
        /// \note Bypasses effect stacking rules and directly modifies stats.
        ///
        /// \param Handle    The handle of the stat to modify.
        /// \param Operation The operation to perform on the stat.
        /// \param Magnitude The magnitude of the modification.
        void ApplyModifier(Stat Handle, StatOp Operation, Real32 Magnitude);

        /// \brief Reverts an effect modifier from the arsenal.
        ///
        /// \note Bypasses effect stacking rules and directly modifies stats.
        ///
        /// \param Handle    The handle of the stat to modify.
        /// \param Operation The operation to perform on the stat.
        /// \param Magnitude The magnitude of the modification.
        void RevertModifier(Stat Handle, StatOp Operation, Real32 Magnitude);

        /// \brief Applies an effect to the arsenal.
        ///
        /// \param Specification The specification of the effect to apply.
        /// \param Timestamp     The current timestamp for effect application (default is current elapsed time).
        /// \return The handle of the applied effect.
        ZYPHRYON_INLINE Effect ApplyEffect(ConstRef<EffectSpec> Specification, Real64 Timestamp = Time::Elapsed())
        {
            return ApplyEffect(Scene::Entity(), Specification, Timestamp);
        }

        /// \brief Applies an effect to the arsenal.
        ///
        /// \param Instigator   The entity that is instigating the effect.
        /// \param Specification The specification of the effect to apply.
        /// \param Timestamp     The current timestamp for effect application (default is current elapsed time).
        /// \return The handle of the applied effect.
        Effect ApplyEffect(Scene::Entity Instigator, ConstRef<EffectSpec> Specification, Real64 Timestamp = Time::Elapsed());

        /// \brief Reverts an effect from the arsenal.
        ///
        /// \param Handle The handle of the effect to revert.
        void RevertEffect(Effect Handle);

        /// \brief Retrieves the effective value of a stat from the arsenal.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return The effective value of the stat.
        ZYPHRYON_INLINE Real32 GetStat(Stat Handle) const
        {
            if (const ConstPtr<StatInstance> Instance = mStats.TryGet(Handle); Instance != nullptr)
            {
                return Instance->GetEffective();
            }
            return StatRepository::Instance().Get(Handle).Calculate(* this, 0.0f, 0.0f, 1.0f);
        }

        /// \brief Retrieves the count of a token in the arsenal.
        ///
        /// \param Name The name of the token to query.
        /// \return The count of the specified token.
        ZYPHRYON_INLINE UInt32 GetToken(ConstStr8 Name) const
        {
            const Token Handle = TokenRepository::Instance().GetByName(Name);
            LOG_ASSERT(!Handle.IsEmpty(), "Attempted to query unknown token '{}' in arsenal.", Name);

            return mTokens.Count(Handle);
        }

        /// \brief Retrieves the count of a token in the arsenal.
        ///
        /// \param Handle The handle of the token to query.
        /// \return The count of the specified token.
        ZYPHRYON_INLINE UInt32 GetToken(Token Handle) const
        {
            return mTokens.Count(Handle);
        }

        /// \brief Iterates over all stats in the arsenal.
        ///
        /// \param Action The action to apply to each stat.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachStat(AnyRef<Function> Action)
        {
            mStats.Traverse(Action);
        }

        /// \brief Iterates over all abilities in the arsenal.
        ///
        /// \param Action The action to apply to each ability.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachAbility(AnyRef<Function> Action)
        {
            mAbilities.Traverse(Action);
        }

        /// \brief Iterates over all tokens in the arsenal.
        ///
        /// \param Action The action to apply to each token.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachToken(AnyRef<Function> Action)
        {
            mTokens.Traverse(Action);
        }

        /// \brief Iterates over all effects in the arsenal.
        ///
        /// \param Action The action to apply to each effect.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachEffect(AnyRef<Function> Action)
        {
            mEffects.Traverse(Action);
        }

    private:

        /// \brief Notifies dependent stats of a change in the specified stat or token.
        ///
        /// \param Dependant The stat or token that has changed.
        template<typename Type>
        ZYPHRYON_INLINE void NotifyDependencies(Type Dependant)
        {
            StatRepository::Instance().NotifyDependency(Dependant, [this](Stat Dependency)
            {
                return mStats.Publish(Dependency, GetStat(Dependency));
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

        /// \brief Applies effect modifiers from an effect instance to the arsenal.
        ///
        /// \param Instance The effect instance containing the modifiers to apply.
        void ApplyEffectModifiers(Ref<EffectInstance> Instance);

        /// \brief Reverts effect modifiers from an effect instance in the arsenal.
        ///
        /// \param Instance The effect instance containing the modifiers to revert.
        void RevertEffectModifiers(ConstRef<EffectInstance> Instance);

        /// \brief Updates an active effect instance based on the current timestamp.
        ///
        /// \param Instance  The effect instance to update.
        /// \param Timestamp The current timestamp for effect updating.
        /// \return `true` if the effect is still active, `false` if it has expired.
        Bool UpdateEffect(Ref<EffectInstance> Instance, Real64 Timestamp);

        /// \brief Runs cues from a cue sheet for a specific event.
        ///
        /// \param Sheet     The cue sheet containing cues to run.
        /// \param Event     The cue event type to trigger.
        /// \param Timestamp The current timestamp for cue triggering.
        /// \param Source    The source entity for the cue event.
        /// \param Magnitude The magnitude associated with the cue event.
        ZYPHRYON_INLINE void RunCues(ConstRef<CueSheet> Sheet, CueData::Event Event, Real64 Timestamp, UInt64 Source, Real32 Magnitude) const
        {
            Ref<CueRepository> Repository = CueRepository::Instance();

            for (const Token Cue : Sheet.GetChildren())
            {
                Repository.Publish(CueData(Cue, Event, Timestamp, Source, mActor.GetID(), Magnitude));
            }
        }

        /// \brief Runs cues associated with an effect instance for a specific event.
        ///
        /// \param Instance  The effect instance to run cues for.
        /// \param Event     The cue event type to trigger.
        /// \param Timestamp The current timestamp for cue triggering.
        ZYPHRYON_INLINE void RunCues(ConstRef<EffectInstance> Instance, CueData::Event Event, Real64 Timestamp) const
        {
            RunCues(Instance.GetArchetype()->GetCues(), Event, Timestamp, Instance.GetInstigator(), Instance.GetEffectiveIntensity());
        }

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