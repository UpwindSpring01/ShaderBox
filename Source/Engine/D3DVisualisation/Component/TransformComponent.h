#pragma once

#include "Component.h"

class TransformComponent final : public Component<TransformComponent>
{
public:
	TransformComponent() {};
	~TransformComponent() {};
#ifdef _DEBUG
	bool IsMultipleComponentAllowed() const override { return false; }
#endif _DEBUG

	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Translate(float x, float y, float z);

	XMFLOAT4X4 GetWorld();
	XMFLOAT3 GetWorldPosition();
	XMFLOAT3 GetWordScale();
	XMFLOAT4 GetWorldRotation();
private:
	void SetDirty(byte dirtyFlags);
	void Update() override {};
	void Draw(const GameContext& /* gameContext */) override {};
	void PostDraw() override {};

	void UpdateTransforms();
private:
	enum DirtyFlag : byte
	{
		SW_DIRTY_NONE = 0,
		SW_DIRTY_TRANSLATE = 1 << 0,
		SW_DIRTY_SCALE = 1 << 1,
		SW_DIRTY_ROTATION = 1 << 2,

	};
	XMFLOAT4X4 m_World = { 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };

	XMFLOAT3 m_Position = { 0, 0, 0 };
	XMFLOAT3 m_WorldPosition = { 0, 0, 0 };
	XMFLOAT3 m_Scale = { 1, 1, 1 };
	XMFLOAT3 m_WorldScale = { 1, 1, 1 };
	XMFLOAT4 m_Rotation = { 0, 0, 0, 1 };
	XMFLOAT4 m_WorldRotation = { 0, 0, 0, 1 };

	byte m_DirtyFlags = SW_DIRTY_NONE;
};