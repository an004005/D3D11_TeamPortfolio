#pragma once
#include "UI.h"

BEGIN(Engine)

class CTextUI : public CUI
{
protected:
	CTextUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextUI(const CUI& rhs);
	virtual void Tick(_double TimeDelta) override;

protected:
	void RenderFont();

protected:
	class CGameInstance* m_pGameInstance = nullptr;

	const wstring m_strFont;
	const wstring m_strText;
	_float2 m_vFontPos;
	_float2 m_vFontScale;
	_float4 m_vFontColor;

public:
	virtual void Free() override;
};

END