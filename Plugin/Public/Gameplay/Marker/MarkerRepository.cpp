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

#include "MarkerRepository.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Load(Ref<Content::Service> Content, ConstStr8 Filename)
    {
        if (const Blob Data = Content.Find(Filename); Data)
        {
            TOMLParser Parser(Data.GetText());
            Load(Parser);
        }
        else
        {
            LOG_WARNING("Failed to load markers from '{}'", Filename);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Save(Ref<Content::Service> Content, ConstStr8 Filename) const
    {
        TOMLParser Parser;
        Save(Parser);

        Content.Save(Filename, Parser.Dump());
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Insert(ConstStr8 Name, Marker Parent)
    {
        for (UInt Start = 0, End = 0; End != ConstStr8::npos; Start = End + 1)
        {
            // Extract the next segment from the hierarchical name.
            End = Name.find_first_of('.', Start);

            // Check if the segment already exists under the parent.
            if (const Marker Token = GetTokenByName(Name.substr(0, End)); Token.IsEmpty())
            {
                MarkerArchetype Archetype = GetMutable(Parent).Extend(Name.substr(Start, End - Start));
                Parent                    = Archetype.GetHandle();
                Insert(Move(Archetype));
            }
            else
            {
                Parent = Token;
            }
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Delete(Marker Token)
    {
        Ref<MarkerArchetype> Archetype = GetMutable(Token);

        // Remove the token from the lookup table.
        mTokens.erase(Archetype.GetName());

        // Clear the path.
        Archetype.SetPath("");

        // Delete all children recursively.
        for (UInt8 Children = 1; Children <= Archetype.GetArity(); ++Children)
        {
            Delete(Archetype.GetHandle().With(Children));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Load(Ref<TOMLParser> Parser)
    {
        const TOMLArray Collection = Parser.GetRoot().GetArray("Tokens");

        Ref<MarkerArchetype> Root = GetMutable(Marker::kEmpty);
        Root.SetArity(Collection.GetSize());

        LoadItemRecursive(Collection, Root.GetHandle(), Root.GetPath());
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::Save(Ref<TOMLParser> Parser) const
    {
        const TOMLArray Collection = Parser.GetRoot().GetArray("Tokens");
        SaveItemRecursive(Collection, Get(Marker::kEmpty));
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::LoadItemRecursive(TOMLArray Collection, Marker Parent, ConstStr8 Prefix)
    {
        for (UInt32 Index = 0; Index < Collection.GetSize(); ++Index)
        {
            const TOMLSection Node     = Collection.GetSection(Index);
            const TOMLArray   Children = Node.GetArray("Children", false);

            MarkerArchetype Archetype;
            Archetype.SetHandle(Parent.With(Index + 1));

            if (!Prefix.empty())
            {
                Archetype.SetPath(Format("{}.{}", Prefix, Node.GetString("Name")));
            }
            else
            {
                Archetype.SetPath(Node.GetString("Name"));
            }

            if (!Children.IsNull())
            {
                Archetype.SetArity(Children.GetSize());

                LoadItemRecursive(Children, Archetype.GetHandle(), Archetype.GetPath());
            }

            Insert(Move(Archetype));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void MarkerRepository::SaveItemRecursive(TOMLArray Collection, ConstRef<MarkerArchetype> Parent) const
    {
        for (UInt8 Index = 1, Count = Parent.GetArity(); Index <= Count; ++Index)
        {
            ConstRef<MarkerArchetype> Child = Get(Parent.GetHandle().With(Index));

            TOMLSection Node = Collection.AddSection();
            Node.SetString("Name", Child.GetName());

            if (Child.GetArity() > 0)
            {
                SaveItemRecursive(Node.SetArray("Children"), Child);
            }
        }
    }
}