#pragma once
namespace SpringWindEngine
{
	template<class T>
	class Singleton
	{
	public:
		static T* GetInstance()
		{
			return m_pInstance;
		};

		static void CreateInstance()
		{
			assert(m_pInstance == nullptr);
			m_pInstance = new T();
		}

		static void DestroyInstance()
		{
			assert(m_pInstance != nullptr);
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	private:
		static T* m_pInstance;
	};

	template<class T>
	T* Singleton<T>::m_pInstance = nullptr;
}