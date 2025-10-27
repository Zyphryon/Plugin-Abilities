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

#include "Gameplay/Ability/AbilityData.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Manages a collection of ability instances.
    class AbilitySet final
    {
    public:

        /// \brief Inserts a new ability archetype into the set.
        ///
        /// \param Archetype The ability archetype to insert.
        ZYPHRYON_INLINE void Insert(ConstRef<AbilityArchetype> Archetype)
        {
            mRegistry.emplace(Archetype);
        }

        /// \brief Removes an ability from the set by its handle.
        ///
        /// \param Handle The handle of the ability to remove.
        ZYPHRYON_INLINE void Remove(Ability Handle)
        {
            mRegistry.erase(Handle);
        }

        /// \brief Attempts to retrieve an ability by its handle.
        ///
        /// \param Handle The handle of the ability to retrieve.
        /// \return A pointer to the ability if found, otherwise nullptr.
        ZYPHRYON_INLINE Ptr<AbilityData> TryGet(Ability Handle)
        {
            if (const auto Iterator = mRegistry.find(Handle); Iterator != mRegistry.end())
            {
                return const_cast<Ptr<AbilityData>>(&*Iterator);
            }
            return nullptr;
        }

        /// \brief Clears all abilities from the set.
        ZYPHRYON_INLINE void Clear()
        {
            mRegistry.clear();
        }

        /// \brief Traverses all abilities in the set and invokes the provided action for each ability.
        ///
        /// \param Action The action to invoke for each ability.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (ConstRef<AbilityData> Instance : mRegistry)
            {
                Action(Instance);
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Set<AbilityData> mRegistry;
    };
}