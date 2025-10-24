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

#include "Marker.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Defines the archetype of a marker.
    class MarkerArchetype final
    {
    public:

        /// \brief Default constructor, initializes members to default values.
        ZYPHRYON_INLINE MarkerArchetype()
            : mArity { 0 }
        {
        }

        /// \brief Constructs a marker archetype with the specified handle and name.
        ///
        /// \param Handle The marker handle.
        /// \param Path   The path of the marker.
        /// \param Arity  The arity of the marker.
        ZYPHRYON_INLINE MarkerArchetype(Marker Handle, ConstStr8 Path, UInt8 Arity)
            : mHandle { Handle },
              mPath   { Path },
              mArity  { Arity }
        {
        }

        /// \brief Sets the handle of the marker.
        ///
        /// \param Handle The marker handle to assign.
        ZYPHRYON_INLINE void SetHandle(Marker Handle)
        {
            mHandle = Handle;
        }

        /// \brief Retrieves the handle of the marker.
        ///
        /// \return The marker handle.
        ZYPHRYON_INLINE Marker GetHandle() const
        {
            return mHandle;
        }

        /// \brief Sets the path of the marker.
        ///
        /// \param Path The path to assign.
        ZYPHRYON_INLINE void SetPath(ConstStr8 Path)
        {
            mPath = Path;
        }

        /// \brief Retrieves the path of the marker.
        ///
        /// \return The marker path.
        ZYPHRYON_INLINE ConstStr8 GetPath() const
        {
            return mPath;
        }

        /// \brief Retrieves the name of the marker.
        ///
        /// \return The marker name.
        ZYPHRYON_INLINE ConstStr8 GetName() const
        {
            const UInt Offset = mPath.find_last_of('.');
            return (Offset != Str8::npos ? GetPath().substr(Offset + 1) : mPath);
        }

        /// \brief Sets the arity of the marker.
        ///
        /// \param Arity The arity to assign.
        ZYPHRYON_INLINE void SetArity(UInt8 Arity)
        {
            mArity = Arity;
        }

        /// \brief Retrieves the arity of the marker.
        ///
        /// \return The marker arity.
        ZYPHRYON_INLINE UInt8 GetArity() const
        {
            return mArity;
        }

        /// \brief Retrieves the active state of the marker.
        ///
        /// \return The marker active state.
        ZYPHRYON_INLINE Bool IsActive() const
        {
            return !mPath.empty();
        }

        /// \brief Checks if the marker is a root marker (has no parent).
        ///
        /// \return `true` if the marker is a root marker, `false` otherwise.
        ZYPHRYON_INLINE Bool IsRoot() const
        {
            return mHandle.IsRoot();
        }

        /// \brief Extends the marker archetype by increasing its arity and appending a new name segment.
        ///
        /// \param Name The name segment to append.
        /// \return A new marker archetype with increased arity and updated name.
        ZYPHRYON_INLINE MarkerArchetype Extend(ConstStr8 Name)
        {
            LOG_ASSERT(mArity < Marker::kLimit, "Exceeded maximum arity for marker.");

            const ConstStr8 Path = !mPath.empty() ? Format("{}.{}", mPath, Name) : Name;
            return MarkerArchetype(mHandle.With(++mArity), Path, 0);
        }

        /// \brief Compares the marker's handle for equality.
        ZYPHRYON_INLINE constexpr Bool operator==(Marker Handle) const
        {
            return mHandle == Handle;
        }

        /// \brief Compares the marker archetype for equality based on its handle.
        ZYPHRYON_INLINE constexpr Bool operator==(ConstRef<MarkerArchetype> Archetype) const
        {
            return mHandle == Archetype.GetHandle();
        }

        /// \brief Generates a hash value for the marker archetype based on its handle.
        ///
        /// \return A hash value uniquely representing the marker archetype.
        ZYPHRYON_INLINE constexpr UInt64 Hash() const
        {
            return mHandle.Hash();
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Marker mHandle;
        Str8   mPath;
        UInt8  mArity;
    };
}