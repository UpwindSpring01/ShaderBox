#include "stdafx.h"
#include "Logger.h"

#include <sstream>
#include <iostream>

HANDLE SpringWindEngine::Logger::m_ConsoleHandle = nullptr;
wchar_t* SpringWindEngine::Logger::m_ConvertBuffer = new wchar_t[m_ConvertBufferSize];

void SpringWindEngine::Logger::LogHRESULT(HRESULT hr, const std::wstring& origin)
{
	if(FAILED(hr))
	{
		if(FACILITY_WINDOWS == HRESULT_FACILITY(hr))
		{
			hr = HRESULT_CODE(hr);
		}
		std::wstringstream ss;
		if(origin.size() != 0)
		{
			ss << L">ORIGIN:\n";
			ss << origin;
			ss << L"\n\n";
		}
		ss << L"ERROR:\n";

		LPWSTR errorMsg = nullptr;
		if(FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			errorMsg, 0, NULL) != 0)
		{
			ss << errorMsg;
		}
		else
		{
			ss << L"Could not find a description for error: ";
			ss << hr;
		}

		Log(LogLevel::Error, ss.str());
	}
}

void SpringWindEngine::Logger::LogError(const std::wstring& origin)
{
	Log(LogLevel::Error, origin);
}

void SpringWindEngine::Logger::LogFormat(const LogLevel level, const wchar_t* format, ...)
{
	va_list ap;
	va_start(ap, format);
	_vsnwprintf_s(&m_ConvertBuffer[0], m_ConvertBufferSize, m_ConvertBufferSize, format, ap);
	va_end(ap);
	Log(level, std::wstring(&m_ConvertBuffer[0]));
}

void SpringWindEngine::Logger::Log(const LogLevel level, const std::wstring& msg)
{
	if(m_ConsoleHandle == nullptr)
	{
		CreateConsole();
	}

	std::wstringstream stream;
	switch(level)
	{
	case LogLevel::Info:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		stream << L"[INFO]    ";
		break;
	case LogLevel::Warning:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		stream << L"[WARNING] ";
		break;
	case LogLevel::Error:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED);
		stream << L"[ERROR]   ";
		break;
	case LogLevel::FixMe:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
		stream << L"[FIX-ME]   ";
		break;
	}

	stream << msg;
	stream << L"\n";

	std::wcout << stream.str();

	if(level == LogLevel::Error)
	{
		MessageBox(0, msg.c_str(), L"ERROR", 0);
		__debugbreak();
	}
}

void SpringWindEngine::Logger::CreateConsole()
{
	if(AllocConsole())
	{
		FILE* pCout;
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);

		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();

		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		//Disable Close-Button
		HWND hwnd = GetConsoleWindow();
		if(hwnd != NULL)
		{
			HMENU hMenu = GetSystemMenu(hwnd, FALSE);
			if(hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
}
