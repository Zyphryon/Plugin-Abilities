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

#include "StatArchetype.hpp"
#include <Zyphryon.Content/Service.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a registry of stat archetypes, allowing loading and saving from/to TOML resources.
    class StatRepository final
    {
    public:

        /// \brief Maximum number of stat archetypes that can be registered.
        static constexpr UInt32 kMaxArchetypes = 256;   // TODO: Macro Configurable

    public:

        /// \brief Loads stats archetypes from the content service.
        ///
        /// \param Content  The content service to load from.
        /// \param Filename The filename of the resource to load.
        void Load(Ref<Content::Service> Content, ConstStr8 Filename);

        /// \brief Saves stats archetypes to the content service.
        ///
        /// \param Content  The content service to save to.
        /// \param Filename The filename of the resource to save.
        void Save(Ref<Content::Service> Content, ConstStr8 Filename) const;

        /// \brief Allocates a new stat archetype in the repository.
        ///
        /// \return A reference to the newly allocated stat archetype.
        ZYPHRYON_INLINE Ref<StatArchetype> Allocate()
        {
            const StatHandle Handle = mArchetypes.Allocate();

            Ref<StatArchetype> Archetype = mArchetypes[Handle.GetID()];
            Archetype.SetHandle(Handle);
            return Archetype;
        }

        /// \brief Deletes a stat archetype from the repository.
        ///
        /// \param Archetype The stat archetype to delete.
        ZYPHRYON_INLINE void Delete(ConstRef<StatArchetype> Archetype)
        {
            LOG_ASSERT(Archetype.GetHandle().IsValid(), "Cannot delete a stat archetype with an invalid handle.");

            DeleteDependencies(Archetype);

            mArchetypes.Free(Archetype.GetHandle().GetID());
        }

        /// \brief Clears all stat archetypes from the repository.
        ZYPHRYON_INLINE void Clear()
        {
            mArchetypes.Clear();
        }

        /// \brief Retrieves a stat archetype by its handle.
        ///
        /// \param Handle The handle of the stat archetype to retrieve.
        /// \return The stat archetype associated with the given handle.
        ZYPHRYON_INLINE ConstRef<StatArchetype> Get(StatHandle Handle) const
        {
            return mArchetypes[Handle.GetID()];
        }

        /// \brief Retrieves all registered stat archetypes.
        ///
        /// \return A span containing all stat archetypes.
        ZYPHRYON_INLINE ConstSpan<StatArchetype> GetAll() const
        {
            return mArchetypes.GetSpan();
        }

        /// \brief Inserts a dependency relationship between two stats.
        ///
        /// \param Stat       The stat that depends on another stat.
        /// \param Dependency The stat that is depended upon.
        ZYPHRYON_INLINE void InsertDependency(StatHandle Stat, StatHandle Dependency)
        {
            mDependencies[Dependency].insert(Stat);
        }

        /// \brief Removes a dependency relationship between two stats.
        ///
        /// \param Stat       The stat that depends on another stat.
        /// \param Dependency The stat that is depended upon.
        ZYPHRYON_INLINE void RemoveDependency(StatHandle Stat, StatHandle Dependency)
        {
            if (const auto Iterator = mDependencies.find(Dependency); Iterator != mDependencies.end())
            {
                Iterator->second.erase(Stat);

                if (Iterator->second.empty())
                {
                    mDependencies.erase(Iterator);
                }
            }
        }

        /// \brief Notifies all stats that depend on the given stat by invoking the provided action.
        ///
        /// \param Stat   The stat whose dependents should be notified.
        /// \param Action The action to invoke for each dependent stat.
        template<typename Function>
        ZYPHRYON_INLINE void NotifyDependency(StatHandle Stat, AnyRef<Function> Action)
        {
            if (const auto Iterator = mDependencies.find(Stat); Iterator != mDependencies.end())
            {
                for (const StatHandle Dependent : Iterator->second)
                {
                    Action(Dependent);

                    // Recursively notify dependents of the dependent stat.
                    NotifyDependency(Dependent, Action);
                }
            }
        }

    private:

        /// \brief Loads stat archetypes from a TOML resource.
        ///
        /// \param Parser The TOML resource containing stat archetype definitions.
        void Load(Ref<TOMLParser> Parser);

        /// \brief Saves stat archetypes to a TOML resource.
        ///
        /// \param Parser The TOML resource to save stat archetype definitions into.
        void Save(Ref<TOMLParser> Parser) const;

        /// \brief Inserts all dependencies of a stat archetype into the repository.
        ///
        /// \param Archetype The stat archetype whose dependencies to insert.
        ZYPHRYON_INLINE void InsertDependencies(ConstRef<StatArchetype> Archetype)
        {
            Archetype.Traverse([&](StatHandle Dependency)
            {
                InsertDependency(Archetype.GetHandle(), Dependency);
            });
        }

        /// \brief Deletes all dependencies of a stat archetype from the repository.
        ///
        /// \param Archetype The stat archetype whose dependencies to delete.
        ZYPHRYON_INLINE void DeleteDependencies(ConstRef<StatArchetype> Archetype)
        {
            Archetype.Traverse([&](StatHandle Dependency)
            {
                RemoveDependency(Archetype.GetHandle(), Dependency);
            });
        }

    public:

        /// \brief Retrieves the singleton instance of the repository.
        ///
        /// \return A reference to the singleton repository instance.
        ZYPHRYON_INLINE static Ref<StatRepository> Instance()
        {
            static StatRepository Singleton;
            return Singleton;
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Pool<StatArchetype, kMaxArchetypes> mArchetypes;
        Table<StatHandle, Set<StatHandle>>  mDependencies;
    };
}