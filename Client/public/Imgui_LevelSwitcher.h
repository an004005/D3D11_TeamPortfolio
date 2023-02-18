#pragma once
#include "ImguiObject.h"

BEGIN(Client)

class CImgui_LevelSwitcher : public CImguiObject
{
private:
	CImgui_LevelSwitcher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderMenu() override;
	static CImgui_LevelSwitcher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);

	virtual void Free() override;

private:

};

END