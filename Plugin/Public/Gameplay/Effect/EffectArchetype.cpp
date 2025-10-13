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
        mHandle = EffectHandle(Section.GetInteger("ID"));
        mName   = Section.GetString("Name");
        mDuration.Load(Section.GetArray("Duration"));
        mPeriod.Load(Section.GetArray("Period"));
        mLimit.Load(Section.GetArray("Limit"));
        mCategory   = Enum::Cast(Section.GetString("Type"), EffectCategory::Temporary);
        mExpiration = Enum::Cast(Section.GetString("Expiration"), EffectExpiration::Single);
        mRefresh    = Enum::Cast(Section.GetString("Refresh"), EffectRefresh::Replace);
        mResolution = Enum::Cast(Section.GetString("Resolution"), EffectResolution::Additive);
        mStack      = Enum::Cast(Section.GetString("Stack"), EffectStack::Linear);

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
        mDuration.Save(Section.SetArray("Length"));
        mPeriod.Save(Section.SetArray("Period"));
        mLimit.Save(Section.SetArray("Limit"));
        Section.SetString("Category", Enum::GetName(mCategory));
        Section.SetString("Expiration", Enum::GetName(mExpiration));
        Section.SetString("Refresh", Enum::GetName(mRefresh));
        Section.SetString("Resolution", Enum::GetName(mResolution));
        Section.SetString("Stack", Enum::GetName(mStack));

        for (TOMLArray Bonuses = Section.SetArray("Bonuses"); ConstRef<StatModifier> Modifier: mBonuses)
        {
            Modifier.Save(Bonuses.AddArray());
        }
    }
}
