#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Client)

class CPointLight : public CGameObject
{
private:
	CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointLight(const CPointLight& rhs);
	virtual ~CPointLight() = default;

public:
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

	void Active(_bool bActive);
	void SetRange(_float fRange);
	void SetColor(_float4 vColor);

private:
	static string CreatePointLightTag();

private:
	CLight* m_pPointLight = nullptr;
	string m_strLightTag;

public:
	static CPointLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END