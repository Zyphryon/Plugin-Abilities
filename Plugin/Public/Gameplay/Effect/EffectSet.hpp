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

#include "Effect.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of effect instances and their dependencies.
    class EffectSet final
    {
    public:

        /// \brief Maximum number of effect instances that can be managed.
        static constexpr UInt32 kMaxInstances = 256;    // TODO: Macro Configurable

        /// \brief Events that can occur within the effect set.
        enum class Event
        {
            Insert,     ///< A new effect instance has been inserted.
            Update,     ///< An existing effect instance has been updated.
        };

    public:

        /// \brief Advances the state of all active effects by the given time interval.
        ///
        /// \param Time   The time interval to advance.
        /// \param Action The action to apply to each effect during the tick.
        template<typename Function>
        ZYPHRYON_INLINE void Tick(ConstRef<Time> Time, AnyRef<Function> Action)
        {
            while (!mActives.empty())
            {
                Ref<Effect> Effect = mRegistry[mActives.back().GetID()];

                if (Effect.GetInterval() > Time.GetAbsolute())
                {
                    break;
                }

                if (Action(Effect))
                {
                    // Free the effect instance from the registry.
                    mRegistry.Free(Effect.GetHandle().GetID());

                    // Remove the effect from the active list.
                    mActives.pop_back();
                }
                else
                {
                    if (mActives.size() > 1)
                    {
                        // Reposition the effect in the active list if its remaining time has changed.
                        if (const auto Position = FindBestPosition(Effect); Position != mActives.end() - 1)
                        {
                            std::rotate(Position, mActives.end() - 1, mActives.end());
                        }
                    }
                }
            }
        }

        /// \brief Creates a new effect instance based on the provided archetype.
        ///
        /// \param Archetype The archetype defining the effect's properties.
        /// \return A reference to the newly allocated effect instance.
        ZYPHRYON_INLINE Ref<Effect> Create(ConstRef<EffectArchetype> Archetype)
        {
            const UInt32 ID = mRegistry.Allocate(Archetype);

            Ref<Effect> Instance = mRegistry[ID];
            Instance.SetHandle(ID);
            return Instance;
        }

        /// \brief Deletes an existing effect instance, freeing its resources.
        ///
        /// \param Instance The effect instance to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<Effect> Instance)
        {
            LOG_ASSERT(Instance.IsValid(), "Attempting to delete an invalid effect instance.");

            // Free the effect instance from the registry.
            mRegistry.Free(Instance.GetHandle().GetID());
        }

        /// \brief Fetches an effect instance by its handle.
        ///
        /// \param Handle The handle of the effect instance to fetch.
        /// \return The effect instance associated with the given handle.
        ZYPHRYON_INLINE ConstRef<Effect> Fetch(EffectHandle Handle) const
        {
            return mRegistry[Handle.GetID()];
        }

        /// \brief Activates or updates a timed effect instance in the set.
        ///
        /// \param Instance The effect instance to activate.
        /// \param Action   The action to apply when the effect is inserted or updated.
        template<typename Function>
        ZYPHRYON_INLINE void Activate(Ref<Effect> Instance, AnyRef<Function> Action)
        {
            if (const auto Iterator = FindByArchetype(Instance.GetArchetype()->GetHandle()); Iterator != mActives.end())
            {
                Ref<Effect> Effect = mRegistry[Iterator->GetID()];

                // Effect is already active, update its properties.
                Action(Effect, Event::Update);

                // Reposition the effect in the active list if its remaining time has changed.
                if (const auto Position = FindBestPosition(Effect); Position != Iterator)
                {
                    std::rotate(Min(Position, Iterator), Iterator, Max(Position, Iterator));
                }
            }
            else
            {
                // Insert the new effect instance into the active list, maintaining sorted order by remaining time.
                mActives.insert(FindBestPosition(Instance), Instance.GetHandle());

                // Notify that a new non-stackable effect has been added.
                Action(Instance, Event::Insert);
            }
        }

        /// \brief Deactivates a specific effect instance from the set.
        ///
        /// \param Instance The effect instance to deactivate.
        ZYPHRYON_INLINE void Deactivate(ConstRef<Effect> Instance)
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
                Ref<Effect> Effect = mRegistry[Iterator->GetID()];

                if (Predicate(Effect))
                {
                    Action(Effect);

                    // Free the effect instance from the registry.
                    mRegistry.Free(Effect.GetHandle().GetID());

                    // Remove the effect from the active list.
                    Iterator = mActives.erase(Iterator);
                }
                else
                {
                    ++Iterator;
                }
            }
        }

        /// \brief Clears all effect instances and their dependencies from the set.
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
            for (ConstRef<Effect> Instance : mRegistry.GetSpan())
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
        ZYPHRYON_INLINE Vector<EffectHandle>::iterator FindByHandle(EffectHandle Handle)
        {
            const auto Filter = [Handle](EffectHandle Other)
            {
                return Other == Handle;
            };
            return std::ranges::find_if(mActives, Filter);
        }

        /// \brief Finds an active effect instance by its archetype handle.
        ///
        /// \param Handle The handle of the effect archetype to search for.
        /// \return An iterator to the found effect instance, or the end iterator if not found.
        ZYPHRYON_INLINE Vector<EffectHandle>::iterator FindByArchetype(EffectHandle Handle)
        {
            const auto Filter = [&](EffectHandle Other)
            {
                return mRegistry[Other.GetID()].GetArchetype()->GetHandle() == Handle;
            };
            return std::ranges::find_if(mActives, Filter);
        }

        /// \brief Finds the best position to insert an effect instance in the active list, sorted by remaining time.
        ///
        /// \param Instance The effect instance to find the position for.
        /// \return An iterator to the position where the effect instance should be inserted.
        ZYPHRYON_INLINE Vector<EffectHandle>::iterator FindBestPosition(ConstRef<Effect> Instance)
        {
            const auto Filter = [this](EffectHandle First, EffectHandle Second)
            {
                ConstRef<Effect> FirstEffect  = mRegistry[First.GetID()];
                ConstRef<Effect> SecondEffect = mRegistry[Second.GetID()];
                return FirstEffect.GetInterval() > SecondEffect.GetInterval();
            };
            return std::ranges::upper_bound(mActives, Instance.GetHandle(), Filter);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Catalog<Effect, kMaxInstances> mRegistry;
        Vector<EffectHandle>           mActives;
    };
}