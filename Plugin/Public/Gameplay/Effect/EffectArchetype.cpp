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

#include "Gameplay/Effect/EffectArchetype.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void EffectArchetype::Load(TOMLSection Section)
    {
        mName     = Section.GetString("Name");
        mHandle   = Section.GetInteger("ID");
        mPolicies.Load(Section.GetSection("Policies"));
        mCategory.Load(Section.GetArray("Category"));

        if (GetApplication() == EffectApplication::Temporary)
        {
            mDuration.Load(Section.GetArray("Duration"));
            mPeriod.Load(Section.GetArray("Period"));
            mLimit = Section.GetInteger("Limit");
        }

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
        Section.SetString("Name", mName);
        Section.SetInteger("ID", mHandle.GetID());
        mPolicies.Save(Section.SetSection("Policies"));
        mCategory.Save(Section.SetArray("Category"));

        if (GetApplication() == EffectApplication::Temporary)
        {
            mDuration.Save(Section.SetArray("Length"));
            mPeriod.Save(Section.SetArray("Period"));
            Section.SetInteger("Limit", mLimit);
        }

        for (TOMLArray Bonuses = Section.SetArray("Bonuses"); ConstRef<EffectModifier> Modifier: mBonuses)
        {
            Modifier.Save(Bonuses.AddArray());
        }
    }
}
