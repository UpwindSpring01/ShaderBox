#include "ShaderRTCompiler.h"
#include <memory>
#include "IncludeHandler.h"
#include <filesystem>
#include <fstream>

char* CompileShader(const char* content, const char* shaderTarget, const char* shaderLocation, const bool saveToDisk, IncludeCallback includeDelegate)
{
	std::unique_ptr<IncludeHandler> includeHandler = std::make_unique<IncludeHandler>(shaderLocation, includeDelegate);
	ID3DBlob* pBlob;
	ID3DBlob* pErrorBlob;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh446869(v=vs.85).aspx
	HRESULT hr = D3DCompile2(content, strlen(content), nullptr, nullptr, includeHandler.get(), "main", shaderTarget, 0, 0, 0, nullptr, 0, &pBlob, &pErrorBlob);

	char* errors = nullptr;

	if (FAILED(hr))
	{
		size_t errorSize = 0;
		if (pErrorBlob != nullptr)
		{
			char *errorsBuffer = (char*)pErrorBlob->GetBufferPointer();

			errorSize = pErrorBlob->GetBufferSize();
			errors = new char[errorSize];
			for (size_t i = 0; i < errorSize; ++i)
			{
				errors[i] = errorsBuffer[i];
			}

			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}
		else
		{
			std::string errorMsg = "Failed to load shader with target: " + std::string(shaderTarget);
			errorSize = errorMsg.length();
			errors = new char[errorSize];
			for (size_t i = 0; i < errorSize; ++i)
			{
				errors[i] = errorMsg[i];
			}
		}
	}

	if (saveToDisk && errors == nullptr)
	{
		std::string path = std::string(shaderLocation) + "/.cso";
		if (!std::experimental::filesystem::exists(path))
		{
			std::experimental::filesystem::create_directory(path);
		}
		path += "/" + std::string(shaderTarget).substr(0, 2) + ".cso";
		
		std::ofstream file(path);
		file.close();

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(path);

		HRESULT hr = D3DWriteBlobToFile(pBlob, wide.c_str(), true);
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"Failed to save compiled shader to disk.\nIf this keeps occuring contact a developer";
			MessageBox(nullptr, errorMsg.c_str(), L"FAIL", MB_ICONERROR | MB_OK);
		}
	}

	if (pBlob != nullptr)
	{
		pBlob->Release();
		pBlob = nullptr;
	}

	return errors;
	//pathToCompiledShader = "";
}

void DeleteArray(void*& chars)
{
	delete[] chars;
	chars = nullptr;
}

BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD /*fwdReason*/, LPVOID /*lpvReserved*/)
{
	return TRUE;
}