#pragma once

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <sstream>

// ShaderBox will save the file so an up-to-date version can be read by the compiler
typedef void(*IncludeCallback)(const char* fileName);

extern "C" {
	__declspec(dllexport) char* __cdecl CompileShader(const char* content, const char* shaderTarget, const char* shaderLocation, const bool saveToDisk, IncludeCallback includeDelegate);
	__declspec(dllexport) void __cdecl DeleteArray(void*& chars);


}

