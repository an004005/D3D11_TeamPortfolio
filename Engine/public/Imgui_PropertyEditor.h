#pragma once
#include "ImguiObject.h"

BEGIN(Engine)

// 사본 object를 선택하고, 선택한 object의 정보를 확인하고 수정하는 기능을 가진 imgui object
// 선택한 object와 component가 가진 Imgui_PropertyRender함수를 실행한다.
class ENGINE_DLL CImgui_PropertyEditor final : public CImguiObject
{
private:
	CImgui_PropertyEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;
	static CImgui_PropertyEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameObject* m_pSelectedObject = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;
};

END