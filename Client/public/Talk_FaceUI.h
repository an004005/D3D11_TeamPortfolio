#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CTalk_FaceUI final : public CUI
{
private:
	CTalk_FaceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTalk_FaceUI(const CTalk_FaceUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Face(const _float2& fFace, const wstring& szTalk0, const wstring& szTalk1);

private:
	_bool		m_bChange = { false };
	_float		m_fFace = { 0.0 };
	_double	m_dChange_TimeAcc = { 0.0 };
	wstring m_wsTalk0 = { L"" };
	wstring m_wsTalk1 = { L"" };

public:
	static CTalk_FaceUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END