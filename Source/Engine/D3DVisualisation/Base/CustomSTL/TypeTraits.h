#pragma once

#include "stdafx.h"

namespace SpringWindEngine
{
	// TYPE TRAIT Can_scalar_delete
	template<class Yty, class = void>
	struct Can_scalar_delete : std::false_type
	{};
	template<class Yty>
	struct Can_scalar_delete<Yty, std::void_t<decltype(delete std::declval<Yty*>())>> : std::true_type
	{};

	// TYPE TRAIT Can_array_delete
	template<class Yty, class = void>
	struct Can_array_delete : std::false_type
	{};
	template<class Yty>
	struct Can_array_delete<Yty, std::void_t<decltype(delete[] std::declval<Yty*>())>> : std::true_type
	{};

	// TYPE TRAIT Can_call_function_object
	template<class Fx, class Arg, class = void>
	struct Can_call_function_object : std::false_type
	{};
	template<class Fx, class Arg>
	struct Can_call_function_object<Fx, Arg, std::void_t<decltype(std::declval<Fx>()(std::declval<Arg>()))>> : std::true_type
	{};

	// TYPE TRAIT SP_convertible
	template<class Yty, class Ty>
	struct SP_convertible : std::is_convertible<Yty*, Ty*>::type
	{};
	template<class Yty, class Uty>
	struct SP_convertible<Yty, Uty[]> : std::is_convertible<Yty(*)[], Uty(*)[]>::type
	{};
	template<class Yty, class Uty, size_t Ext>
	struct SP_convertible<Yty, Uty[Ext]> : std::is_convertible<Yty(*)[Ext], Uty(*)[Ext]>::type
	{};

	// TYPE TRAIT _SP_pointer_compatible
	template<class Yty, class Ty>
	struct SP_pointer_compatible : std::is_convertible<Yty*, Ty*>::type
	{	// N4659 [util.smartptr.shared]/5 "a pointer type Y* is said to be compatible with a pointer type T* "
		// "when either Y* is convertible to T* ..."
	};
	template<class Uty, size_t Ext>
	struct SP_pointer_compatible<Uty[Ext], Uty[]> : std::true_type
	{	// N4659 [util.smartptr.shared]/5 "... or Y is U[N] and T is cv U[]."
	};
	template<class Uty, size_t Ext>
	struct SP_pointer_compatible<Uty[Ext], const Uty[]> : std::true_type
	{	// N4659 [util.smartptr.shared]/5 "... or Y is U[N] and T is cv U[]."
	};
	template<class Uty, size_t Ext>
	struct SP_pointer_compatible<Uty[Ext], volatile Uty[]> : std::true_type
	{	// N4659 [util.smartptr.shared]/5 "... or Y is U[N] and T is cv U[]."
	};
	template<class Uty, size_t Ext>
	struct SP_pointer_compatible<Uty[Ext], const volatile Uty[]> : std::true_type
	{	// N4659 [util.smartptr.shared]/5 "... or Y is U[N] and T is cv U[]."
	};
}