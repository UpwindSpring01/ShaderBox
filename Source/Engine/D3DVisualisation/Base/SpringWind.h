//////////////////////////
// Stripped/Changed version of the SpringWind Engine
//////////////////////////

#pragma once

class SpringWind
{
public:
	SpringWind();
	virtual ~SpringWind();

	static SpringWind* GetInstance() { return m_pInstance; }

	HRESULT Initialize();

	HRESULT Render(void * pResource, bool isNewSurface);

	HRESULT RenderTumbnail(const std::wstring model, const std::wstring saveLocation);
	HRESULT RenderTumbnail(const std::wstring saveLocation);

	GameScene* GetScene() const { return m_pScene; }
	const GameContext& GetGameContext() const { return m_GameContext; }

	HRESULT SetRasterizerState(const int cullMode, const int fillMode);
	// Special function definitions to ensure alingment between c# and c++ 
	//void* operator new(size_t size)
	//{
	//	return _aligned_malloc(size, 16);
	//}

	//void operator delete(void *p)
	//{
	//	_aligned_free(p);
	//}

	void SetCameraAspectRatio(float aspectRatio);
	void SetCameraOffset(const float offset);

	struct RenderTarget
	{
		RenderTarget() {}
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
		ID3D11Texture2D* m_pDepthStencil = nullptr;
		ID3D11Texture2D* m_pColor = nullptr;

		~RenderTarget()
		{
			SafeRelease(m_pRenderTargetView);
			SafeRelease(m_pDepthStencilView);
			SafeRelease(m_pShaderResourceView);
			SafeRelease(m_pDepthStencil);
			SafeRelease(m_pColor);
		}

		RenderTarget(const RenderTarget &obj) = delete;
		RenderTarget& operator=(const RenderTarget& obj) = delete;
	};

	RenderTarget& GetRenderTarget()
	{
		return (m_IsRenderingThumb) ? m_ThumbRT : m_MainRT;
	}

	RenderTarget& GetPPRenderTarget()
	{
		return (m_IsRenderingThumb) ? m_ThumbPPRT : m_MainPPRT;
	}

	UINT GetWidth()
	{
		return (m_IsRenderingThumb) ? THUMB_SIZE : m_Width;
	}

	UINT GetHeight()
	{
		return (m_IsRenderingThumb) ? THUMB_SIZE : m_Height;
	}
private:
	void SetUpViewport();
	void UpdatePPTargets(SpringWind::RenderTarget& rt, const UINT width, const UINT height);
	// Initial window resolution
	UINT m_Width;
	UINT m_Height;

	static SpringWind* m_pInstance;

	bool m_IsRenderingThumb = false;

	float m_CameraOffset = 0.0f;

	HRESULT DestroyBuffers();
	HRESULT CreateBuffers();
	HRESULT InitRenderTarget(void* pResource);
	static const UINT THUMB_SIZE = 200;
	HRESULT InitThumbnailRenderTarget();

	D3D_FEATURE_LEVEL m_featureLevel;

	RenderTarget m_MainRT;
	RenderTarget m_ThumbRT;
	RenderTarget m_MainPPRT;
	RenderTarget m_ThumbPPRT;

	friend class GameScene;
	friend class SpriteRenderer;


	D3D11_VIEWPORT m_Viewport = {};

	GameScene* m_pScene = nullptr;
	GameContext m_GameContext;

	SpringWind(const SpringWind& t) = delete;
	SpringWind& operator=(const SpringWind& t) = delete;
};

