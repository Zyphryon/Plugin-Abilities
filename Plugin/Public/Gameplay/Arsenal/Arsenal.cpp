// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2025 by Agustin L. Alvarez. All rights reserved.
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
        // Poll all effects and update their state based on the current time.
        mEffects.Poll(Time, [&](Ref<EffectData> Instance)
        {
            return UpdateEffect(Instance, Time.GetAbsolute());
        });

        // Poll all tokens and notify the coordinator of any changes.
        mTokens.Poll([this](Token Handle, UInt32 Previous, UInt32 Current)
        {
            Coordinator::Instance().Publish(Handle, mActor, Previous, Current);
        });

        // Poll all stats and notify the coordinator of any changes.
        mStats.Poll(* this, [this](Stat Handle, Real32 Previous, Real32 Current)
        {
            Coordinator::Instance().Publish(Handle, mActor, Previous, Current);
        });
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyModifier(Stat Handle, StatModifier Modifier, Real32 Magnitude)
    {
        Ref<StatData> Instance = mStats.GetOrInsert(* this, StatRepository::Instance().Get(Handle));

        // Notify dependencies only if the stat was successfully published.
        if (mStats.Publish(Handle, Instance.GetEffective()))
        {
            NotifyDependencies(Handle);
        }

        // Apply the modifier to the stat instance.
        Instance.Apply(* this, Modifier, Magnitude);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertModifier(Stat Handle, StatModifier Modifier, Real32 Magnitude)
    {
        Ref<StatData> Instance = mStats.GetOrInsert(* this, StatRepository::Instance().Get(Handle));

        // Notify dependencies only if the stat was successfully published.
        if (mStats.Publish(Handle, Instance.GetEffective()))
        {
            NotifyDependencies(Handle);
        }

        // Revert the modifier from the stat instance.
        Instance.Revert(* this, Modifier, Magnitude);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Effect Arsenal::ApplyEffect(Scene::Entity Instigator, ConstRef<EffectSpec> Specification, Real64 Timestamp)
    {
        ConstRef<EffectArchetype> Archetype = EffectRepository::Instance().Get(Specification.GetTarget());
        Ref<Arsenal>              Source    = GetSource(Instigator);

        Gameplay::Effect Result;

        switch (Archetype.GetApplication())
        {
        case EffectApplication::Instant:
        {
            for (ConstRef<EffectModifier> Bonus : Archetype.GetBonuses())
            {
                ApplyModifier(Bonus.GetTarget(), Bonus.GetModifier(), Bonus.GetMagnitude().Resolve(Source, * this));
            }
            break;
        }
        case EffectApplication::Temporary:
        case EffectApplication::Permanent:
        {
            // Create a new effect instance.
            Ref<EffectData> Instance = mEffects.Create(Archetype);
            Instance.SetStack(Specification.GetStack().Resolve(* this));
            Instance.SetIntensity(Specification.GetIntensity().Resolve(* this));
            Instance.SetInstigator(Instigator.GetID());

            // Set the expiration based on the effect application type.
            if (Archetype.GetApplication() == EffectApplication::Temporary)
            {
                Instance.SetDuration(Instance.GetArchetype()->GetDuration().Resolve(Source, * this));
                Instance.SetExpiration(Instance.GetDuration() + Timestamp);
            }
            else
            {
                Instance.SetExpiration(kInfinity<Real32>);
            }

            // Set the period and interval for the effect.
            Instance.SetPeriod(Instance.GetArchetype()->GetPeriod().Resolve(Source, * this));

            if (const Real32 Period = Instance.GetPeriod(); Period > 0.0f)
            {
                Instance.SetInterval(Period + Timestamp);
            }
            else
            {
                Instance.SetInterval(Instance.GetExpiration());
            }

            // Handle effect stacking behavior.
            const auto OnHandleEffectStack = [&](Ref<EffectData> Inplace, EffectSet::Event Event)
            {
                Result = Inplace.GetHandle();

                switch (Event)
                {
                case EffectSet::Event::Insert:
                    ApplyEffectModifiers(Inplace);
                    break;
                case EffectSet::Event::Update:
                    RevertEffectModifiers(Inplace);

                    // Merge the new instance into the existing one.
                    Inplace.Merge(Instance);

                    ApplyEffectModifiers(Inplace);
                    break;
                }

            };
            mEffects.Activate(Instance, OnHandleEffectStack);
            break;
        }
        }
        return Result;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffect(Effect Handle)
    {
        ConstRef<EffectData> Instance = mEffects.GetByHandle(Handle);

        // Revert all modifiers applied by the effect.
        RevertEffectModifiers(Instance);

        // Deactivate the effect if it is currently active.
        if (Instance.CanExpire())
        {
            mEffects.Deactivate(Instance);
        }

        // Free the effect instance from the registry.
        mEffects.Delete(Instance);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::ApplyEffectModifiers(Ref<EffectData> Instance)
    {
        ConstRef<Arsenal> Instigator = GetSource(Instance.GetInstigator());

        // Calculate the effective intensity of the effect.
        const Real32 Intensity = Instance.GetEffectiveIntensity();

        // Iterate over each modifier in the effect and apply it.
        for (const auto [Index, Modifier] : std::views::enumerate(Instance.GetArchetype()->GetBonuses()))
        {
            // Resolve the modifier's value based on its magnitude and the effect's intensity.
            const Real32 Value = Modifier.GetMagnitude().Resolve(Instigator, * this) * Intensity;
            Instance.SetSnapshot(Index, Value);

            // Apply the modifier to the arsenal.
            ApplyModifier(Modifier.GetTarget(), Modifier.GetModifier(), Value);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Arsenal::RevertEffectModifiers(ConstRef<EffectData> Instance)
    {
        for (const auto [Index, Modifier] : std::views::enumerate(Instance.GetArchetype()->GetBonuses()))
        {
            RevertModifier(Modifier.GetTarget(), Modifier.GetModifier(), Instance.GetSnapshot(Index));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Arsenal::UpdateEffect(Ref<EffectData> Instance, Real64 Timestamp)
    {
        const ConstPtr<EffectArchetype> Archetype = Instance.GetArchetype();

        if (Instance.GetInterval() >= Instance.GetExpiration())
        {
            // Revert the effect's modifiers before any changes.
            RevertEffectModifiers(Instance);

            // Handle expiration based on the archetype's policy.
            switch (Archetype->GetExpiration())
            {
            case EffectExpiration::Single:
                Instance.SetStack(Instance.GetStack() - 1);
                break;
            case EffectExpiration::All:
                Instance.SetStack(0);
                break;
            case EffectExpiration::Tick:
                break;
            }

            // Re-apply the effect's modifiers to ensure correct intensity.
            if (Instance.GetStack() > 0)
            {
                ApplyEffectModifiers(Instance);

                // Refresh duration.
                Instance.SetExpiration(Instance.GetDuration() + Timestamp);

                // Schedule next tick if applicable.
                if (Instance.CanTick())
                {
                    Instance.SetInterval(Instance.GetPeriod());
                }
                else
                {
                    Instance.SetInterval(Instance.GetExpiration());
                }
            }
            else
            {
                return true; // Effect has expired.
            }
        }
        else
        {
            // Re-apply the effect's modifiers on tick.
            ApplyEffectModifiers(Instance);

            // Schedule the next tick.
            Instance.SetInterval(Instance.GetPeriod() + Timestamp);

            // Decrease stack for tick-based expiration.
            if (Archetype->GetExpiration() == EffectExpiration::Tick)
            {
                Instance.SetStack(Instance.GetStack() - 1);
            }
        }
        return false;
    }
}