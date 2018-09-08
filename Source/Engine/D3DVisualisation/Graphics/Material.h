#pragma once

namespace SpringWindEngine
{
	class Material
	{
	public:
		Material(Effect* pEffect);
		virtual ~Material();
		
		void SetShadersAndBuffers(const EngineContext& engineContext, const TransformComponent* pTransComp);
		
		const Effect* GetEffect() const { return m_pEffect; }
		
		virtual void BeginDraw() {};
		virtual void EndDraw() {};
	private:
		Effect* m_pEffect = nullptr;

		Material(const Material &obj) = delete;
		Material& operator=(const Material& obj) = delete;
	};
}