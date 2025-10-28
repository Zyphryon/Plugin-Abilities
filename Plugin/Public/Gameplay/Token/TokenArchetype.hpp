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

#include "Gameplay/Token/Token.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of a token.
    class TokenArchetype final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE TokenArchetype()
            : mSize { 0 }
        {
        }

        /// \brief Constructs a token archetype with specified parameters.
        ///
        /// \param Handle The token handle.
        /// \param Size   The size of the token.
        /// \param Path   The path of the token.
        ZYPHRYON_INLINE TokenArchetype(Token Handle, UInt8 Size, ConstStr8 Path)
            : mHandle { Handle },
              mSize   { Size },
              mPath   { Path }
        {
        }

        /// \brief Sets the handle of the token.
        ///
        /// \param Handle The token handle to assign.
        ZYPHRYON_INLINE void SetHandle(Token Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the handle of the token.
        ///
        /// \return The token handle.
        ZYPHRYON_INLINE Token GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the size of the token.
        ///
        /// \param Size The size to assign.
        ZYPHRYON_INLINE void SetSize(UInt8 Size)
        {
            mSize = Size;
        }

        /// \brief Retrieves the size of the token.
        ///
        /// \return The token size.
        ZYPHRYON_INLINE UInt8 GetSize() const
        {
            return mSize;
        }

        /// \brief Sets the path of the token.
        ///
        /// \param Path The path to assign.
        ZYPHRYON_INLINE void SetPath(ConstStr8 Path)
        {
            mPath = Path;
        }

        /// \brief Retrieves the path of the token.
        ///
        /// \return The token path.
        ZYPHRYON_INLINE ConstStr8 GetPath() const
        {
            return mPath;
        }

        /// \brief Retrieves the name of the token.
        ///
        /// \return The token name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            const UInt Offset = mPath.find_last_of('.');
            return (Offset != Str8::npos ? GetPath().substr(Offset + 1) : mPath);
        }

        /// \brief Retrieves the active state of the token.
        ///
        /// \return The token active state.
        ZYPHRYON_INLINE Bool IsActive() const
        {
            return !mPath.empty();
        }

        /// \brief Checks if the token is a root token (has no parent).
        ///
        /// \return `true` if the token is a root token, `false` otherwise.
        ZYPHRYON_INLINE Bool IsRoot() const
        {
            return mHandle.IsRoot();
        }

        /// \brief Extends the token archetype by increasing its size and appending a new name segment.
        ///
        /// \param Name The name segment to append.
        /// \return A new token archetype with increased size and updated name.
        ZYPHRYON_INLINE TokenArchetype Extend(ConstStr8 Name)
        {
            LOG_ASSERT(mSize < Token::kDepth, "Exceeded maximum size for token.");

            const ConstStr8 Path = !mPath.empty() ? Format("{}.{}", mPath, Name) : Name;
            return TokenArchetype(mHandle.With(++mSize), 0, Path);
        }

        /// \brief Compares the token's handle for equality.
        ZYPHRYON_INLINE Bool operator==(Token Handle) const
        {
            return mHandle == Handle;
        }

        /// \brief Compares the token archetype for equality based on its handle.
        ZYPHRYON_INLINE Bool operator==(ConstRef<TokenArchetype> Archetype) const
        {
            return mHandle == Archetype.GetHandle();
        }

        /// \brief Generates a hash value for the token archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the token archetype.
        ZYPHRYON_INLINE UInt64 Hash() const
        {
            return mHandle.Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Token mHandle;
        UInt8 mSize;
        Str8  mPath;
    };
}