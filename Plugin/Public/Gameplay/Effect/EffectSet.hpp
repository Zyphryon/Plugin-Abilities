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

#include "Gameplay/Effect/EffectInstance.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of effect instances.
    class EffectSet final
    {
    public:

        /// \brief Maximum number of effect instances that can be managed.
        static constexpr UInt32 kMaxInstances = 256;    // TODO: Macro Configurable

        /// \brief Events that can occur within the effect set.
        enum class Event : UInt8
        {
            Insert,     ///< A new effect instance has been inserted.
            Update,     ///< An existing effect instance has been updated.
        };

    public:

        /// \brief Invoke the provided action for each expired effect instance.
        ///
        /// \param Time   The time interval to advance.
        /// \param Action The action to apply to each effect during the tick.
        template<typename Function>
        ZYPHRYON_INLINE void Poll(ConstRef<Time> Time, AnyRef<Function> Action)
        {
            Vector<Ptr<EffectInstance>, 6> Threshold;

            // Iterate over active effects in reverse order to handle expirations.
            for (SInt32 Index = mActives.size() - 1; Index >= 0; --Index)
            {
                Ref<EffectInstance> EffectInstance = mRegistry[mActives[Index].GetID()];

                if (EffectInstance.GetInterval() > Time.GetAbsolute())
                {
                    break;
                }

                if (Action(EffectInstance))
                {
                    // Free the effect instance from the registry.
                    mRegistry.Free(EffectInstance.GetHandle().GetID());

                    // Remove the effect from the active list.
                    mActives.erase(mActives.begin() + Index);
                }
                else
                {
                    if (Threshold.size() != Threshold.capacity())
                    {
                        Threshold.emplace_back(& EffectInstance);
                    }
                }
            }

            // Update intervals for effects that are still active.
            if (Threshold.size() == Threshold.capacity())
            {
                // Re-sort the entire active list if all effects were processed.
                std::sort(mActives.begin(), mActives.end(), [this](Effect First, Effect Second)
                {
                    ConstRef<EffectInstance> FirstEffect  = mRegistry[First.GetID()];
                    ConstRef<EffectInstance> SecondEffect = mRegistry[Second.GetID()];
                    return FirstEffect.GetInterval() >= SecondEffect.GetInterval();
                });
            }
            else
            {
                // Reposition only the effects that were processed.
                for (const Ptr<EffectInstance> EffectInstance : Threshold)
                {
                    // Reposition the effect in the active list if its remaining time has changed.
                    if (const auto Position = FindBestPosition(* EffectInstance); Position != mActives.end())
                    {
                        std::rotate(Position, mActives.end() - 1, mActives.end());
                    }
                }
            }
        }

        /// \brief Creates a new effect instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the effect's properties.
        /// \return A reference to the newly allocated effect instance.
        ZYPHRYON_INLINE Ref<EffectInstance> Create(ConstRef<EffectArchetype> Archetype)
        {
            const UInt32 ID = mRegistry.Allocate(Archetype);

            Ref<EffectInstance> Instance = mRegistry[ID];
            Instance.SetHandle(ID);
            return Instance;
        }

        /// \brief Deletes an existing effect instance, freeing its resources.
        ///
        /// \param Instance The effect instance to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<EffectInstance> Instance)
        {
            LOG_ASSERT(Instance.IsValid(), "Attempting to delete an invalid effect instance.");

            // Free the effect instance from the registry.
            mRegistry.Free(Instance.GetHandle().GetID());
        }

        /// \brief Retrieves an effect instance by its handle.
        ///
        /// \param Handle The handle of the effect instance to fetch.
        /// \return The effect instance associated with the given handle.
        ZYPHRYON_INLINE ConstRef<EffectInstance> GetByHandle(Effect Handle) const
        {
            return mRegistry[Handle.GetID()];
        }

        /// \brief Activates a specific effect instance within the set.
        ///
        /// \param Instance The effect instance to activate.
        /// \param Action   A function to apply to the effect instance upon activation or update.
        template<typename Function>
        ZYPHRYON_INLINE void Activate(Ref<EffectInstance> Instance, AnyRef<Function> Action)
        {
            const ConstPtr<EffectArchetype> Archetype = Instance.GetArchetype();

            // Check if the effect can stack and if an instance already exists.
            if (Archetype->CanStack())
            {
                const auto Iterator = FindByArchetype(Instance.GetArchetype()->GetHandle());

                if (Iterator != mActives.end())
                {
                    Ref<EffectInstance> Inplace = mRegistry[Iterator->GetID()];

                    // EffectInstance is already active, update its properties.
                    Action(Inplace, Event::Update);

                    // Reposition the effect in the active list if its remaining time has changed.
                    if (const auto Position = FindBestPosition(Inplace); Position != Iterator)
                    {
                        std::rotate(Min(Position, Iterator), Iterator, Max(Position, Iterator));
                    }

                    // No need to insert a new instance.
                    Delete(Instance);
                    return;
                }
            }

            // Insert the new effect instance into the active list, maintaining sorted order by remaining time.
            mActives.insert(FindBestPosition(Instance), Instance.GetHandle());

            // Notify that a new non-stackable effect has been added.
            Action(Instance, Event::Insert);
        }

        /// \brief Deactivates a specific effect instance from the set.
        ///
        /// \param Instance The effect instance to deactivate.
        ZYPHRYON_INLINE void Deactivate(ConstRef<EffectInstance> Instance)
        {
            mActives.erase(FindByHandle(Instance.GetHandle()));
        }

        /// \brief Deactivates effect instances that match the given predicate.
        ///
        /// \param Predicate A function that returns `true` for effects to deactivate.
        /// \param Action    A function to apply to each effect before deactivation.
        template<typename Filter, typename Function>
        ZYPHRYON_INLINE void Deactivate(AnyRef<Filter> Predicate, AnyRef<Function> Action)
        {
            for (auto Iterator = mActives.begin(); Iterator != mActives.end();)
            {
                Ref<EffectInstance> EffectInstance = mRegistry[Iterator->GetID()];

                if (Predicate(EffectInstance))
                {
                    Action(EffectInstance);

                    // Free the effect instance from the registry.
                    mRegistry.Free(EffectInstance.GetHandle().GetID());

                    // Remove the effect from the active list.
                    Iterator = mActives.erase(Iterator);
                }
                else
                {
                    ++Iterator;
                }
            }
        }

        /// \brief Clears all effect instances from the set.
        ZYPHRYON_INLINE void Clear()
        {
            // Clear all active effects.
            mActives.clear();

            // Free all effect instances from the registry.
            mRegistry.Clear();
        }

        /// \brief Iterates over all effect instances in the set.
        ///
        /// \param Action The action to apply to each effect instance.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<EffectInstance> Instance : mRegistry.GetSpan())
            {
                if (Instance.IsValid())
                {
                    Action(Instance);
                }
            }
        }

    private:

        /// \brief Finds an active effect instance by its unique handle.
        ///
        /// \param Handle The handle of the effect instance to search for.
        /// \return An iterator to the found effect instance, or the end iterator if not found.
        ZYPHRYON_INLINE Vector<Effect>::iterator FindByHandle(Effect Handle)
        {
            const auto Filter = [Handle](Effect Other)
            {
                return Other == Handle;
            };
            return std::ranges::find_if(mActives, Filter);
        }

        /// \brief Finds an active effect instance by its archetype handle.
        ///
        /// \param Handle The handle of the effect archetype to search for.
        /// \return An iterator to the found effect instance, or the end iterator if not found.
        ZYPHRYON_INLINE Vector<Effect>::iterator FindByArchetype(Effect Handle)
        {
            const auto Filter = [&](Effect Other)
            {
                return mRegistry[Other.GetID()].GetArchetype()->GetHandle() == Handle;
            };
            return std::ranges::find_if(mActives, Filter);
        }

        /// \brief Finds the best position to insert an effect instance in the active list, sorted by remaining time.
        ///
        /// \param Instance The effect instance to find the position for.
        /// \return An iterator to the position where the effect instance should be inserted.
        ZYPHRYON_INLINE Vector<Effect>::iterator FindBestPosition(ConstRef<EffectInstance> Instance)
        {
            const auto Filter = [this](Effect First, Effect Second)
            {
                ConstRef<EffectInstance> A = mRegistry[First.GetID()];
                ConstRef<EffectInstance> B = mRegistry[Second.GetID()];
                return A.GetInterval() >= B.GetInterval();
            };
            return std::ranges::upper_bound(mActives, Instance.GetHandle(), Filter);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Catalog<EffectInstance, kMaxInstances> mRegistry;
        Vector<Effect>                         mActives;
    };
}