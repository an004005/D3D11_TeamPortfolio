#pragma once
#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImgui_CameraManager : public CImguiObject
{
private:
	CImgui_CameraManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;
	static CImgui_CameraManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END