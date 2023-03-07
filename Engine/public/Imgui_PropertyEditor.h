#pragma once
#include "ImguiObject.h"

BEGIN(Engine)

// �纻 object�� �����ϰ�, ������ object�� ������ Ȯ���ϰ� �����ϴ� ����� ���� imgui object
// ������ object�� component�� ���� Imgui_PropertyRender�Լ��� �����Ѵ�.
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