#include "stdafx.h"
#include "ThreadSafeFunctionContainer.h"

void SpringWindEngine::ThreadSafeFunctionContainer::Push(std::function<void()> value)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_Container.emplace_back(value);
}

void SpringWindEngine::ThreadSafeFunctionContainer::ExecuteAll()
{
	std::vector<std::function<void()>> container;
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		container = m_Container;
		m_Container.clear();
	}
	for (const std::function<void()>& func : container)
	{
		func();
	}
}
