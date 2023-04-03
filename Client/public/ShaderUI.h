#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CShaderUI final : public CUI
{
private:
	CShaderUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShaderUI(const CShaderUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	_float	Get_Floats0() {
		return m_tParams.Floats[0];
	}
	void	Set_Floats0(const _float & fFloat) {
		m_tParams.Floats[0] = fFloat;
	}

	void	Set_Float2s(const _float2& fFloat2) {
		m_tParams.Float2s[0] = fFloat2;
	}

	_float	Get_Float4s_W() {
		return m_tParams.Float4s[0].w;
	}
	void	Set_Float4s_W(const _float fFloat) {
		m_tParams.Float4s[0].w = fFloat;
	}

public:
	static CShaderUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END