#pragma once
#include "UI.h"

BEGIN(Engine)

// UI를 가지고 있는 일종의 Layer 개념으로 생각한다.
class ENGINE_DLL CCanvas abstract : public CUI
{
protected:
	CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas(const CCanvas& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	// child 의 BeginTick을 부모에서 관리하지 않는 이유는 object manager와의 충돌을 피하기 위함(구조개선이 필요한데...)
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;

	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;

	CUI* Find_ChildUI(const _tchar* pChildTag);
protected:
	// ref 자동 증가 안됩니다.
	CUI* Add_ChildUI(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pChildTag, void* pArg = nullptr);

protected:
	map<wstring, CUI*> m_mapChildUIs;

private:
	const _tchar* m_pSelectedChild = nullptr;

public:
	virtual void Free() override;
};
END
