#include "stdafx.h"
#include "NativeWindowBridge.h"
#include "NativeBridge.h"

#include "Base/SpringWindWindow.h"
#include "SceneGraph/SceneManager.h"
#include "Content/Shaders/EffectFactory.h"
#include "Content/Shaders/ShaderStorage.h"
#include "Content/MeshLoader.h"

#include "ShaderBoxMaterial.h"
#include "ShaderBoxPPMaterial.h"

using namespace SpringWindEngine;

ShaderBoxNativeBridge::NativeWindowBridge::NativeWindowBridge(HWND parentHandle, REGISTER_WINDOW_CALLBACK callback)
{
	m_pWindow = new SpringWindWindow();

	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, parentHandle, callback]
	{
		GameObject* pCamera = new GameObject();
		m_pCamComponent = new CameraComponent();
		pCamera->AddComponent(m_pCamComponent);

		GameScene* pScene = SceneManager::GetInstance()->CreateScene();
		pScene->AddChild(pCamera);
		pScene->SetActiveCamera(m_pCamComponent);

		GameObject* obj = new GameObject();
		m_pMeshDrawComponent = new MeshDrawComponent();

		Unsafe_Shared_Ptr<Mesh> pMesh = ContentManager::Load<Mesh>(L"./Resources/.Internal/Models/cube.obj");

		Effect* pEffect = EffectFactory::GetInstance()->LoadEffect(L"./Resources/.Internal/CSO/vs.cso", L"./Resources/.Internal/CSO/ps.cso");
		pEffect->Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		m_pDefaultMaterial = new Material(pEffect);

		m_pMeshDrawComponent->SetMeshAndMaterial(pMesh, m_pDefaultMaterial);

		obj->AddComponent(m_pMeshDrawComponent);
		pScene->AddChild(obj);

		XMVECTOR worldPosition = XMVectorSet(0, 5, -5, 0);

		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
		XMVECTOR eyeDir = XMVectorSet(0, -5, 5, 0);

		XMFLOAT4 rot;
		XMStoreFloat4(&rot, XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, XMMatrixLookToLH(worldPosition, eyeDir, upVec))));

		pCamera->GetTransform()->Translation(0, 5, -5);
		pCamera->GetTransform()->Rotation(rot);

		m_pWindow->SetScene(pScene);
		SpringWind::GetInstance()->RegisterWindow(m_pWindow, GetModuleHandle(nullptr), parentHandle, nullptr);

		callback();
	});
}

ShaderBoxNativeBridge::NativeWindowBridge::~NativeWindowBridge()
{
	delete m_pActiveMaterial;
	delete m_pActivePPMaterial;
	delete m_pDefaultMaterial;

	delete m_pWindow;
}

HWND ShaderBoxNativeBridge::NativeWindowBridge::GetWindowHandle() const
{
	return m_pWindow->GetHandle();
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetModel(const std::wstring& filePath) const
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, filePath]
	{
		m_pMeshDrawComponent->SetMesh(ContentManager::Load<Mesh>(filePath));
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdateShaders(const std::wstring& vertexShader, const std::wstring& hullShader,
	const std::wstring& domainShader, const std::wstring& geometryShader, const std::wstring& pixelShader)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, vertexShader, hullShader, domainShader, geometryShader, pixelShader]
	{
		// Logic wont work for GameObjects not attached to scene.
		// This logic should be rewritten, maybe use custom draw component
		// Planned for when adding support for multiple objects in scene inside shaderbox
		Effect* pActiveEffect = const_cast<Effect*>(m_pActiveMaterial->GetEffect());
		const InputLayoutDesc* pOldInputLayoutDesc = pActiveEffect->pInputLayoutDesc;
		const ID3D11VertexShader* pOldVertexShader = pActiveEffect->pVertexShader;
		const ID3D11HullShader* pOldHullShader = pActiveEffect->pHullShader;
		const ID3D11DomainShader* pOldDomainShader = pActiveEffect->pDomainShader;
		const ID3D11GeometryShader* pOldGeometryShader = pActiveEffect->pGeometryShader;
		const ID3D11PixelShader* pOldPixelShader = pActiveEffect->pPixelShader;

		std::vector<Effect*> affectedEffectPtrArr;

		EffectFactory::GetInstance()->ForceRefreshEffect(pActiveEffect, vertexShader, pixelShader, geometryShader, hullShader, domainShader);

		// Update all effects which uses one of the shaders passed in
		// And gather the effects which have a changed, input layout or vertexshader so, draw components buffers can be refreshed
		for (const std::pair<size_t, Effect*>& pair : EffectFactory::GetInstance()->GetEffects())
		{
			Effect* pEffect = pair.second;

			if (pEffect == pActiveEffect)
			{
				affectedEffectPtrArr.push_back(pEffect);
				continue;
			}

			bool addToAffected = false;
			if (pEffect->pInputLayoutDesc == pOldInputLayoutDesc && pOldInputLayoutDesc != pActiveEffect->pInputLayoutDesc)
			{
				addToAffected = true;
				pEffect->pInputLayoutDesc = pActiveEffect->pInputLayoutDesc;
			}
			if (pEffect->pVertexShader == pOldVertexShader)
			{
				addToAffected = true;
				pEffect->pVertexShader = pActiveEffect->pVertexShader;
			}

			if (addToAffected)
			{
				affectedEffectPtrArr.push_back(pEffect);
			}

			if (pEffect->pHullShader == pOldHullShader)
			{
				pEffect->pHullShader = pActiveEffect->pHullShader;
			}
			if (pEffect->pDomainShader == pOldDomainShader)
			{
				pEffect->pDomainShader = pActiveEffect->pDomainShader;
			}
			if (pEffect->pGeometryShader == pOldGeometryShader)
			{
				pEffect->pGeometryShader = pActiveEffect->pGeometryShader;
			}
			if (pEffect->pPixelShader == pOldPixelShader)
			{
				pEffect->pPixelShader = pActiveEffect->pPixelShader;
			}
		}

		// Update buffers draw components, Depth First
		std::stack<const GameObject*> objectPtrStack;
		const GameObject* pCurrObj;
		for (const GameScene* pScene : SceneManager::GetInstance()->GetScenes())
		{
			for (const GameObject* pObj : pScene->GetChildren())
			{
				objectPtrStack.push(pObj);
				while (!objectPtrStack.empty())
				{
					pCurrObj = objectPtrStack.top();
					objectPtrStack.pop();

					for (BaseComponent* pComp : pCurrObj->GetComponents())
					{
						MeshDrawComponent* pMeshDrawComp = dynamic_cast<MeshDrawComponent*>(pComp);
						if (pMeshDrawComp != nullptr)
						{
							const std::vector<Effect*>::const_iterator it = std::find(affectedEffectPtrArr.cbegin(), affectedEffectPtrArr.cend(), pMeshDrawComp->GetMaterial()->GetEffect());
							if (it != affectedEffectPtrArr.cend())
							{
								pMeshDrawComp->RefreshBuffers();
							}
						}
					}

					for (const TransformComponent* pChild : pCurrObj->GetTransform()->GetChildren())
					{
						objectPtrStack.push(pChild->GetGameObject());
					}
				}
			}
		}

		// Release unused resources
		ContentManager::GetMeshLoader()->UnloadUnusedMeshBuffers();
		// Only input layouts can be unused
		ShaderStorage::GetInstance()->DestroyUnusedShaders();
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdatePPShader(const std::wstring & pixelShader)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, pixelShader]
	{
		const ID3D11PixelShader* pOldPixelShader = m_pActivePPMaterial->GetShader();

		ID3D11PixelShader* pNewPixelShader = ShaderStorage::GetInstance()->RefreshPixelShaderForced(pOldPixelShader, pixelShader);

		for (ShaderBoxPPMaterial* pMat : ShaderBoxPPMaterial::GetMaterials())
		{
			if (pMat->GetShader() == pOldPixelShader)
			{
				pMat->SetShader(pNewPixelShader);
			}
		}
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetShaders(const std::wstring& vertexShader, const std::wstring& hullShader,
	const std::wstring& domainShader, const std::wstring& geometryShader, const std::wstring& pixelShader)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, vertexShader, hullShader, domainShader, geometryShader, pixelShader]
	{
		m_pCamComponent->ClearPPMaterials();

		Effect* pActiveEffect = EffectFactory::GetInstance()->LoadEffect(vertexShader, pixelShader, geometryShader, hullShader, domainShader);
		delete m_pActiveMaterial;
		m_pActiveMaterial = new ShaderBoxMaterial(pActiveEffect);
		m_pMeshDrawComponent->SetMaterial(m_pActiveMaterial);
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetPPShader(const std::wstring& pixelShader)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, pixelShader]
	{
		m_pCamComponent->ClearPPMaterials();
		m_pMeshDrawComponent->SetMaterial(m_pDefaultMaterial);

		delete m_pActivePPMaterial;
		m_pActivePPMaterial = new ShaderBoxPPMaterial(pixelShader);
		m_pCamComponent->AddPPMaterial(m_pActivePPMaterial);
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetRasterizerState(const int cullMode, const int fillMode)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, cullMode, fillMode]
	{
		m_pActiveMaterial->SetRasterizerState(cullMode, fillMode);
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetTopology(const int topology)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, topology]
	{
		Effect* pActiveEffect = const_cast<Effect*>(m_pActiveMaterial->GetEffect());
		D3D11_PRIMITIVE_TOPOLOGY newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		switch (topology)
		{
		case 0:
			newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case 1:
			newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
			break;
		case 2:
			newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
			break;
		}

		if (newTop != pActiveEffect->Topology)
		{
			pActiveEffect->Topology = newTop;

			// Update buffers draw components, Depth First
			std::stack<const GameObject*> objectPtrStack;
			const GameObject* pCurrObj;
			for (const GameScene* pScene : SceneManager::GetInstance()->GetScenes())
			{
				for (const GameObject* pObj : pScene->GetChildren())
				{
					objectPtrStack.push(pObj);
					while (!objectPtrStack.empty())
					{
						pCurrObj = objectPtrStack.top();
						objectPtrStack.pop();

						for (BaseComponent* pComp : pCurrObj->GetComponents())
						{
							MeshDrawComponent* pMeshDrawComp = dynamic_cast<MeshDrawComponent*>(pComp);
							if (pMeshDrawComp != nullptr)
							{
								if (pMeshDrawComp->GetMaterial()->GetEffect() == pActiveEffect)
								{
									pMeshDrawComp->RefreshBuffers();
								}
							}
						}

						for (const TransformComponent* pChild : pCurrObj->GetTransform()->GetChildren())
						{
							objectPtrStack.push(pChild->GetGameObject());
						}
					}
				}
			}

			// Release unused resources
			ContentManager::GetMeshLoader()->UnloadUnusedMeshBuffers();
		}
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdateBuffer(const UINT slotIndex, const int shaderType, const std::byte* data, const UINT length)
{
	std::byte* newData = new std::byte[length];
	memcpy(newData, data, length);

	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, slotIndex, shaderType, newData, length]
	{
		m_pActiveMaterial->SetBuffer(slotIndex, shaderType, newData, length);
		delete[] newData;
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdateImageBuffer(const UINT slotIndex, const int shaderType, const std::wstring& path)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, slotIndex, shaderType, path]
	{
		m_pActiveMaterial->SetImageBuffer(slotIndex, shaderType, path);
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdatePPBuffer(const UINT slotIndex, const std::byte* data, const UINT length)
{
	std::byte* newData = new std::byte[length];
	memcpy(newData, data, length);

	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, slotIndex, newData, length]
	{
		m_pActivePPMaterial->SetBuffer(slotIndex, newData, length);
		delete[] newData;
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::UpdatePPImageBuffer(const UINT slotIndex, const std::wstring& path)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, slotIndex, path]
	{
		m_pActivePPMaterial->SetImageBuffer(slotIndex, path);
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::RenderThumbnail(const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, saveLocation, callback]
	{
		const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();

		m_pCamComponent->SetAspectRatio(1.0f);

		NativeBridge* pBridge = NativeBridge::GetInstance();
		
		pBridge->SetThumbnailRenderTarget();
		WindowContext context;
		context.Width = pBridge->GetThumbSize();
		context.Height = pBridge->GetThumbSize();
		context.pGameTime = m_pWindow->GetWindowContext().pGameTime;
		m_pWindow->GetScene()->Draw(engineContext, context, pBridge->GetThumbRT(), pBridge->GetPPThumbRT());
		CHECK_HR(SaveWICTextureToFile(engineContext.pDeviceContext, pBridge->GetThumbTexture(), GUID_ContainerFormatPng, saveLocation.c_str()));

		// Reset
		m_pWindow->SetUpViewport();

		m_pCamComponent->SetAspectRatio(0.0f);

		callback();
	});
}

void ShaderBoxNativeBridge::NativeWindowBridge::SetCameraOffset(const float offset)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, offset]
	{
		XMVECTOR worldPosition = XMVectorSet(0, 5, -5, 0);
		XMVECTOR eyeDir = XMVector3Normalize(XMVectorSet(0, -5, 5, 0));

		worldPosition = worldPosition - eyeDir * offset;

		XMFLOAT3 worldPos;
		XMStoreFloat3(&worldPos, worldPosition);
		m_pCamComponent->GetGameObject()->GetTransform()->Translation(worldPos);
	});
}