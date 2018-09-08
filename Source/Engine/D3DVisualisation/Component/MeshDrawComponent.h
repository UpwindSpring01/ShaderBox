#pragma once
#include "Component.h"

namespace SpringWindEngine
{
	class MeshDrawComponent final : public Component<MeshDrawComponent>
	{
	public:
		MeshDrawComponent() {}

		void SetMesh(Unsafe_Shared_Ptr<Mesh> pMesh);
		Unsafe_Shared_Ptr<Mesh> GetMesh() const { return m_pMesh; }
		void SetMaterial(Material* pMaterial);
		Material* GetMaterial() const { return m_pMaterial; }

		void SetMeshAndMaterial(Unsafe_Shared_Ptr<Mesh> pMesh, Material* pMaterial);

		void RefreshBuffers();
	private:
		void Update() override {};
		void Draw(const EngineContext& engineContext) override;

		Unsafe_Shared_Ptr<ID3D11Buffer> m_pVertexBuffer = nullptr;
		Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>> m_pIndexBuffer = nullptr;

		Unsafe_Shared_Ptr<Mesh> m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
	};
}