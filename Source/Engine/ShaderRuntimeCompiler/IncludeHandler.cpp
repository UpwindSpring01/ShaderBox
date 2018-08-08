#include "IncludeHandler.h"
#include <fstream>

IncludeHandler::IncludeHandler(const char* shaderLocation, IncludeCallback includeDelegate)
{
	m_ShaderDir = shaderLocation;
	_includeDelegate = includeDelegate;
}

HRESULT __stdcall IncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	if (IncludeType == D3D_INCLUDE_LOCAL)
	{
		std::ifstream includeFile;
		try {
			// Ask shaderBox to update the file on disk
			_includeDelegate(pFileName);

			std::string finalPath(m_ShaderDir);
			finalPath += "/";
			finalPath += pFileName;
			includeFile.open(finalPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

			if (includeFile.is_open())
			{
				std::streampos fileSize = includeFile.tellg();
				char* buf = new char[fileSize];
				includeFile.seekg(0, std::ios::beg);
				includeFile.read(buf, fileSize);
				includeFile.close();
				*ppData = buf;
				*pBytes = (UINT)fileSize;
			}
			else
			{
				return E_FAIL;
			}
			return S_OK;
		}
		catch (std::exception&)
		{
			includeFile.close();
			return E_FAIL;
		}

	}
	else
	{
		return E_FAIL;
	}

}

HRESULT __stdcall IncludeHandler::Close(LPCVOID pData) {
	char* buf = (char*)pData;
	delete[] buf;
	return S_OK;
}