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

    #include "Gameplay/Stat/StatFormula.hpp"

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // [   CODE   ]
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    namespace Gameplay
    {
        /// \brief An expression representing a direct value, a reference to another stat, or a formula.
        class StatInput final
        {
            // TODO: Refactor Formula pointer to be a handle type instead of a pointer (from 16b to 8b).

        public:

            /// \brief Defines the kind of data held by the stat input.
            enum class Kind : UInt8
            {
                Float,     ///< A direct floating-point value.
                Ref,       ///< A reference to another stat.
                Formula,   ///< A custom formula.
            };

            /// \brief Structure representing a reference to another stat with an optional base and coefficient.
            struct Reference final
            {
                /// \brief The handle of the referenced stat.
                Stat       Handle;

                /// \brief The scope context of the referenced stat.
                StatScope  Scope       = StatScope::Target;

                /// \brief The base value applied when referencing the stat.
                Real16     Base        = 0.0f;

                /// \brief The coefficient applied to the referenced stat.
                Real16     Coefficient = 1.0f;

                /// \brief Loads the reference data from a TOML array.
                ///
                /// \param Array The TOML array to load from.
                ZYPHRYON_INLINE void Load(TOMLArray Array)
                {
                    Handle      = Array.GetInteger(1);
                    Scope       = Array.GetEnum(2, StatScope::Target);
                    Base        = FloatToHalf(Array.GetReal(3));
                    Coefficient = FloatToHalf(Array.GetReal(4));
                }

                /// \brief Saves the reference data to a TOML array.
                ///
                /// \param Array The TOML array to save to.
                ZYPHRYON_INLINE void Save(TOMLArray Array) const
                {
                    Array.AddInteger(Handle.GetID());
                    Array.AddEnum(Scope);
                    Array.AddReal(HalfToFloat(Base));
                    Array.AddReal(HalfToFloat(Coefficient));
                }
            };

        public:

            /// \brief Default constructor, initializes an empty stat input.
            ZYPHRYON_INLINE StatInput() = default;

            /// \brief Constructs a stat input from a floating-point value.
            ///
            /// \param Value The floating-point value to use.
            ZYPHRYON_INLINE StatInput(Real32 Value)
                : mContainer { Value }
            {
            }

            /// \brief Constructs a stat input from a stat reference.
            ///
            /// \param Handle      The handle of the referenced stat.
            /// \param Scope       The scope context of the referenced stat.
            /// \param Base        The base value applied when referencing the stat.
            /// \param Coefficient The coefficient applied to the referenced stat.
            ZYPHRYON_INLINE StatInput(Stat Handle, StatScope Scope, Real32 Base, Real32 Coefficient)
                : mContainer { Reference { Handle, Scope, FloatToHalf(Base), FloatToHalf(Coefficient) } }
            {
            }

            /// \brief Constructs a stat input from a formula pointer.
            ///
            /// \param Formula The formula pointer to use.
            ZYPHRYON_INLINE StatInput(Ptr<StatFormula> Formula)
                : mContainer { Formula }
            {
            }

            /// \brief Constructs a stat input from a TOML array.
            ///
            /// \param Array The TOML array to load from.
            ZYPHRYON_INLINE StatInput(TOMLArray Array)
            {
                Load(Array);
            }

            /// \brief Retrieves the kind of data held by the stat input.
            ///
            /// \return The kind of data.
            ZYPHRYON_INLINE Kind GetKind() const
            {
                return static_cast<Kind>(mContainer.GetIndex());
            }

            /// \brief Retrieves the underlying data of the stat input.
            ///
            /// \return A constant reference to the underlying data.
            template<typename Type>
            ZYPHRYON_INLINE ConstRef<Type> GetData() const
            {
                return mContainer.GetData<Type>();
            }

            /// \brief Resolves the stat input using the source context.
            ///
            /// \param Source The source context to retrieve stat values from.
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

            /// \brief Resolves the stat input using both source and target contexts.
            ///
            /// \param Source The source context to retrieve stat values from.
            /// \param Target The target context to retrieve stat values from.
            template<typename Context>
            ZYPHRYON_INLINE Real32 Resolve(ConstRef<Context> Source, ConstRef<Context> Target) const
            {
                switch (GetKind())
                {
                case Kind::Float:
                    return GetData<Real32>();
                case Kind::Ref:
                {
                    const auto [Handle, Scope, Base, Coefficient] = GetData<Reference>();

                    switch (Scope)
                    {
                    case StatScope::Source:
                        return HalfToFloat(Base) + Source.GetStat(Handle) * HalfToFloat(Coefficient);
                    case StatScope::Target:
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

            /// \brief Traverses all dependencies in the stat input, invoking the provided action for each.
            ///
            /// \param Action The action to invoke for each dependency.
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

            /// \brief Traverses dependencies in the stat input filtered by scope, invoking the provided action for each.
            ///
            /// \param Action The action to invoke for each dependency.
            /// \param Scope  The scope to filter dependencies by.
            template<typename Function>
            ZYPHRYON_INLINE void Traverse(AnyRef<Function> Action, StatScope Scope) const
            {
                switch (GetKind())
                {
                    case Kind::Ref:
                    {
                        ConstRef<Reference> Data = GetData<Reference>();

                        if (Data.Scope == Scope)
                        {
                            Action(Data.Handle);
                        }
                        break;
                    }
                    case Kind::Formula:
                    {
                        GetData<Ptr<StatFormula>>()->Traverse(Action, Scope);
                        break;
                    }
                    default:
                        break;
                }
            }

            /// \brief Loads the stat input from a TOML array.
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

            /// \brief Saves the stat input to a TOML array.
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
            ZYPHRYON_INLINE static StatInput CreateSourceRef(Stat Handle, Real32 Base = 0.0f, Real32 Coefficient = 1.0f)
            {
                return StatInput(Handle, StatScope::Source, Base, Coefficient);
            }

            /// \brief Creates a target stat reference input.
            ///
            /// \param Handle      The handle of the referenced stat.
            /// \param Base        The base value to apply when referencing the stat.
            /// \param Coefficient The coefficient to apply when referencing the stat.
            /// \return A new stat input representing a target stat reference.
            ZYPHRYON_INLINE static StatInput CreateTargetRef(Stat Handle, Real32 Base = 0.0f, Real32 Coefficient = 1.0f)
            {
                return StatInput(Handle, StatScope::Target, Base, Coefficient);
            }

        private:

            // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
            // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

            Variant<Real32, Reference, Ptr<StatFormula>> mContainer;
        };
    }