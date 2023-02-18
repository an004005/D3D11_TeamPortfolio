#pragma once
#include "UI.h"

BEGIN(Engine)

// UI�� ������ �ִ� ������ Layer �������� �����Ѵ�.
class ENGINE_DLL CCanvas abstract : public CUI
{
protected:
	CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas(const CCanvas& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	// child �� BeginTick�� �θ𿡼� �������� �ʴ� ������ object manager���� �浹�� ���ϱ� ����(���������� �ʿ��ѵ�...)
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;

	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;

	CUI* Find_ChildUI(const _tchar* pChildTag);
protected:
	// ref �ڵ� ���� �ȵ˴ϴ�.
	CUI* Add_ChildUI(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pChildTag, void* pArg = nullptr);

protected:
	map<wstring, CUI*> m_mapChildUIs;

private:
	const _tchar* m_pSelectedChild = nullptr;

public:
	virtual void Free() override;
};
END
