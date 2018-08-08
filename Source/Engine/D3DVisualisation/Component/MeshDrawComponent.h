#pragma once
#include "Component.h"

class MeshDrawComponent final : public Component<MeshDrawComponent>
{
public:
	void SetMesh(Mesh* mesh) { m_pMesh = mesh; }
	Mesh* GetMesh() const { return m_pMesh; }
	void SetMaterial(Material* material) { m_pMaterial = material; }
	Material* GetMaterial() const { return m_pMaterial; }
private:
	void Update() override {};
	void Draw(const GameContext& gameContext) override;
	void PostDraw() override {};

	Mesh* m_pMesh = nullptr;
	Material* m_pMaterial = nullptr;
};