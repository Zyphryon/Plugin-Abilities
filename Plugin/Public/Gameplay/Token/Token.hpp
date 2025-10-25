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

#include <Zyphryon.Base/Base.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents a hierarchical token used for categorization and identification.
    class Token final
    {
    public:

        /// \brief Represents an invalid token key.
        static constexpr UInt32 kEmpty                 = 0;

        /// \brief Maximum depth of the token hierarchy.
        static constexpr UInt32 kDepth                 = 6;

        /// \brief Limit values for each hierarchical level of the token.
        static constexpr Array<UInt32, kDepth> kLimit  = { 0x40, 0x20, 0x20, 0x20, 0x20, 0x40 };

        /// \brief Bit masks for each level
        static constexpr Array<UInt32, kDepth> kMasks  = { 0x3F, 0x1F, 0x1F, 0x1F, 0x1F, 0x3F };

        /// \brief Bit shifts for each level
        static constexpr Array<UInt32, kDepth> kShifts = { 0x00, 0x06, 0x0B, 0x10, 0x15, 0x1A };

    public:

        /// \brief Default constructor, initializes an empty token.
        ZYPHRYON_INLINE constexpr Token()
            : mKey { kEmpty }
        {
        }

        /// \brief Constructs a token from a pre-encoded key.
        ///
        /// \param Key The pre-encoded token key.
        ZYPHRYON_INLINE constexpr Token(UInt32 Key)
            : mKey { Key }
        {
        }

        /// \brief Retrieves the encoded identifier of the token.
        ///
        /// \return The encoded token identifier.
        ZYPHRYON_INLINE constexpr UInt32 GetID() const
        {
            return mKey;
        }

        /// \brief Retrieves the value at a specific hierarchical level of the token.
        ///
        /// \param Level The hierarchical level to retrieve (0-based).
        /// \return The value at the specified hierarchical level.
        ZYPHRYON_INLINE constexpr UInt32 GetLevel(UInt8 Level) const
        {
            return (mKey >> kShifts[Level]) & kMasks[Level];
        }

        /// \brief Checks if the token is empty.
        ///
        /// \return `true` if the token is empty, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsEmpty() const
        {
            return mKey == kEmpty;
        }

        /// \brief Checks if the token is a root token (has no parent).
        ///
        /// \return `true` if the token is a root token, `false` otherwise.
        ZYPHRYON_INLINE constexpr Bool IsRoot() const
        {
            return mKey <= kLimit[0];
        }

        /// \brief Iterates over each hierarchical level of the token, invoking the provided action.
        ///
        /// \tparam Function The type of the action to invoke for each level.
        template<typename Function>
        ZYPHRYON_INLINE void Iterate(AnyRef<Function> Action) const
        {
            Token Handle;

            for (UInt8 Level = 0; Level < kDepth; ++Level)
            {
                if (const UInt8 Value = GetLevel(Level); Value != 0)
                {
                    Handle.mKey |= (Value & kMasks[Level]) << kShifts[Level];

                    Action(Handle);
                }
                else
                {
                    break;
                }
            }
        }

        /// \brief Creates a new token by appending a child token to the current token.
        ///
        /// \param Child The child token to append.
        /// \return The new token with the appended child.
        ZYPHRYON_INLINE constexpr Token With(UInt8 Child) const
        {
            for (UInt8 Level = 0; Level < kDepth; ++Level)
            {
                if (GetLevel(Level) == 0)
                {
                    return Token(mKey | (Child & kMasks[Level]) << kShifts[Level]);
                }
            }

            LOG_ASSERT(false, "Exceeded maximum token hierarchy depth.");
            return (* this);
        }

        /// \brief Compares two tokens for equality based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator==(ConstRef<Token> Other) const
        {
            return mKey == Other.mKey;
        }

        /// \brief Compares two tokens for inequality based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator!=(ConstRef<Token> Other) const
        {
            return !(*this == Other);
        }

        /// \brief Compares two tokens to determine if this token is less than the other based on their hierarchy.
        ZYPHRYON_INLINE constexpr Bool operator<(ConstRef<Token> Other) const
        {
            return mKey < Other.mKey;
        }

        /// \brief Generates a hash value for the token based on its encoded key.
        ///
        /// \return A hash value uniquely representing the token.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return GetID();
        }

    public:

        /// \brief Creates a token from a variable number of hierarchical tokens.
        ///
        /// \param Tokens The hierarchical tokens to encode.
        /// \return The constructed token.
        template<typename... Values>
        ZYPHRYON_INLINE static constexpr Token From(Values... Tokens)
        {
            static_assert(sizeof...(Tokens) <= kDepth, "Exceeded maximum token hierarchy depth.");
            return Token(Encode(std::index_sequence_for<Values...>{ }, static_cast<UInt8>(Tokens)...));
        }

    private:

        /// \brief Helper method to encode hierarchical tokens into a token.
        ///
        /// \param Tokens The hierarchical tokens to encode.
        /// \return The constructed token.
        template<UInt... Indices, typename... Values>
        ZYPHRYON_INLINE static constexpr UInt32 Encode(std::index_sequence<Indices...>, Values... Tokens)
        {
            return ((static_cast<UInt32>(static_cast<UInt8>(Tokens) & kMasks[Indices]) << kShifts[Indices]) | ...);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        UInt32 mKey;
    };
}