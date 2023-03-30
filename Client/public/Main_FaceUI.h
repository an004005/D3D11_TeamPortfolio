#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_FaceUI final : public CUI
{
private:
	CMain_FaceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_FaceUI(const CMain_FaceUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	void	Set_Face(const _float & fFace);

private:
	_bool	m_bChange = { false };
	_float	m_fFaceNum = { 0.0f };
	_double m_dChange_TimeAcc = { 0.0 };

public:
	static CMain_FaceUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END