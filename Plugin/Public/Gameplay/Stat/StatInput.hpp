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

#include "StatFormula.hpp"
#include "StatPolicies.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Gameplay
{
    /// \brief An expression representing a direct value, a reference to another stat, or a formula.
    class StatInput final
    {
    public:

        /// \brief Defines the kind of data held by the proxy.
        enum class Kind : UInt8
        {
            Float,     ///< Direct stat value.
            Ref,       ///< Reference to another stat.
            Formula,   ///< A formula to compute the stat value.
        };

        /// \brief Structure representing a reference to another stat with an optional coefficient.
        struct Reference final
        {
            /// \brief Handle to the referenced stat.
            StatHandle Handle;

            /// \brief Context of the referenced stat.
            StatOrigin Origin      = StatOrigin::Target;

            /// \brief Base value to apply when referencing another stat.
            Real16     Base        = 0.0f;

            /// \brief Coefficient to apply when referencing another stat.
            Real16     Coefficient = 1.0f;

            /// \brief Loads the reference data from a TOML array.
            ///
            /// \param Array The TOML array to load from.
            ZYPHRYON_INLINE void Load(TOMLArray Array)
            {
                Handle      = Array.GetInteger(1);
                Base        = FloatToHalf(Array.GetReal(2));
                Coefficient = FloatToHalf(Array.GetReal(3));
                Origin      = Enum::Cast(Array.GetString(4), StatOrigin::Target);
            }

            /// \brief Saves the reference data to a TOML array.
            ///
            /// \param Array The TOML array to save to.
            ZYPHRYON_INLINE void Save(TOMLArray Array) const
            {
                Array.AddInteger(Handle.GetID());
                Array.AddReal(HalfToFloat(Base));
                Array.AddReal(HalfToFloat(Coefficient));
                Array.AddString(Enum::GetName(Origin));
            }
        };

    public:

        /// \brief Default constructor, initializes an empty proxy.
        ZYPHRYON_INLINE StatInput() = default;

        /// \brief Constructs a proxy holding a direct stat value.
        ///
        /// \param Value The direct stat value.
        ZYPHRYON_INLINE StatInput(Real32 Value)
            : mContainer { Value }
        {
        }

        /// \brief Constructs a proxy holding a reference to another stat.
        ///
        /// \param Handle      The handle of the referenced stat.
        /// \param Origin      The origin context of the referenced stat.
        /// \param Base        The base value to apply when referencing the stat.
        /// \param Coefficient The coefficient to apply when referencing the stat.
        ZYPHRYON_INLINE StatInput(StatHandle Handle, StatOrigin Origin, Real32 Base, Real32 Coefficient)
            : mContainer { Reference { Handle, Origin, FloatToHalf(Base), FloatToHalf(Coefficient) } }
        {
        }

        /// \brief Constructs a proxy holding a formula to compute the stat value.
        ///
        /// \param Formula The formula used to compute the stat value.
        ZYPHRYON_INLINE StatInput(Ptr<StatFormula> Formula)
            : mContainer { Formula }
        {
        }

        /// \brief Constructs a proxy by loading data from a TOML array.
        ///
        /// \param Array The TOML array to load from.
        ZYPHRYON_INLINE StatInput(TOMLArray Array)
        {
            Load(Array);
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

        /// \brief Resolves the proxy to a concrete stat value using the provided source context.
        ///
        /// \param Source The source context providing access to stats.
        /// \return The resolved stat value.
        template<typename Context>
        ZYPHRYON_INLINE Real32 Resolve(ConstRef<Context> Source) const
        {
            switch (GetKind())
            {
            case Kind::Float:
                return GetData<Real32>();
            case Kind::Ref:
            {
                ConstRef<Reference> Data = GetData<Reference>();
                return HalfToFloat(Data.Base) + Source.GetStat(Data.Handle) * HalfToFloat(Data.Coefficient);
            }
            case Kind::Formula:
            {
                return GetData<Ptr<StatFormula>>()->Calculate(Source);
            }
            }
            return 0.0f;
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
                const auto [Handle, Origin, Base, Coefficient] = GetData<Reference>();

                switch (Origin)
                {
                case StatOrigin::Source:
                    return HalfToFloat(Base) + Source.GetStat(Handle) * HalfToFloat(Coefficient);
                case StatOrigin::Target:
                    return HalfToFloat(Base) + Target.GetStat(Handle) * HalfToFloat(Coefficient);
                }
            }
            case Kind::Formula:
            {
                return GetData<Ptr<StatFormula>>()->Calculate(Source, Target);
            }
            }
            return 0.0f;
        }

        /// \brief Iterates over all dependencies referenced by this proxy.
        ///
        /// \param Action The action to apply to each dependency.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action) const
        {
            switch (GetKind())
            {
            case Kind::Ref:
            {
                Action(GetData<Reference>().Handle);
                break;
            }
            case Kind::Formula:
            {
                GetData<Ptr<StatFormula>>()->Traverse(Action);
                break;
            }
            default:
                break;
            }
        }

        /// \brief Iterates over all dependencies referenced by this proxy.
        ///
        /// \param Action The action to apply to each dependency.
        /// \param Origin The origin context of dependencies to consider.
        template<typename Function>
        ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatOrigin Origin) const
        {
            switch (GetKind())
            {
                case Kind::Ref:
                {
                    ConstRef<Reference> Data = GetData<Reference>();

                    if (Data.Origin == Origin)
                    {
                        Action(Data.Handle);
                    }
                    break;
                }
                case Kind::Formula:
                {
                    GetData<Ptr<StatFormula>>()->Traverse(Action, Origin);
                    break;
                }
                default:
                    break;
            }
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
            else if (Type == "Formula")
            {
                LOG_WARNING("Loading formulas is not supported yet.");
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
            case Kind::Formula:
                Array.AddString("Formula");
                LOG_WARNING("Saving formulas is not supported yet.");
                break;
            }
        }

    public:

        /// \brief Creates a source stat reference input.
        ///
        /// \param Handle      The handle of the referenced stat.
        /// \param Base        The base value to apply when referencing the stat.
        /// \param Coefficient The coefficient to apply when referencing the stat.
        /// \return A new stat input representing a source stat reference.
        ZYPHRYON_INLINE static StatInput CreateSourceReference(StatHandle Handle, Real32 Base = 0.0f, Real32 Coefficient = 1.0f)
        {
            return StatInput(Handle, StatOrigin::Source, Base, Coefficient);
        }

        /// \brief Creates a target stat reference input.
        ///
        /// \param Handle      The handle of the referenced stat.
        /// \param Base        The base value to apply when referencing the stat.
        /// \param Coefficient The coefficient to apply when referencing the stat.
        /// \return A new stat input representing a target stat reference.
        ZYPHRYON_INLINE static StatInput CreateTargetReference(StatHandle Handle, Real32 Base = 0.0f, Real32 Coefficient = 1.0f)
        {
            return StatInput(Handle, StatOrigin::Target, Base, Coefficient);
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Variant<Real32, Reference, Ptr<StatFormula>> mContainer;
    };
}