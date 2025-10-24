// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2025 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Gameplay/Effect/EffectRepository.hpp"
#include "Gameplay/Effect/EffectSet.hpp"
#include "Gameplay/Marker/MarkerRepository.hpp"
#include "Gameplay/Marker/MarkerSet.hpp"
#include "Gameplay/Stat/StatRepository.hpp"
#include "Gameplay/Stat/StatSet.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Encapsulates a collection of markers, stats, effects, and abilities for an entity.
    class Arsenal final
    {
        // TODO: Concurrency and Live Modifiers.

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

        /// \brief Inserts a marker into the arsenal by its name.
        ///
        /// \param Name  The name of the marker to insert.
        /// \param Count The number of markers to insert.
        ZYPHRYON_INLINE void InsertMarker(ConstStr8 Name, UInt32 Count = 1)
        {
            const Marker Token = MarkerRepository::Instance().GetTokenByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to insert unknown marker '{}' into arsenal.", Name);

            mMarkers.Insert(Token, Count);
        }

        /// \brief Inserts a marker into the arsenal.
        ///
        /// \param Token The marker token to insert.
        /// \param Count The number of markers to insert.
        ZYPHRYON_INLINE void InsertMarker(Marker Token, UInt32 Count = 1)
        {
            mMarkers.Insert(Token, Count);
        }

        /// \brief Removes a marker from the arsenal by its name.
        ///
        /// \param Name  The name of the marker to remove.
        /// \param Count The number of markers to insert.
        ZYPHRYON_INLINE void RemoveMarker(ConstStr8 Name, UInt32 Count = 1)
        {
            const Marker Token = MarkerRepository::Instance().GetTokenByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to remove an unknown marker '{}' from arsenal.", Name);

            mMarkers.Remove(Token, Count);
        }

        /// \brief Removes a marker from the arsenal.
        ///
        /// \param Token The marker token to remove.
        /// \param Count The number of markers to remove.
        ZYPHRYON_INLINE void RemoveMarker(Marker Token, UInt32 Count = 1)
        {
            mMarkers.Remove(Token, Count);
        }

        /// \brief Applies a modifier to the arsenal with a specified magnitude.
        ///
        /// \note Bypasses effect systems and directly modifies stats.
        ///
        /// \param Archetype The archetype of the stat to modify.
        /// \param Operator  The operator to apply.
        /// \param Magnitude The magnitude to apply with the modifier.
        void ApplyModifier(StatHandle Archetype, StatOperator Operator, Real32 Magnitude);

        /// \brief Reverts a modifier from the arsenal with a specified magnitude.
        ///
        /// \note Bypasses effect systems and directly modifies stats.
        ///
        /// \param Archetype The archetype of the stat to modify.
        /// \param Operator  The operator to revert.
        /// \param Magnitude The magnitude to revert with the modifier.
        void RevertModifier(StatHandle Archetype, StatOperator Operator, Real32 Magnitude);

        /// \brief Applies an effect to the arsenal using the current entity as the instigator.
        ///
        /// \param Archetype The archetype of the effect to apply.
        /// \param Intensity The intensity of the effect.
        /// \param Stack     The stack count of the effect.
        /// \param Timestamp The timestamp of the effect application.
        /// \return A handle to the applied effect.
        ZYPHRYON_INLINE EffectHandle ApplyEffect(EffectHandle Archetype, Real32 Intensity = 1.0f, UInt16 Stack = 1, Real64 Timestamp = Time::Elapsed())
        {
            return ApplyEffect(Scene::Entity(), Archetype, Intensity, Stack, Timestamp);
        }

        /// \brief Applies an effect to the arsenal.
        ///
        /// \param Instigator The entity that is applying the effect.
        /// \param Archetype  The archetype of the effect to apply.
        /// \param Intensity  The intensity of the effect.
        /// \param Stack      The stack count of the effect.
        /// \param Timestamp  The timestamp of the effect application.
        /// \return A handle to the applied effect.
        EffectHandle ApplyEffect(Scene::Entity Instigator, EffectHandle Archetype, Real32 Intensity = 1.0f, UInt16 Stack = 1, Real64 Timestamp = Time::Elapsed());

        /// \brief Reverts an effect applied to the arsenal.
        ///
        /// \param Handle The handle of the effect to revert.
        void RevertEffect(EffectHandle Handle);

        /// \brief Retrieves the effective value of a stat by its handle.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return The effective value of the stat.
        ZYPHRYON_INLINE Real32 GetStat(StatHandle Handle) const
        {
            ConstRef<StatArchetype> Archetype = StatRepository::Instance().Get(Handle);

            if (const ConstPtr<Stat> Stat = mStats.TryGet(Handle); Stat)
            {
                return Stat->Calculate(* this);
            }
            return Archetype.Calculate(* this, 0.0f, 0.0f, 1.0f);
        }

        /// \brief Retrieves the count of a specific marker in the arsenal by its name.
        ///
        /// \param Name The name of the marker to query.
        /// \return The count of the specified marker.
        ZYPHRYON_INLINE UInt32 GetMarker(ConstStr8 Name) const
        {
            const Marker Token = MarkerRepository::Instance().GetTokenByName(Name);
            LOG_ASSERT(!Token.IsEmpty(), "Attempted to query unknown marker '{}' in arsenal.", Name);

            return mMarkers.Count(Token);
        }

        /// \brief Retrieves the count of a specific marker in the arsenal.
        ///
        /// \param Token The marker token to query.
        /// \return The count of the specified marker.
        ZYPHRYON_INLINE UInt32 GetMarker(Marker Token) const
        {
            return mMarkers.Count(Token);
        }

        /// \brief Checks if a specific marker exists in the arsenal by its name.
        ///
        /// \param Name The name of the marker to check.
        /// \return `true` if the marker exists, `false` otherwise.
        ZYPHRYON_INLINE Bool HasMarker(ConstStr8 Name) const
        {
            return GetMarker(Name) > 0;
        }

        /// \brief Checks if a specific marker exists in the arsenal.
        ///
        /// \param Token The marker token to check.
        /// \return `true` if the marker exists, `false` otherwise.
        ZYPHRYON_INLINE Bool HasMarker(Marker Token) const
        {
            return GetMarker(Token) > 0;
        }

        /// \brief Traverses each marker in the marker set and applies the given action.
        ///
        /// \param Action The action to apply to each marker.
        template<typename Function>
        ZYPHRYON_INLINE void ForEachMarker(AnyRef<Function> Action)
        {
            mMarkers.Traverse(Action);
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
        void ApplyEffectModifiers(Ref<Effect> Effect);

        /// \brief Reloads effect modifiers in the arsenal.
        ///
        /// \param Effect The effect whose modifiers are to be reloaded.
        /// \param Slot   The slot index of the modifier to reload.
        void ReloadEffectModifier(Ref<Effect> Effect, UInt16 Slot, Real32 Value);

        /// \brief Reverts effect modifiers from the arsenal.
        ///
        /// \param Effect The effect whose modifiers are to be reverted.
        void RevertEffectModifiers(ConstRef<Effect> Effect);

        /// \brief Handles the ticking of a timed effect.
        ///
        /// \param Time   The time interval for the tick.
        /// \param Effect The effect to tick.
        /// \return `true` if the effect should stop ticking, `false` otherwise.
        Bool OnTickEffect(ConstRef<Time> Time, Ref<Effect> Effect);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Scene::Entity mActor;
        MarkerSet     mMarkers;
        StatSet       mStats;
        EffectSet     mEffects;
    };
}