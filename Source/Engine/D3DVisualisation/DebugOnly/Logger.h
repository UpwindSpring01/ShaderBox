#pragma once

class Logger final
{
public:
	enum class LogLevel
	{
		Info = 0x1,
		Warning = 0x2,
		Error = 0x4,
		FixMe = 0x8
	};
	static void LogHRESULT(HRESULT hr, const std::wstring& origin);
	static void LogError(const std::wstring& msg);
	static void LogFormat(const LogLevel level, const wchar_t* format, ...);
	static void Log(const LogLevel level, const std::wstring& msg);
private:
	Logger() {};
	~Logger() {};

	static wchar_t* m_ConvertBuffer;
	static const size_t m_ConvertBufferSize = 1024;

	static void CreateConsole();
	static HANDLE m_ConsoleHandle;
};