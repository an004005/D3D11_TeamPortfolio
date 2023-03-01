#pragma once
#include "GameObject.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CPostProcess abstract : public CGameObject
{
protected:
	CPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostProcess(const CPostProcess& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;

	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;

	ShaderParams& GetParam() { return m_tParam; }
	_uint GetPass() const { return m_iPass; }
	_uint GetPriority() const { return m_iPriority; }

	virtual void Imgui_RenderProperty() override;

private:
	class CRenderer* m_pRenderer = nullptr;

protected:
	ShaderParams m_tParam;
	_uint m_iPass = 0;
	// priority가 작은거부터 먼저 실행한다.
	_uint m_iPriority = 0;

public:
	virtual void Free() override;

};

END