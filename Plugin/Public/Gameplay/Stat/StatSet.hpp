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

#include "Stat.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of stat instances and their dependencies.
    class StatSet final
    {
    public:

        /// \brief Type alias for the delegate used to notify observers of stat changes.
        using OnChangeDelegate  = Delegate<void(StatHandle), DelegateInlineSize::Small>;

        /// \brief Type alias for the signal used to notify observers of stat changes.
        using OnChangeDelegates = MulticastDelegate<void(StatHandle), DelegateInlineSize::Small>;

    public:

        /// \brief Polls the set for any stat changes and notifies observers.
        ZYPHRYON_INLINE void Poll()
        {
            for (const StatHandle Handle : mNotifications)
            {
                if (const auto Iterator = mObservers.find(Handle); Iterator != mObservers.end())
                {
                    (Iterator->second).Broadcast(Handle);
                }
            }
            mNotifications.clear();
        }

        /// \brief Inserts an observer for a specific stat.
        ///
        /// \param Handle   The handle of the stat to observe.
        /// \param Observer The delegate to be notified on stat changes.
        ZYPHRYON_INLINE void InsertObserver(StatHandle Handle, ConstRef<OnChangeDelegate> Observer)
        {
            mObservers[Handle].Add(Observer);
        }

        /// \brief Removes an observer for a specific stat.
        ///
        /// \param Handle   The handle of the stat being observed.
        /// \param Observer The delegate to be removed from notifications.
        ZYPHRYON_INLINE void RemoveObserver(StatHandle Handle, ConstRef<OnChangeDelegate> Observer)
        {
            if (const auto Iterator = mObservers.find(Handle); Iterator != mObservers.end())
            {
                Iterator->second.Remove(Observer);
            }
        }

        /// \brief Inserts a dependency relationship between two stats.
        ///
        /// \param Source     The stat that other stats depend on.
        /// \param Dependency The stat that depends on the source stat.
        ZYPHRYON_INLINE void InsertDependency(StatHandle Source, StatHandle Dependency)
        {
            mDependencies[Source].insert(Dependency);
        }

        /// \brief Inserts dependencies for all stats defined in the given archetype.
        ///
        /// \param Archetype The archetype whose dependencies to insert.
        ZYPHRYON_INLINE void InsertDependencies(ConstRef<StatArchetype> Archetype)
        {
            Archetype.Traverse([this, Handle = Archetype.GetHandle()](StatHandle Dependency)
            {
                InsertDependency(Dependency, Handle);
            });
        }

        /// \brief Removes a dependency relationship between two stats.
        ///
        /// \param Source     The stat that other stats depend on.
        /// \param Dependency The stat that depends on the source stat.
        ZYPHRYON_INLINE void RemoveDependency(StatHandle Source, StatHandle Dependency)
        {
            if (const auto Iterator = mDependencies.find(Source); Iterator != mDependencies.end())
            {
                Iterator->second.erase(Dependency);

                if (Iterator->second.empty())
                {
                    mDependencies.erase(Iterator);
                }
            }
        }

        /// \brief Removes dependencies for all stats defined in the given archetype.
        ///
        /// \param Archetype The archetype whose dependencies to remove.
        ZYPHRYON_INLINE void RemoveDependencies(ConstRef<StatArchetype> Archetype)
        {
            Archetype.Traverse([this, Handle = Archetype.GetHandle()](StatHandle Dependency)
            {
                RemoveDependency(Dependency, Handle);
            });
        }

        /// \brief Notifies all stats that depend on the given source stat, marking them as dirty.
        ///
        /// \param Source The stat that has changed and whose dependents need to be notified.
        ZYPHRYON_INLINE void NotifyDependencies(StatHandle Source)
        {
            if (const auto Iterator = mDependencies.find(Source); Iterator != mDependencies.end())
            {
                for (const StatHandle Dependent : Iterator->second)
                {
                    if (const auto Child = mRegistry.find(Dependent); Child != mRegistry.end())
                    {
                        if (!Child->IsDirty())
                        {
                            Child->SetDirty();

                            NotifyDependencies(Dependent);
                        }
                    }
                }
            }

            // If there are delegates for the source stat, notify them as well.
            if (const auto Iterator = mObservers.find(Source); Iterator != mObservers.end())
            {
                Ref<OnChangeDelegates> MulticastDelegate = (Iterator->second);

                if (!MulticastDelegate.IsEmpty())
                {
                    mNotifications.emplace(Source);
                }
            }
        }

        /// \brief Attempts to retrieve a stat instance by its handle.
        ///
        /// \param Handle The handle of the stat to retrieve.
        /// \return A pointer to the stat instance if found, or `nullptr` if not found.
        ZYPHRYON_INLINE ConstPtr<Stat> TryGet(StatHandle Handle) const
        {
            if (const auto Iterator = mRegistry.find(Handle); Iterator != mRegistry.end())
            {
                return &* Iterator;
            }
            return nullptr;
        }

        /// \brief Retrieves an existing stat instance by its archetype or inserts a new one if it doesn't exist.
        ///
        /// \param Archetype The archetype defining the stat to retrieve or insert.
        /// \return A reference to the existing or newly inserted stat instance.
        ZYPHRYON_INLINE Ref<Stat> GetOrInsert(ConstRef<StatArchetype> Archetype)
        {
            const auto [Iterator, Insertion] = mRegistry.emplace(Archetype);

            if (Insertion)
            {
                InsertDependencies(Archetype);
            }
            return const_cast<Ref<Stat>>(* Iterator);
        }

        /// \brief Clears all stat instances and their dependencies from the set.
        ZYPHRYON_INLINE void Clear()
        {
            // Clear all stats.
            mRegistry.clear();

            // Clear all observers.
            mObservers.clear();

            // Clear all dependencies.
            mDependencies.clear();
        }

        /// \brief Iterates over all stat instances in the set.
        ///
        /// \param Action The action to apply to each stat instance.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<Stat> Instance : mRegistry)
            {
                Action(Instance);
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<Stat>                            mRegistry;
        Table<StatHandle, Set<StatHandle>>   mDependencies;
        Table<StatHandle, OnChangeDelegates> mObservers;
        Set<StatHandle>                      mNotifications;
    };
}