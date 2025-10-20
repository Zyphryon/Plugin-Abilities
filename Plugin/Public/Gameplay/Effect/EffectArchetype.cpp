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

#include "EffectArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void EffectArchetype::Load(TOMLSection Section)
    {
        mHandle      = EffectHandle(Section.GetInteger("ID"));
        mName        = Section.GetString("Name");
        mCategory    = Section.GetInteger("Category");
        mDuration.Load(Section.GetArray("Duration"));
        mPeriod.Load(Section.GetArray("Period"));
        mLimit       = Section.GetInteger("Limit");
        mApplication = Enum::Cast(Section.GetString("Application"), EffectApplication::Temporary);
        mExpiration  = Enum::Cast(Section.GetString("Expiration"), EffectExpiration::Single);
        mRefresh     = Enum::Cast(Section.GetString("Refresh"), EffectRefresh::Replace);
        mResolution  = Enum::Cast(Section.GetString("Resolution"), EffectResolution::Additive);
        mStack       = Enum::Cast(Section.GetString("Stack"), EffectStack::Linear);

        if (const TOMLArray Bonuses = Section.GetArray("Bonuses"); !Bonuses.IsEmpty())
        {
            mBonuses.resize(Bonuses.GetSize());

            for (UInt32 Element = 0; Element < mBonuses.size(); ++Element)
            {
                mBonuses[Element].Load(Bonuses.GetArray(Element));
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void EffectArchetype::Save(TOMLSection Section) const
    {
        Section.SetInteger("ID", mHandle.GetID());
        Section.SetString("Name", mName);
        Section.SetInteger("Category", mCategory.GetID());
        mDuration.Save(Section.SetArray("Length"));
        mPeriod.Save(Section.SetArray("Period"));
        Section.SetInteger("Limit", mLimit);
        Section.SetString("Application", Enum::GetName(mApplication));
        Section.SetString("Expiration", Enum::GetName(mExpiration));
        Section.SetString("Refresh", Enum::GetName(mRefresh));
        Section.SetString("Resolution", Enum::GetName(mResolution));
        Section.SetString("Stack", Enum::GetName(mStack));

        for (TOMLArray Bonuses = Section.SetArray("Bonuses"); ConstRef<EffectModifier> Modifier: mBonuses)
        {
            Modifier.Save(Bonuses.AddArray());
        }
    }
}
