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

#include "Arsenal.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::Tick(ConstRef<Time> Time)
    {
        // Tick timed effects.
        mEffects.Tick(Time, [&](Ref<Effect> Effect)
        {
            return OnTickEffect(Time, Effect);
        });

        // Poll stats for changes.
        mStats.Poll();
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyModifier(StatHandle Archetype, StatOperator Operator, Real32 Magnitude)
    {
        // Ensure the stat is valid.
        Ref<Stat> Stat = mStats.GetOrInsert(StatRepository::Instance().Get(Archetype));

        // Ensure the stat has an effective value.
        if (Stat.IsUndefined())
        {
            Stat.SetEffective(* this, Stat.GetOutcome(* this));
        }

        // Apply the modifier to the stat.
        Stat.Apply(* this, Operator, Magnitude);

        // Notify dependencies of the stat change.
        mStats.NotifyDependencies(Archetype);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertModifier(StatHandle Archetype, StatOperator Operator, Real32 Magnitude)
    {
        // Ensure the stat is valid.
        Ref<Stat> Stat = mStats.GetOrInsert(StatRepository::Instance().Get(Archetype));

        // Revert the modifier from the stat.
        Stat.Revert(* this, Operator, Magnitude);

        // Notify dependencies of the stat change.
        mStats.NotifyDependencies(Archetype);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    EffectHandle Arsenal::ApplyEffect(Scene::Entity Instigator, EffectHandle Archetype, Real32 Intensity, UInt16 Stack, Real64 Timestamp)
    {
        Ref<Arsenal>              Source = GetSource(Instigator);
        ConstRef<EffectArchetype> Effect = EffectRepository::Instance().Get(Archetype);

        EffectHandle Result;

        switch (Effect.GetApplication())
        {
        case EffectApplication::Instant:
        {
            for (ConstRef<EffectModifier> Bonus : Effect.GetBonuses())
            {
                ApplyModifier(Bonus.GetTarget(), Bonus.GetOperator(), Bonus.GetMagnitude().Resolve(Source, * this));
            }
            break;
        }
        case EffectApplication::Temporary:
        case EffectApplication::Permanent:
        {
            // Create a new effect instance.
            Ref<Gameplay::Effect> Instance = mEffects.Create(Effect);
            Instance.SetStack(Stack);
            Instance.SetIntensity(Intensity);
            Instance.SetInstigator(Instigator);

            if (Effect.GetApplication() == EffectApplication::Temporary)
            {
                Instance.SetDuration(Instance.GetArchetype()->GetDuration().Resolve(Source, * this));
                Instance.SetExpiration(Timestamp + Instance.GetDuration());
            }
            else
            {
                Instance.SetExpiration(kInfinity<Real32>);
            }

            Instance.SetPeriod(Instance.GetArchetype()->GetPeriod().Resolve(Source, * this));

            if (const Real32 Period = Instance.GetPeriod(); Period > 0.0f)
            {
                Instance.SetInterval(Timestamp + Period);
            }
            else
            {
                Instance.SetInterval(Instance.GetExpiration());
            }

            // Handle stacking effects.
            if (Effect.CanStack())
            {
                const auto OnStackEffect = [&](Ref<Gameplay::Effect> Inplace, EffectSet::Event Event)
                {
                    Result = Inplace.GetHandle();

                    switch (Event)
                    {
                    case EffectSet::Event::Insert:
                        // Merge the new effect instance into the existing one.
                        ApplyEffectModifiers(Inplace);

                        // Insert live observation for the effect.
                        InsertLiveObservation(Instance);
                        break;
                    case EffectSet::Event::Update:
                        // Revert the previous effect modifiers.
                        RevertEffectModifiers(Inplace);

                        // Merge the new effect instance into the existing one.
                        Inplace.Merge(Instance);

                        // Delete the temporary instance.
                        mEffects.Delete(Instance);

                        // Apply the updated effect modifiers.
                        ApplyEffectModifiers(Inplace);
                        break;
                    }
                };
                mEffects.Activate(Instance, OnStackEffect);
            }
            else
            {
                Result = Instance.GetHandle();

                // Activate the effect if it can tick or expire.
                if (Instance.CanTick() || Instance.CanExpire())
                {
                    mEffects.Activate(Instance);
                }

                // Apply the effect modifiers.
                ApplyEffectModifiers(Instance);

                // Insert live observation for the effect.
                InsertLiveObservation(Instance);
            }
            return Result;
        }
        }

        return Result;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffect(EffectHandle Handle)
    {
        ConstRef<Effect> Effect = mEffects.Fetch(Handle);

        // Stop the effect from ticking if it can expire.
        if (Effect.CanExpire())
        {
            mEffects.Deactivate(Effect);
        }

        // Revert the effect modifiers.
        RevertEffectModifiers(Effect);

        // Remove live observation for the effect.
        RemoveLiveObservation(Effect);

        // Remove the effect from the arsenal.
        mEffects.Delete(Effect);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyEffectModifiers(Ref<Effect> Effect)
    {
        ConstRef<Arsenal> Instigator = GetSource(Effect.GetInstigator());

        // Calculate the effective intensity of the effect.
        const Real32 Intensity = Effect.GetEffectiveIntensity();

        // Iterate over each modifier in the effect and apply it.
        for (const auto [Index, Modifier] : std::views::enumerate(Effect.GetArchetype()->GetBonuses()))
        {
            // Calculate the value of the modifier based on its evaluation method.
            const Real32 Value = Modifier.GetMagnitude().Resolve(Instigator, * this) * Intensity;
            Effect.SetSnapshot(Index, Value);

            // Apply the modifier to the arsenal.
            ApplyModifier(Modifier.GetTarget(), Modifier.GetOperator(), Value);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ReloadEffectModifier(Ref<Effect> Effect, UInt16 Slot)
    {
        ConstRef<Arsenal> Instigator = GetSource(Effect.GetInstigator());

        // Revert the previous modifier value.
        ConstRef<EffectModifier> Modifier = Effect.GetArchetype()->GetBonus(Slot);
        RevertModifier(Modifier.GetTarget(), Modifier.GetOperator(), Effect.GetSnapshot(Slot));

        // Calculate the new modifier value.
        const Real32 Value = Modifier.GetMagnitude().Resolve(Instigator, * this) * Effect.GetEffectiveIntensity();
        Effect.SetSnapshot(Slot, Value);

        // Apply the updated modifier value.
        ApplyModifier(Modifier.GetTarget(), Modifier.GetOperator(), Value);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffectModifiers(ConstRef<Effect> Effect)
    {
        // Iterate over each modifier in the effect and revert it.
        for (const auto [Index, Modifier] : std::views::enumerate(Effect.GetArchetype()->GetBonuses()))
        {
            RevertModifier(Modifier.GetTarget(), Modifier.GetOperator(), Effect.GetSnapshot(Index));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Arsenal::OnTickEffect(ConstRef<Time> Time, Ref<Effect> Effect)
    {
        ConstPtr<EffectArchetype> Archetype = Effect.GetArchetype();

        if (Effect.GetInterval() >= Effect.GetExpiration())
        {
            // Revert the effect's modifiers before any changes.
            RevertEffectModifiers(Effect);

            // Handle expiration based on the archetype's policy.
            switch (Archetype->GetExpiration())
            {
            case EffectExpiration::Single:
                Effect.SetStack(Effect.GetStack() - 1);
                break;
            case EffectExpiration::All:
                Effect.SetStack(0);
                break;
            case EffectExpiration::Tick:
                break;
            }

            // Re-apply the effect's modifiers to ensure correct intensity.
            if (Effect.GetStack() > 0)
            {
                ApplyEffectModifiers(Effect);

                // Refresh the effect's duration.
                Effect.SetExpiration(Time.GetAbsolute() + Effect.GetDuration());

                if (Effect.CanTick())
                {
                    Effect.SetInterval(Time.GetAbsolute() + Effect.GetPeriod());
                }
                else
                {
                    Effect.SetInterval(Effect.GetExpiration());
                }
            }
            else
            {
                // Remove live observation for the effect.
                RemoveLiveObservation(Effect);
                return true;
            }
        }
        else
        {
            // Re-apply the effect's modifiers on tick.
            ApplyEffectModifiers(Effect);

            // Schedule the next tick.
            Effect.SetInterval(Time.GetAbsolute() + Effect.GetPeriod());

            // Decrease stack for tick-based expiration.
            if (Archetype->GetExpiration() == EffectExpiration::Tick)
            {
                Effect.SetStack(Effect.GetStack() - 1);
            }
        }
        return false;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::InsertLiveObservation(ConstRef<Effect> Effect)
    {
        for (const auto [Index, Modifier] : std::views::enumerate(Effect.GetArchetype()->GetBonuses()))
        {
            if (Modifier.GetEvaluation() == StatEvaluation::Live)
            {
                const StatHandle Target = Modifier.GetTarget();

                Ref<Set<EffectLiveObservation>> Observers = mObservations[Target];

                if (Observers.empty())
                {
                    mStats.InsertObserver(
                        Target, StatSet::OnChangeDelegate::Create<& Arsenal::UpdateLiveObservation>(this));
                }
                Observers.emplace(Effect.GetHandle(), static_cast<UInt16>(Index));
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RemoveLiveObservation(ConstRef<Effect> Effect)
    {
        for (const auto [Index, Modifier] : std::views::enumerate(Effect.GetArchetype()->GetBonuses()))
        {
            if (Modifier.GetEvaluation() == StatEvaluation::Live)
            {
                const StatHandle Target = Modifier.GetTarget();

                Ref<Set<EffectLiveObservation>> Observers = mObservations[Target];

                const auto Iterator = Observers.find(EffectLiveObservation(Effect.GetHandle(), Index));
                if (Iterator != Observers.end())
                {
                    Observers.erase(Iterator);

                    if (Observers.empty())
                    {
                        mStats.RemoveObserver(
                            Target, StatSet::OnChangeDelegate::Create<& Arsenal::UpdateLiveObservation>(this));
                    }
                }
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::UpdateLiveObservation(StatHandle Handle)
    {
        if (const auto Iterator = mObservations.find(Handle); Iterator != mObservations.end())
        {
            for (const auto [ID, Slot] : Iterator->second)
            {
                Ref<Effect> Instance = const_cast<Ref<Effect>>(mEffects.Fetch(ID));
                ReloadEffectModifier(Instance, Slot);
            }
        }
    }
}