#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Client)

class CCapsuleLight : public CGameObject
{
private:
	CCapsuleLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCapsuleLight(const CCapsuleLight& rhs);
	virtual ~CCapsuleLight() = default;

public:
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

	void Active(_bool bActive);
	void SetRange(_float fRange);
	void SetColor(_float4 vColor);
	void SetLength(_float fLength);

private:
	static string CreateCapsuleLightTag();

private:
	CLight* m_pCapsuleLight = nullptr;
	string m_strLightTag;
	_float m_fLength = 1.f;

public:
	static CCapsuleLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END