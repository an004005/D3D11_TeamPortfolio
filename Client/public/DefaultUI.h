#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDefaultUI final : public CUI
{
private:
	CDefaultUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefaultUI(const CDefaultUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	static CDefaultUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END