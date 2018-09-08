#pragma once

namespace SpringWindEngine
{
	class ThreadSafeFunctionContainer
	{
	public:
		void Push(std::function<void()> value);
		void ExecuteAll();
	private:
		std::mutex m_Mutex;

		std::vector<std::function<void()>> m_Container;
	};
}