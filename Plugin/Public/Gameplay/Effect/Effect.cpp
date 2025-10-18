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

#include "Effect.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Effect::Merge(ConstRef<Effect> Other)
    {
        // Merge the expiration based on the refresh policy.
        switch (mArchetype->GetRefresh())
        {
        case EffectRefresh::Keep:
            break;
        case EffectRefresh::Replace:
            mExpiration = Other.GetExpiration();
        case EffectRefresh::Longest:
            mExpiration = Max(mExpiration, Other.GetExpiration());
        case EffectRefresh::Extend:
            mExpiration += Other.GetExpiration();
            break;
        }

        // Merge the stacks based on the stacking behavior.
        const Bool IsFull = (mArchetype->GetLimit() == mStack);
        mStack = Min(mStack + Other.GetStack(), mArchetype->GetLimit());

        // Merge the intensity based on the intensity policy.
        switch (mArchetype->GetResolution())
        {
        case EffectResolution::Additive:
            if (!IsFull)
            {
                mIntensity += Other.GetIntensity();
            }
            break;
        case EffectResolution::Replace:
            if (!IsAlmostEqual(mIntensity, Other.GetIntensity()))
            {
                mIntensity = Other.GetIntensity();
                mStack     = 1;
            }
            break;
        case EffectResolution::Highest:
            if (!IsAlmostEqual(mIntensity, Other.GetIntensity()))
            {
                mIntensity = Max(mIntensity, Other.GetIntensity());
                mStack     = 1;
            }
            break;
        case EffectResolution::Lowest:
            if (!IsAlmostEqual(mIntensity, Other.GetIntensity()))
            {
                mIntensity = Min(mIntensity, Other.GetIntensity());
                mStack     = 1;
            }
            break;
        case EffectResolution::Average:
            if (!IsFull)
            {
                mIntensity = (mIntensity + Other.GetIntensity()) * 0.5f;
            }
            break;
        }
    }
}