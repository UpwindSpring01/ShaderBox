#pragma once

namespace SpringWindEngine
{
	class PPMaterial;

	class CameraComponent final : public Component<CameraComponent>
	{
	public:
		const XMFLOAT4X4& GetViewMatrix();
		const XMMATRIX GetView();
		const XMFLOAT4X4& GetProjectionMatrix();
		const XMMATRIX GetProjection();
		const XMMATRIX GetViewProjection();

		void SetProjectionDirty();
		void SetAspectRatio(const float aspectRatio);
		void ClearPPMaterials() { m_PPMaterialPtrArr.clear(); }
		void AddPPMaterial(PPMaterial* pMaterial);
	private:
		friend class GameScene;

		void Update() override {};
		void Draw(const EngineContext& /*engineContext*/) override {};

		void UpdateFrameData(const EngineContext& engineContext);
		void SetTransformDirty() override;

		const std::vector<PPMaterial*>& GetPPMaterials() const { return m_PPMaterialPtrArr; }

		float m_NearPlane = 0.01f;
		float m_FarPlane = 100.0f;

		float m_AspectRatio = 0.0f;

		XMFLOAT4X4 m_View;
		XMFLOAT4X4 m_Projection;
		XMFLOAT4X4 m_ViewProjection;
		enum DirtyFlag : byte
		{
			SW_DIRTY_NONE = 0,
			SW_DIRTY_PROJECTION = 1 << 0,
			SW_DIRTY_VIEW = 1 << 1,
			SW_DIRTY_VIEWPROJECTION = 1 << 2,
			SW_DIRTY_BUFFER_PROJECTION = 1 << 3,
			SW_DIRTY_BUFFER_VIEW = 1 << 4,
		};
		byte m_DirtyFlags = ~SW_DIRTY_NONE;

		std::vector<PPMaterial*> m_PPMaterialPtrArr;
	};
}