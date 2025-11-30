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
    /// \brief Manages a set of tokens with associated counts.
    class TokenSet final
    {
    public:

        /// \brief Polls for changes in the token counts and invokes the provided action for each change.
        ///
        /// \param Action The action to invoke for each token whose count has changed.
        template<typename Function>
        ZYPHRYON_INLINE void Poll(AnyRef<Function> Action)
        {
            for (auto [Handle, Value] : mNotifications)
            {
                if (const UInt32 Current = Count(Handle); Current != Value)
                {
                    Action(Handle, Value, Current);
                }
            }
            mNotifications.clear();
        }

        /// \brief Inserts tokens into the set, incrementing their counts by the specified amount.
        ///
        /// \param Handle The token handle to insert.
        /// \param Count  The amount to increment the token count by.
        ZYPHRYON_INLINE void Insert(Token Handle, UInt32 Count)
        {
            Handle.Iterate([this, Count](Token Child)
            {
                Ref<UInt32> Value = mRegistry[Child];

                mNotifications.emplace(Child, Exchange(Value, Value + Count));
            });
        }

        /// \brief Removes tokens from the set, decrementing their counts by the specified amount.
        ///
        /// \param Handle The token handle to remove.
        /// \param Count  The amount to decrement the token count by.
        ZYPHRYON_INLINE void Remove(Token Handle, UInt32 Count)
        {
            Handle.Iterate([this, Count](Token Child)
            {
                if (const auto Iterator = mRegistry.find(Child); Iterator != mRegistry.end())
                {
                    mNotifications.emplace(Child, Iterator->second);

                    if (Iterator->second <= Count)
                    {
                        mRegistry.erase(Iterator);
                    }
                    else
                    {
                        Iterator->second -= Count;
                    }
                }
            });
        }

        /// \brief Clears all tokens from the set.
        ZYPHRYON_INLINE void Clear()
        {
            mRegistry.clear();
        }

        /// \brief Retrieves the count of a specific token in the set.
        ///
        /// \param Handle The token handle to query.
        /// \return The count of the specified token.
        ZYPHRYON_INLINE UInt32 Count(Token Handle) const
        {
            const auto Iterator = mRegistry.find(Handle);
            return Iterator != mRegistry.end() ? Iterator->second : 0;
        }

        /// \brief Traverses all tokens in the set, invoking the provided action for each token and its count.
        ///
        /// \tparam Function The type of the action to invoke for each token.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            for (const auto [Handle, Count] : mRegistry)
            {
                Action(Handle, Count);
            }
        }

    private:

        /// \brief Represents a notification for a token count change.
        struct Notification final
        {
            /// \brief The token associated with the notification.
            Token  Key;

            /// \brief The previous count value of the token.
            UInt32 Value;

            /// \brief Generates a hash value for the notification based on its token.
            ///
            /// \return A hash value uniquely representing the notification.
            ZYPHRYON_INLINE UInt64 Hash() const
            {
                return Key.Hash();
            }

            /// \brief Compares two notifications for equality based on their tokens.
            ///
            /// \param Other The other notification to compare against.
            /// \return `true` if both notifications are for the same token, otherwise `false`.
            ZYPHRYON_INLINE Bool operator==(ConstRef<Notification> Other) const
            {
                return Key == Other.Key;
            }
        };

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Table<Token, UInt32> mRegistry;
        Set<Notification>    mNotifications;
    };
}