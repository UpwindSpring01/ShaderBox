#pragma once

#include "Component.h"

namespace SpringWindEngine
{
	class TransformComponent final : public Component<TransformComponent>
	{
	public:
		TransformComponent() {};
		~TransformComponent() {};
#ifdef _DEBUG
		bool IsMultipleComponentAllowed() const override { return false; }
#endif _DEBUG

		void Rotation(const float x, const float y, const float z);
		void Rotation(const XMFLOAT4& rotation);
		void Scale(const float x, const float y, const float z);
		void Translation(const float x, const float y, const float z);
		void Translation(const XMFLOAT3& translation);
		

		const XMFLOAT4X4& GetWorldMatrix();
		const XMMATRIX GetWorld();
		const XMFLOAT3& GetWorldPosition();
		const XMFLOAT3& GetWordScale();
		const XMFLOAT4& GetWorldRotation();

		void AddChild(TransformComponent* pChild);
		void MoveChild(TransformComponent* pChild);
		const std::vector<TransformComponent*>& GetChildren() const { return m_ChildrenPtrArr; }
	private:
		void SetDirty(byte dirtyFlags);
		void Update() override {};
		void Draw(const EngineContext& /*engineContext*/) override {};

		void UpdateTransforms(bool decompose);

		std::vector<TransformComponent*> m_ChildrenPtrArr;
		TransformComponent* m_pParent = nullptr;

		enum DirtyFlag : byte
		{
			SW_DIRTY_NONE = 0,
			SW_DIRTY_TRANSLATE = 1 << 0,
			SW_DIRTY_SCALE = 1 << 1,
			SW_DIRTY_ROTATION = 1 << 2,
			SW_DIRTY_WORLD = 1 << 3
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
}