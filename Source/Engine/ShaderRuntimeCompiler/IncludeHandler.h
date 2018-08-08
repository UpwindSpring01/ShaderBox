#pragma once

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <string>
typedef void(*IncludeCallback)(const char* fileName);


class IncludeHandler : public ID3DInclude
{
public:
	IncludeHandler(const char* shaderLocation, IncludeCallback includeDelegate);
	HRESULT Close(LPCVOID pData) override;

	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData,
		LPCVOID *ppData, UINT *pBytes) override;

private:
	const char* m_ShaderDir;
	IncludeCallback _includeDelegate;
};
