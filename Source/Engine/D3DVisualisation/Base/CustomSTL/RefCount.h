#pragma once

#include "Base/stdafx.h"
#include "CompressedPair.h"

namespace SpringWindEngine
{
	// ALIAS TEMPLATE Rebind_alloc_t
	template<class Alloc, class Value_type>
	using Rebind_alloc_t = typename std::allocator_traits<Alloc>::template rebind_alloc<Value_type>;

	// FUNCTION TEMPLATE Deallocate_plain
	template<class Alloc, std::enable_if_t<std::is_same_v<typename std::allocator_traits<Alloc>::pointer, typename Alloc::value_type*>, int> = 0>
	inline void Deallocate_plain(Alloc& _Al, typename Alloc::value_type* const _Ptr)
	{	// deallocate a plain pointer using an allocator, non-fancy pointers special case
		std::allocator_traits<Alloc>::deallocate(_Al, _Ptr, 1);
	}

	template<class Alloc, std::enable_if_t<!std::is_same_v<typename std::allocator_traits<Alloc>::pointer, typename Alloc::value_type*>, int> = 0>
	inline void Deallocate_plain(Alloc& _Al, typename Alloc::value_type* const _Ptr)
	{	// deallocate a plain pointer using an allocator
		using _Alloc_traits = std::allocator_traits<Alloc>;
		using _Ptr_traits = std::pointer_traits<typename Alloc_traits::pointer>;
		_Alloc_traits::deallocate(_Al, _Ptr_traits::pointer_to(*_Ptr), 1);
	}

	// CLASS RefCountBase
	class __declspec(novtable) RefCountBase
	{
	private:
		virtual void Destroy() noexcept = 0;

		std::size_t m_Count;
	protected:
		RefCountBase() : m_Count(1) {}
	public:
		void IncRef() noexcept
		{
			++m_Count;
		}

		void DecRef()
		{
			if (--m_Count == 0)
			{
				Destroy();
			}
		}

		std::size_t UseCount() const noexcept
		{
			return m_Count;
		}
	};

	// CLASS TEMPLATE RefCount
	template<typename T>
	class RefCount : public RefCountBase
	{
	public:
		explicit RefCount(T* _Px)
			: RefCountBase(), m_Ptr(_Px)
		{	// construct
		}
	private:
		virtual void Destroy() noexcept override
		{
			delete m_Ptr;
			delete this;
		}

		T* m_Ptr;
	};

	// CLASS TEMPLATE RefCount
	template<typename T>
	class RefCountIUnknown : public RefCountBase
	{
	public:
		explicit RefCountIUnknown(T* _Px)
			: RefCountBase(), m_Ptr(_Px)
		{	// construct
		}
	private:
		virtual void Destroy() noexcept override
		{
			m_Ptr->Release();
			delete this;
		}

		T* m_Ptr;
	};


	// CLASS TEMPLATE RefCountResource
	template<class T, class D>
	class RefCountResource : public RefCountBase
	{
	public:
		RefCountResource(T _Px, D _Dt)
			: RefCountBase(), m_MyPair(_One_then_variadic_args_t(), std::move(_Dt), _Px)
		{	// construct
		}

	private:
		virtual void Destroy() noexcept override
		{
			m_MyPair.GetFirst()(m_MyPair.GetSecond());
			delete this;
		}

		CompressedPair<D, T> m_MyPair;
	};

	// CLASS TEMPLATE RefCountObj
	template<class T>
	class RefCountObj : public RefCountBase
	{	// handle reference counting for object in control block, no allocator
	public:
		template<class... Types>
		explicit RefCountObj(Types&&... _Args)
			: RefCountBase()
		{	// construct from argument list
			::new (static_cast<void *>(&m_Storage)) T(std::forward<Types>(_Args)...);
		}

		T* GetPtr()
		{	// get pointer
			return (reinterpret_cast<T*>(&m_Storage));
		}

	private:
		virtual void Destroy() noexcept override
		{	// destroy managed resource
			GetPtr()->~T();
			delete this;
		}

		std::aligned_union_t<1, T> m_Storage;
	};

	// CLASS TEMPLATE RefCountPairIUknownObj
	template<class T>
	class RefCountPairIUknownObj : public RefCountBase
	{	// handle reference counting for object in control block, no allocator
	public:
		template<class... Types>
		explicit RefCountPairIUknownObj(Types&&... _Args)
			: RefCountBase()
		{	// construct from argument list
			::new (static_cast<void *>(&m_Storage)) T(std::forward<Types>(_Args)...);
		}

		T* GetPtr()
		{	// get pointer
			return (reinterpret_cast<T*>(&m_Storage));
		}

	private:
		virtual void Destroy() noexcept override
		{	// destroy managed resource
			GetPtr()->first->Release();
			delete this;
		}

		std::aligned_union_t<1, T> m_Storage;
	};

	// CLASS TEMPLATE RefCountResourceAlloc
	template<class R, class D, class Alloc>
	class RefCountResourceAlloc : public RefCountBase
	{	// handle reference counting for object with deleter and allocator
	public:
		RefCountResourceAlloc(R _Px, D _Dt, const Alloc& _Ax)
			: RefCountBase(), m_Mypair(_One_then_variadic_args_t(), std::move(_Dt), _One_then_variadic_args_t(), _Ax, _Px)
		{	// construct
		}

	private:
		using _Myalty = Rebind_alloc_t<Alloc, RefCountResourceAlloc>;

		virtual void Destroy() noexcept override
		{
			m_Mypair.GetFirst()(m_Mypair.GetSecond().GetSecond());

			_Myalty _Al = m_Mypair.GetSecond().GetFirst();
			std::allocator_traits<_Myalty>::destroy(_Al, this);
			Deallocate_plain(_Al, this);
		}

		CompressedPair<D, CompressedPair<_Myalty, R>> m_Mypair;
	};

	// CLASS TEMPLATE _Ref_count_obj_alloc
	template<class T, class Alloc>
	class RefCountObjAlloc : public RefCountBase
	{	// handle reference counting for object in control block, allocator
	public:
		template<class... Types>
		explicit RefCountObjAlloc(const Alloc& _Al_arg, Types&&... _Args)
			: RefCountBase(), m_Mypair(_One_then_variadic_args_t(), _Al_arg)
		{	// construct from argument list, allocator
			::new (static_cast<void *>(&m_Mypair.GetSecond())) T(std::forward<Types>(_Args)...);
		}

		T* GetPtr()
		{	// get pointer
			return (reinterpret_cast<T*>(&m_Mypair.GetSecond()));
		}

	private:
		using _Myalty = Rebind_alloc_t<Alloc, RefCountObjAlloc>;
		using _Mystoragety = std::aligned_union_t<1, T>;

		virtual void Destroy() noexcept override
		{	
			GetPtr()->~T();

			_Myalty _Al = m_Mypair.GetFirst();
			std::allocator_traits<_Myalty>::destroy(_Al, this);
			Deallocate_plain(_Al, this);
		}

		CompressedPair<_Myalty, _Mystoragety> m_Mypair;
	};
}