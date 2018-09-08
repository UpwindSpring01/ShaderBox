#pragma once
#include "Base/stdafx.h"

#include "RefCount.h"
#include "TypeTraits.h"

namespace SpringWindEngine
{
	// No atomic support
	// No weak support
	// No shared from this support
	// No constructors taking unique ptr

	template<class T>
	class Unsafe_Shared_Ptr;

	// FUNCTION TEMPLATE _Unfancy
	template<class P>
	inline auto Unfancy(P _Ptr)
	{	// converts from a fancy pointer to a plain pointer
		return (std::addressof(*_Ptr));
	}

	template<class T>
	inline T* Unfancy(T* _Ptr)
	{	// do nothing for plain pointers
		return (_Ptr);
	}

	template<class T>
	class Unsafe_Ptr_Base
	{
	public:
		using element_type = std::remove_extent_t<T>;

		[[nodiscard]] std::size_t UseCount() const noexcept
		{	// return use count
			return (m_Rep ? m_Rep->UseCount() : 0);
		}

		Unsafe_Ptr_Base(const Unsafe_Ptr_Base&) = delete;
		Unsafe_Ptr_Base& operator=(const Unsafe_Ptr_Base&) = delete;

	protected:
		[[nodiscard]] element_type* Get() const noexcept
		{	// return pointer to resource
			return (m_Ptr);
		}

		constexpr Unsafe_Ptr_Base() noexcept = default;

		~Unsafe_Ptr_Base() = default;

		template<class T2>
		void Move_construct_from(Unsafe_Ptr_Base<T2>&& _Right)
		{	// implement shared_ptr's (converting) move ctor and weak_ptr's move ctor
			m_Ptr = _Right.m_Ptr;
			m_Rep = _Right.m_Rep;

			_Right.m_Ptr = nullptr;
			_Right.m_Rep = nullptr;
		}

		template<class T2>
		void Copy_construct_from(const Unsafe_Shared_Ptr<T2>& _Other)
		{	// implement shared_ptr's (converting) copy ctor
			if (_Other.m_Rep)
			{
				_Other.m_Rep->IncRef();
			}

			m_Ptr = _Other.m_Ptr;
			m_Rep = _Other.m_Rep;
		}

		template<class T2>
		void Alias_construct_from(const Unsafe_Shared_Ptr<T2>& _Other, element_type* _Px)
		{	// implement shared_ptr's aliasing ctor
			if (_Other.m_Rep)
			{
				_Other.m_Rep->IncRef();
			}

			m_Ptr = _Px;
			m_Rep = _Other.m_Rep;
		}

		void DecRef()
		{	// decrement reference count
			if (m_Rep)
			{
				m_Rep->DecRef();
			}
		}

		void Swap(Unsafe_Ptr_Base& _Right) noexcept
		{	// swap pointers
			std::swap(m_Ptr, _Right.m_Ptr);
			std::swap(m_Rep, _Right.m_Rep);
		}

		void Set_ptr_rep(element_type* _Other_ptr, RefCountBase* _Other_rep)
		{	// take new resource
			m_Ptr = _Other_ptr;
			m_Rep = _Other_rep;
		}
	private:
		element_type* m_Ptr{ nullptr };
		RefCountBase* m_Rep{ nullptr };

		template<class T0>
		friend class Unsafe_Ptr_Base;
	};

	template<typename T>
	class Unsafe_Shared_Ptr : public Unsafe_Ptr_Base<T>
	{
	private:
		using MyBase = Unsafe_Ptr_Base<T>;
	public:
		using typename MyBase::element_type;

		constexpr Unsafe_Shared_Ptr() noexcept {}

		constexpr Unsafe_Shared_Ptr(std::nullptr_t) noexcept {}

		template<class U, std::enable_if_t<std::conjunction_v<SP_convertible<U, T>, SP_convertible<T, IUnknown>>, int> = 0>
		explicit Unsafe_Shared_Ptr(U* _Px)
		{	// construct shared_ptr object that owns _Px
			SetIUknownPtr(_Px);
		}

		template<class U, std::enable_if_t<std::conjunction_v<std::conditional_t<std::is_array_v<T>, Can_array_delete<U>, Can_scalar_delete<U>>, SP_convertible<U, T>, std::negation<SP_convertible<T, IUnknown>>>, int> = 0>
		explicit Unsafe_Shared_Ptr(U* _Px)
		{	// construct shared_ptr object that owns _Px
			SetPtr(_Px, std::is_array<T>{});
		}

		template<class U, class D, std::enable_if_t<std::conjunction_v<std::is_move_constructible<D>, Can_call_function_object<D&, U*&>, SP_convertible<U, T>>, int> = 0>
		Unsafe_Shared_Ptr(U* _Px, D _Dt)
		{	// construct with _Px, deleter
			SetPtrAndDeleter(_Px, std::move(_Dt));
		}

		template<class U, class D, class Alloc, std::enable_if_t<std::conjunction_v<std::is_move_constructible<D>, Can_call_function_object<D&, U*&>, SP_convertible<U, T>>, int> = 0>
		Unsafe_Shared_Ptr(U* _Px, D _Dt, Alloc _Ax)
		{	// construct with _Px, deleter, allocator
			SetPtrDeleterAndAlloc(_Px, std::move(_Dt), _Ax);
		}

		template<class D, std::enable_if_t<std::conjunction_v<std::is_move_constructible<D>, Can_call_function_object<D&, std::nullptr_t&>>, int> = 0>
		Unsafe_Shared_Ptr(std::nullptr_t, D _Dt)
		{	// construct with nullptr, deleter
			SetPtrAndDeleter(nullptr, std::move(_Dt));
		}

		template<class D, class Alloc, std::enable_if_t<std::conjunction_v<std::is_move_constructible<D>, Can_call_function_object<D&, std::nullptr_t&>>, int> = 0>
		Unsafe_Shared_Ptr(std::nullptr_t, D _Dt, Alloc _Ax)
		{	// construct with nullptr, deleter, allocator
			SetPtrDeleterAndAlloc(nullptr, std::move(_Dt), _Ax);
		}

		template<class T2>
		Unsafe_Shared_Ptr(const Unsafe_Shared_Ptr<T2>& _Right, element_type* _Px) noexcept
		{	// construct shared_ptr object that aliases _Right
			this->Alias_construct_from(_Right, _Px);
		}

		Unsafe_Shared_Ptr(const Unsafe_Shared_Ptr& _Other) noexcept
		{	// construct shared_ptr object that owns same resource as _Other
			this->Copy_construct_from(_Other);
		}

		template<class T2, std::enable_if_t<SP_pointer_compatible<T2, T>::value, int> = 0>
		Unsafe_Shared_Ptr(const Unsafe_Shared_Ptr<T2>& _Other) noexcept
		{	// construct shared_ptr object that owns same resource as _Other
			this->Copy_construct_from(_Other);
		}

		Unsafe_Shared_Ptr(Unsafe_Shared_Ptr&& _Right) noexcept
		{	// construct shared_ptr object that takes resource from _Right
			this->Move_construct_from(std::move(_Right));
		}

		template<class T2, std::enable_if_t<SP_pointer_compatible<T2, T>::value, int> = 0>
		Unsafe_Shared_Ptr(Unsafe_Shared_Ptr<T2>&& _Right) noexcept
		{	// construct shared_ptr object that takes resource from _Right
			this->Move_construct_from(std::move(_Right));
		}

		~Unsafe_Shared_Ptr() noexcept
		{
			this->DecRef();
		}

		Unsafe_Shared_Ptr& operator=(const Unsafe_Shared_Ptr& _Right) noexcept
		{	// assign shared ownership of resource owned by _Right
			Unsafe_Shared_Ptr(_Right).Swap(*this);
			return (*this);
		}

		template<class T2>
		Unsafe_Shared_Ptr& operator=(const Unsafe_Shared_Ptr<T2>& _Right) noexcept
		{	// assign shared ownership of resource owned by _Right
			Unsafe_Shared_Ptr(_Right).Swap(*this);
			return (*this);
		}

		Unsafe_Shared_Ptr& operator=(Unsafe_Shared_Ptr&& _Right) noexcept
		{	// take resource from _Right
			Unsafe_Shared_Ptr(std::move(_Right)).Swap(*this);
			return (*this);
		}

		template<class T2>
		Unsafe_Shared_Ptr& operator=(Unsafe_Shared_Ptr<T2>&& _Right) noexcept
		{	// take resource from _Right
			Unsafe_Shared_Ptr(std::move(_Right)).Swap(*this);
			return (*this);
		}

		void reset() noexcept
		{	// release resource and convert to empty shared_ptr object
			Unsafe_Shared_Ptr().Swap(*this);
		}

		template<class U>
		void Reset(U* _Px)
		{	// release, take ownership of _Px
			Unsafe_Shared_Ptr(_Px).Swap(*this);
		}

		template<class U, class D>
		void Reset(U* _Px, D _Dt)
		{	// release, take ownership of _Px, with deleter _Dt
			Unsafe_Shared_Ptr(_Px, _Dt).Swap(*this);
		}

		template<class U, class D, class Alloc>
		void Reset(U* _Px, D _Dt, Alloc _Ax)
		{	// release, take ownership of _Px, with deleter _Dt, allocator _Ax
			Unsafe_Shared_Ptr(_Px, _Dt, _Ax).Swap(*this);
		}

		using MyBase::Get;

		template<class T2 = T, std::enable_if_t<!std::disjunction_v<std::is_array<T2>, std::is_void<T2>>, int> = 0>
		[[nodiscard]] T2& operator*() const noexcept
		{	// return reference to resource
			return (*Get());
		}

		template<class T2 = T, std::enable_if_t<!std::is_array_v<T2>, int> = 0>
		[[nodiscard]] T2* operator->() const noexcept
		{	// return pointer to resource
			return (Get());
		}

		template<class T2 = T, class _Elem = element_type, std::enable_if_t<std::is_array_v<T2>, int> = 0>
		[[nodiscard]] _Elem& operator[](std::ptrdiff_t _Idx) const
		{	// subscript
			return (Get()[_Idx]);
		}

		explicit operator bool() const noexcept
		{	// test if shared_ptr object owns a resource
			return (Get() != nullptr);
		}
	private:
		template<class U>
		void SetPtr(U* _Px, std::true_type)
		{	// take ownership of _Px
			SetPtrAndDeleter(_Px, std::default_delete<U[]>{});
		}

		template<class U>
		void SetPtr(U* _Px, std::false_type)
		{	// take ownership of _Px
			try
			{
				this->Set_ptr_rep(_Px, new RefCount<U>(_Px));
			}
			catch (...)
			{
				delete _Px;
				throw;
			}
		}

		template<class U>
		void SetIUknownPtr(U* _Px)
		{	// take ownership of _Px
			try
			{
				this->Set_ptr_rep(_Px, new RefCountIUnknown<U>(_Px));
			}
			catch (...)
			{
				_Px->Release();
				throw;
			}
		}

		template<class UptrOrNullptr, class D>
		void SetPtrAndDeleter(UptrOrNullptr _Px, D _Dt)
		{	// take ownership of _Px, deleter _Dt
			try
			{
				this->Set_ptr_rep(_Px, new RefCountResource<UptrOrNullptr, D>(_Px, std::move(_Dt)));
			}
			catch (...)
			{
				_Dt(_Px);
				throw;
			}
		}

		template<class UptrOrNullptr, class D, class Alloc>
		void SetPtrDeleterAndAlloc(UptrOrNullptr _Px, D _Dt, Alloc _Ax)
		{	// take ownership of _Px, deleter _Dt, allocator _Ax
			using _Refd = RefCountResourceAlloc<UptrOrNullptr, D, Alloc>;
			using _Alref_alloc = Rebind_alloc_t<Alloc, _Refd>;
			using _Alref_traits = std::allocator_traits<_Alref_alloc>;
			_Alref_alloc _Alref(_Ax);

			try
			{
				// allocate control block and set
				const auto _Pfancy = _Alref_traits::allocate(_Alref, 1);
				_Refd* const _Pref = Unfancy(_Pfancy);
				try
				{
					_Alref_traits::construct(_Alref, _Pref, _Px, std::move(_Dt), _Ax);
					Set_ptr_rep(_Px, _Pref);
				}
				catch (...)
				{
					_Alref_traits::deallocate(_Alref, _Pfancy, 1);
					throw;
				}
			}
			catch (...)
			{
				_Dt(_Px);
				throw;
			}
		}

		template<class T0, class... Types>
		friend Unsafe_Shared_Ptr<T0> Unsafe_Make_Shared(Types&&... _Args);

		template<class T0, class T1, class... Types, std::enable_if_t<std::conjunction_v<SP_convertible<T0, IUnknown>>, int> = 0>
		friend Unsafe_Shared_Ptr<std::pair<T0*, T1>> Unsafe_Make_Shared_Pair(Types&&... _Args);

		template<class T0, class Alloc, class... Types>
		friend Unsafe_Shared_Ptr<T0> Unsafe_Allocate_Shared(const Alloc& _Al_arg, Types&&... _Args);
	};

	// FUNCTION TEMPLATE Unsafe_Make_Shared
	template<class T, class... Types>
	[[nodiscard]] inline Unsafe_Shared_Ptr<T> Unsafe_Make_Shared(Types&&... _Args)
	{
		const auto _Rx = new RefCountObj<T>(std::forward<Types>(_Args)...);

		Unsafe_Shared_Ptr<T> _Ret;
		_Ret.Set_ptr_rep(_Rx->GetPtr(), _Rx);
		return (_Ret);
	}

	// FUNCTION TEMPLATE Unsafe_Make_Shared_Pair
	template<class T0, class T1, class... Types, std::enable_if_t<std::conjunction_v<SP_convertible<T0, IUnknown>>, int> = 0>
	[[nodiscard]] inline Unsafe_Shared_Ptr<std::pair<T0*, T1>> Unsafe_Make_Shared_Pair(Types&&... _Args)
	{
		const auto _Rx = new RefCountPairIUknownObj<std::pair<T0*, T1>>(std::forward<Types>(_Args)...);

		Unsafe_Shared_Ptr<std::pair<T0*, T1>> _Ret;
		_Ret.Set_ptr_rep(_Rx->GetPtr(), _Rx);
		return (_Ret);
	}

	// FUNCTION TEMPLATE Unsafe_Allocate_Shared
	template<class T, class Alloc, class... Types>
	[[nodiscard]]  inline Unsafe_Shared_Ptr<T> Unsafe_Allocate_Shared(const Alloc& _Al_arg, Types&&... _Args)
	{	// make a shared_ptr
		using _Refoa = RefCountObjAlloc<T, Alloc>;
		using _Alref_alloc = Rebind_alloc_t<Alloc, _Refoa>;
		using _Alref_traits = std::allocator_traits<_Alref_alloc>;
		_Alref_alloc _Alref(_Al_arg);

		const auto _Rx = _Alref_traits::allocate(_Alref, 1);

		try
		{
			_Alref_traits::construct(_Alref, Unfancy(_Rx), _Al_arg, std::forward<Types>(_Args)...);
		}
		catch (...)
		{
			_Alref_traits::deallocate(_Alref, _Rx, 1);
			throw;
		}

		Unsafe_Shared_Ptr<T> _Ret;
		_Ret.Set_ptr_rep(_Rx->GetPtr(), Unfancy(_Rx));
		return (_Ret);
	}

	template<class Ty>
	[[nodiscard]] bool operator==(const Unsafe_Shared_Ptr<Ty>& _Left, std::nullptr_t) noexcept
	{
		return (_Left.Get() == nullptr);
	}

	template<class Ty>
	[[nodiscard]] bool operator==(std::nullptr_t, const Unsafe_Shared_Ptr<Ty>& _Right) noexcept
	{
		return (nullptr == _Right.Get());
	}

	template<class Ty>
	[[nodiscard]] bool operator!=(const Unsafe_Shared_Ptr<Ty>& _Left, std::nullptr_t) noexcept
	{
		return (_Left.Get() != nullptr);
	}

	template<class Ty>
	[[nodiscard]] bool operator!=(std::nullptr_t, const Unsafe_Shared_Ptr<Ty>& _Right) noexcept
	{
		return (nullptr != _Right.Get());
	}
}