#pragma once
#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImgui_CamAnimEditor : public CImguiObject
{
private:
	CImgui_CamAnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;
	virtual void Imgui_RenderTab() override;

private:
	class CAnimCam* m_pAnimCam = nullptr;
	class CCamAnimation* m_pCurAnim = nullptr;

	static CImgui_CamAnimEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END