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

#include "Gameplay/Arsenal/Arsenal.hpp"
#include "Gameplay/Arsenal/Coordinator.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::Tick(ConstRef<Time> Time)
    {
        // Tick effects.
        mEffects.Tick(Time, [&](Ref<EffectData> Effect)
        {
            return OnTickEffect(Time, Effect);
        });

        // Poll token changes.
        mTokens.Poll([this](Token Handle, UInt32 Previous, UInt32 Current)
        {
            Coordinator::Instance().Publish(Handle, mActor, Previous, Current);
        });

        // Poll stat changes.
        mStats.Poll([this](Stat Handle, Real32 Previous)
        {
            Real32 Current;

            if (const Ptr<StatData> Stat = const_cast<Ptr<StatData>>(mStats.TryGet(Handle)); Stat)
            {
                Current = Stat->Resolve(* this);
            }
            else
            {
                Current = StatRepository::Instance().Get(Handle).Calculate(* this, 0.0f, 0.0f, 1.0f);
            }

            if (Current != Previous)
            {
                Coordinator::Instance().Publish(Handle, mActor, Previous, Current);
            }
        });
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyModifier(Stat Archetype, StatModifier Operator, Real32 Magnitude)
    {
        // Ensure the stat is valid.
        Ref<StatData> Stat = mStats.GetOrInsert(* this, StatRepository::Instance().Get(Archetype));

        // Notify dependant stats if the stat is being marked as dirty for the first time.
        if (mStats.Publish(Archetype, Stat.GetEffective()))
        {
            Notify(Archetype);
        }

        // Apply the modifier to the stat.
        Stat.Apply(* this, Operator, Magnitude);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertModifier(Stat Archetype, StatModifier Operator, Real32 Magnitude)
    {
        // Ensure the stat is valid.
        Ref<StatData> Stat = mStats.GetOrInsert(* this, StatRepository::Instance().Get(Archetype));

        // Notify dependant stats if the stat is being marked as dirty for the first time.
        if (mStats.Publish(Archetype, Stat.GetEffective()))
        {
            Notify(Archetype);
        }

        // Revert the modifier from the stat.
        Stat.Revert(* this, Operator, Magnitude);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Effect Arsenal::ApplyEffect(Scene::Entity Instigator, ConstRef<EffectSpec> Specification, Real64 Timestamp)
    {
        Ref<Arsenal>              Source = GetSource(Instigator);
        ConstRef<EffectArchetype> Effect = EffectRepository::Instance().Get(Specification.GetTarget());

        Gameplay::Effect Result;

        switch (Effect.GetApplication())
        {
        case EffectApplication::Instant:
        {
            for (ConstRef<EffectModifier> Bonus : Effect.GetBonuses())
            {
                ApplyModifier(Bonus.GetTarget(), Bonus.GetModifier(), Bonus.GetMagnitude().Resolve(Source, * this));
            }
            break;
        }
        case EffectApplication::Temporary:
        case EffectApplication::Permanent:
        {
            // Create a new effect instance.
            Ref<EffectData> Instance = mEffects.Create(Effect);
            Instance.SetStack(Specification.GetStack().Resolve(* this));
            Instance.SetIntensity(Specification.GetIntensity().Resolve(* this));
            Instance.SetInstigator(Instigator.GetID());

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
                const auto OnStackEffect = [&](Ref<EffectData> Inplace, EffectSet::Event Event)
                {
                    Result = Inplace.GetHandle();

                    switch (Event)
                    {
                    case EffectSet::Event::Insert:
                        // Merge the new effect instance into the existing one.
                        ApplyEffectModifiers(Inplace);

                        // Insert live observation for the effect.
                        if (!Instance.CanTick())
                        {
                            //InsertLiveObservation(Instance);
                        }
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
                if (!Instance.CanTick())
                {
                    //InsertLiveObservation(Instance);
                }
            }
            return Result;
        }
        }

        return Result;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffect(Effect Handle)
    {
        ConstRef<EffectData> Effect = mEffects.GetInstance(Handle);

        // Stop the effect from ticking if it can expire.
        if (Effect.CanExpire())
        {
            mEffects.Deactivate(Effect);
        }

        // Revert the effect modifiers.
        RevertEffectModifiers(Effect);

        // Remove live observation for the effect.
        if (!Effect.CanTick())
        {
            //RemoveLiveObservation(Effect);
        }

        // Remove the effect from the arsenal.
        mEffects.Delete(Effect);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyEffectModifiers(Ref<EffectData> Effect)
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
            ApplyModifier(Modifier.GetTarget(), Modifier.GetModifier(), Value);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ReloadEffectModifier(Ref<EffectData> Effect, UInt16 Slot, Real32 Value)
    {
        // Revert the previous modifier value.
        ConstRef<EffectModifier> Modifier = Effect.GetArchetype()->GetBonus(Slot);
        RevertModifier(Modifier.GetTarget(), Modifier.GetModifier(), Effect.GetSnapshot(Slot));

        // Calculate the new modifier value.
        const Real32 Effective = Value * Effect.GetEffectiveIntensity();
        Effect.SetSnapshot(Slot, Effective);

        // Apply the updated modifier value.
        ApplyModifier(Modifier.GetTarget(), Modifier.GetModifier(), Effective);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffectModifiers(ConstRef<EffectData> Effect)
    {
        // Iterate over each modifier in the effect and revert it.
        for (const auto [Index, Modifier] : std::views::enumerate(Effect.GetArchetype()->GetBonuses()))
        {
            RevertModifier(Modifier.GetTarget(), Modifier.GetModifier(), Effect.GetSnapshot(Index));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Arsenal::OnTickEffect(ConstRef<Time> Time, Ref<EffectData> Effect)
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
                if (!Effect.CanTick())
                {
                    //RemoveLiveObservation(Effect);
                }
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

}