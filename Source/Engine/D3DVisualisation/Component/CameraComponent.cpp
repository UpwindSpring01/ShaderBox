#include "stdafx.h"

#include "CameraComponent.h"
#include "Base/SpringWindWindow.h"

void SpringWindEngine::CameraComponent::UpdateFrameData(const EngineContext& engineContext)
{
	bool forceUpdate = SpringWind::GetInstance()->GetLastRenderedCamera() != this;
	bool updated = false;
	if (forceUpdate || (m_DirtyFlags & SW_DIRTY_BUFFER_PROJECTION) != 0)
	{
		XMStoreFloat4x4(&engineContext.pBaseCBufferFrame->projection, XMMatrixTranspose(GetProjection()));
		m_DirtyFlags &= ~SW_DIRTY_BUFFER_PROJECTION;
		updated = true;
	}

	if (forceUpdate || (m_DirtyFlags & SW_DIRTY_BUFFER_VIEW) != 0)
	{
		XMMATRIX transposedView = XMMatrixTranspose(GetView());
		XMStoreFloat4x4(&engineContext.pBaseCBufferFrame->view, transposedView);
		XMStoreFloat4x4(&engineContext.pBaseCBufferFrame->viewInverse, XMMatrixInverse(nullptr, transposedView));
		m_DirtyFlags &= ~SW_DIRTY_BUFFER_VIEW;
		updated = true;
	}

	if (updated)
	{
		if (forceUpdate)
		{
			SpringWind::GetInstance()->SetLastRenderedCamera(this);
		}
		SpringWind::GetInstance()->UpdateFrameBuffer();
	}
}

const XMMATRIX SpringWindEngine::CameraComponent::GetView()
{
	if ((m_DirtyFlags & SW_DIRTY_VIEW) != 0)
	{
		XMMATRIX view = XMMatrixInverse(nullptr, GetGameObject()->GetTransform()->GetWorld());
		XMStoreFloat4x4(&m_View, view);

		m_DirtyFlags &= ~SW_DIRTY_VIEW;
		return view;
	}
	return XMLoadFloat4x4(&m_View);
}

const XMFLOAT4X4 & SpringWindEngine::CameraComponent::GetViewMatrix()
{
	if ((m_DirtyFlags & SW_DIRTY_VIEW) != 0)
	{
		XMMATRIX view = XMMatrixInverse(nullptr, GetGameObject()->GetTransform()->GetWorld());
		XMStoreFloat4x4(&m_View, view);

		m_DirtyFlags &= ~SW_DIRTY_VIEW;
	}
	return m_View;
}

const XMMATRIX SpringWindEngine::CameraComponent::GetProjection()
{
	if ((m_DirtyFlags & SW_DIRTY_PROJECTION) != 0)
	{
		XMMATRIX projection;
		if (m_AspectRatio == 0)
		{
			const SpringWindWindow* pWindow = GetGameObject()->GetScene()->GetWindow();
			float aspectRatio = (float)pWindow->GetWidth() / pWindow->GetHeight();
			projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, m_NearPlane, m_FarPlane);
		}
		else
		{
			projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_AspectRatio, m_NearPlane, m_FarPlane);
		}
		XMStoreFloat4x4(&m_Projection, projection);

		m_DirtyFlags &= ~SW_DIRTY_PROJECTION;
		return projection;
	}
	return XMLoadFloat4x4(&m_Projection);
}

const XMFLOAT4X4& SpringWindEngine::CameraComponent::GetProjectionMatrix()
{
	if ((m_DirtyFlags & SW_DIRTY_PROJECTION) != 0)
	{
		XMMATRIX projection;
		if (m_AspectRatio == 0)
		{
			const SpringWindWindow* pWindow = GetGameObject()->GetScene()->GetWindow();
			float aspectRatio = (float)pWindow->GetWidth() / pWindow->GetHeight();
			projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, m_NearPlane, m_FarPlane);
		}
		else
		{
			projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_AspectRatio, m_NearPlane, m_FarPlane);
		}
		XMStoreFloat4x4(&m_Projection, projection);

		m_DirtyFlags &= ~SW_DIRTY_PROJECTION;
	}
	return m_Projection;
}

const XMMATRIX SpringWindEngine::CameraComponent::GetViewProjection()
{
	if ((m_DirtyFlags & SW_DIRTY_VIEWPROJECTION) != 0)
	{
		XMMATRIX viewProj = GetView() * GetProjection();
		XMStoreFloat4x4(&m_ViewProjection, viewProj);

		m_DirtyFlags &= ~SW_DIRTY_VIEWPROJECTION;
		return viewProj;
	}
	return XMLoadFloat4x4(&m_ViewProjection);
}

void SpringWindEngine::CameraComponent::SetAspectRatio(const float ratio)
{
	m_AspectRatio = ratio;
	SetProjectionDirty();
}

void SpringWindEngine::CameraComponent::SetTransformDirty()
{
	m_DirtyFlags |= SW_DIRTY_VIEW | SW_DIRTY_BUFFER_VIEW | SW_DIRTY_VIEWPROJECTION;
}

void SpringWindEngine::CameraComponent::SetProjectionDirty()
{
	m_DirtyFlags |= SW_DIRTY_PROJECTION | SW_DIRTY_BUFFER_PROJECTION | SW_DIRTY_VIEWPROJECTION;
}

void SpringWindEngine::CameraComponent::AddPPMaterial(PPMaterial* pMaterial)
{
	m_PPMaterialPtrArr.push_back(pMaterial);
}
