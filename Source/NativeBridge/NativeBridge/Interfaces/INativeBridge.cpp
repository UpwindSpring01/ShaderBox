#include "stdafx.h"

#include "INativeBridge.h"
#include "../NativeBridge.h"
#include "Base/SpringWindWindow.h"

ShaderBoxNativeBridge::INativeBridge* ShaderBoxNativeBridge::INativeBridge::m_pNativeBridge = nullptr;

ShaderBoxNativeBridge::INativeBridge* ShaderBoxNativeBridge::INativeBridge::CreateNativeBridge()
{
	m_pNativeBridge = new NativeBridge();
	return m_pNativeBridge;
}

void ShaderBoxNativeBridge::INativeBridge::DestroyNativeBridge()
{
	delete m_pNativeBridge;
}

ShaderBoxNativeBridge::INativeBridge* ShaderBoxNativeBridge::INativeBridge::GetNativeBridge()
{
	return m_pNativeBridge;
}