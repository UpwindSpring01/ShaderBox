#pragma once
#include "stdafx.h"

#include <type_traits>

namespace SpringWindEngine
{
	// CLASS TEMPLATE CompressedPair
	struct _Zero_then_variadic_args_t
	{	// tag type for value-initializing first,
	};	// constructing second from remaining args

	struct _One_then_variadic_args_t
	{	// tag type for constructing first from one arg,
	};	// constructing second from remaining args

	template<class T1, class T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
	class CompressedPair final : private T1
	{	// store a pair of values, deriving from empty first
	private:
		T2 m_Value2;

	public:
		template<class... _Other2>
		constexpr explicit CompressedPair(_Zero_then_variadic_args_t, _Other2&&... _Val2)
			: T1(), m_Value2(std::forward<_Other2>(_Val2)...)
		{	// construct from forwarded values
		}

		template<class _Other1, class... _Other2>
		CompressedPair(_One_then_variadic_args_t, _Other1&& _Val1, _Other2&&... _Val2)
			: T1(std::forward<_Other1>(_Val1)), m_Value2(std::forward<_Other2>(_Val2)...)
		{	// construct from forwarded values
		}

		T1& GetFirst() noexcept
		{	// return reference to first
			return (*this);
		}

		const T1& GetFirst() const noexcept
		{
			return (*this);
		}

		T2& GetSecond() noexcept
		{
			return (m_Value2);
		}

		const T2& GetSecond() const noexcept
		{
			return (m_Value2);
		}
	};

	template<class T1, class T2>
	class CompressedPair<T1, T2, false> final
	{	// store a pair of values, not deriving from first
	private:
		T1 m_Value1;
		T2 m_Value2;

	public:
		template<class... _Other2>
		constexpr explicit CompressedPair(_Zero_then_variadic_args_t, _Other2&&... _Val2)
			: m_Value1(), m_Value2(std::forward<_Other2>(_Val2)...)
		{	// construct from forwarded values
		}

		template<class _Other1, class... _Other2>
		CompressedPair(_One_then_variadic_args_t, _Other1&& _Val1, _Other2&&... _Val2)
			: m_Value1(std::forward<_Other1>(_Val1)),
			m_Value2(std::forward<_Other2>(_Val2)...)
		{	// construct from forwarded values
		}

		T1& GetFirst() noexcept
		{
			return (m_Value1);
		}

		const T1& GetFirst() const noexcept
		{
			return (m_Value1);
		}

		T2& GetSecond() noexcept
		{
			return (m_Value2);
		}

		const T2& GetSecond() const noexcept
		{
			return (m_Value2);
		}
	};
}