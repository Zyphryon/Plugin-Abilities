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

#include "StatHandle.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief Represents either a direct stat value or a reference to another stat.
    class StatProxy final
    {
    public:

        /// \brief Enumeration defining the type of data held by the proxy.
        enum class Kind : UInt8
        {
            Ref,    ///< Reference to another stat.
            Float,  ///< Direct stat value.
        };

        /// \brief Enumeration defining the origin context of the stat.
        enum class Origin : UInt8
        {
            Source, ///< The stat originates from the source context.
            Target, ///< The stat originates from the target context.
        };

        /// \brief Enumeration defining how the stat value should be resolved.
        enum class Resolution : UInt8
        {
            Live,     ///< Resolve thAe stat value live from the context.
            Snapshot, ///< Resolve the stat value from a snapshot of the context.
        };

        /// \brief Structure representing a reference to another stat with an optional coefficient.
        struct Reference final
        {
            /// \brief Handle to the referenced stat.
            StatHandle Handle;

            /// \brief Context of the referenced stat.
            Origin     Origin      = Origin::Target;

            /// \brief How the stat should be resolved (live or snapshot).
            Resolution Resolution  = Resolution::Snapshot;

            /// \brief Coefficient to apply when referencing another stat.
            Real32     Coefficient = 1.0f;

            /// \brief Loads the reference data from a TOML array.
            ///
            /// \param Array The TOML array to load from.
            ZYPHRYON_INLINE void Load(TOMLArray Array)
            {
                Handle      = Array.GetInteger(1);
                Coefficient = Array.GetReal(2);
                Resolution  = Enum::Cast(Array.GetString(3), Resolution::Snapshot);
                Origin      = Enum::Cast(Array.GetString(4), Origin::Target);
            }

            /// \brief Saves the reference data to a TOML array.
            ///
            /// \param Array The TOML array to save to.
            ZYPHRYON_INLINE void Save(TOMLArray Array) const
            {
                Array.AddInteger(Handle.GetID());
                Array.AddReal(Coefficient);
                Array.AddString(Enum::GetName(Resolution));
                Array.AddString(Enum::GetName(Origin));
            }
        };

    public:

        /// \brief Default constructor, initializes an empty proxy.
        ZYPHRYON_INLINE StatProxy() = default;

        /// \brief Constructs a proxy holding a direct stat value.
        ///
        /// \param Value The direct stat value.
        ZYPHRYON_INLINE explicit StatProxy(Real32 Value)
            : mContainer { Value }
        {
        }

        /// \brief Constructs a proxy holding a reference to another stat with an optional coefficient.
        ///
        /// \param Handle      The handle of the referenced stat.
        /// \param Origin      The origin context of the referenced stat.
        /// \param Resolution  How the stat should be resolved.
        /// \param Coefficient The coefficient to apply when referencing the stat.
        ZYPHRYON_INLINE explicit StatProxy(StatHandle Handle, Origin Origin, Resolution Resolution, Real32 Coefficient)
            : mContainer { Reference { Handle, Origin, Resolution, Coefficient } }
        {
        }

        /// \brief Retrieves the kind of data held by the proxy.
        ///
        /// \return The kind of data.
        ZYPHRYON_INLINE Kind GetKind() const
        {
            return static_cast<Kind>(mContainer.GetIndex());
        }

        /// \brief Retrieves the data held by the proxy, cast to the specified type.
        ///
        /// \tparam Type The type to cast the data to.
        template<typename Type>
        ZYPHRYON_INLINE ConstRef<Type> GetData() const
        {
            return mContainer.GetData<Type>();
        }

        /// \brief Retrieves the origin of the referenced stat if the proxy holds a reference.
        ///
        /// \return The origin of the referenced stat, or `Target` if the proxy holds a direct value.
        ZYPHRYON_INLINE Origin GetOrigin() const
        {
            return (GetKind() == Kind::Ref ? GetData<Reference>().Origin : Origin::Target);
        }

        /// \brief Retrieves the handle of the referenced stat if the proxy holds a reference.
        ///
        /// \return The handle of the referenced stat, or an invalid handle if the proxy holds a direct value.
        ZYPHRYON_INLINE StatHandle GetDependency() const
        {
            return (GetKind() == Kind::Ref ? GetData<Reference>().Handle : StatHandle());
        }

        /// \brief Retrieves the resolution mode of the referenced stat if the proxy holds a reference.
        ///
        /// \return The resolution mode of the referenced stat, or `Snapshot` if the proxy holds a direct value.
        ZYPHRYON_INLINE Resolution GetResolution() const
        {
            return (GetKind() == Kind::Ref ? GetData<Reference>().Resolution : Resolution::Snapshot);
        }

        /// \brief Resolves the proxy to a concrete stat value using the provided source context.
        ///
        /// \param Source The source context providing access to stats.
        /// \return The resolved stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Resolve(ConstRef<Context> Source) const
        {
            return Resolve(Source, Source);
        }

        /// \brief Resolves the proxy to a concrete stat value using the provided source and target contexts.
        ///
        /// \param Source The source context providing access to stats.
        /// \param Target The target context providing access to stats.
        /// \return The resolved stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Resolve(ConstRef<Context> Source, ConstRef<Context> Target) const
        {
            switch (GetKind())
            {
            case Kind::Float:
                return GetData<Real32>();
            case Kind::Ref:
            {
                ConstRef<Reference> Data = GetData<Reference>();

                switch (Data.Origin)
                {
                case Origin::Source:
                    return Source.GetStat(Data.Handle) * Data.Coefficient;
                case Origin::Target:
                    return Target.GetStat(Data.Handle) * Data.Coefficient;
                }
            }
            }
            return 0.0f;
        }

        /// \brief Loads the proxy data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE void Load(TOMLArray Array)
        {
            const ConstStr8 Type = Array.GetString(0);

            if (Type == "Float")
            {
                mContainer.Emplace<Real32>(static_cast<Real32>(Array.GetReal(1)));
            }
            else if (Type == "Ref")
            {
                mContainer.Emplace<Reference>().Load(Array);
            }
        }

        /// \brief Saves the proxy data to a TOML array.
        ///
        /// \param Array The TOML array to save to.
        ZYPHRYON_INLINE void Save(TOMLArray Array) const
        {
            switch (GetKind())
            {
            case Kind::Float:
                Array.AddString("Float");
                Array.AddReal(mContainer.GetData<Real32>());
                break;
            case Kind::Ref:
                Array.AddString("Ref");
                GetData<Reference>().Save(Array);
                break;
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Variant<Reference, Real32> mContainer;
    };
}