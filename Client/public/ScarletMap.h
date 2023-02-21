#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

class CScarletMap : public CGameObject
{
public:
	CScarletMap();
	virtual ~CScarletMap();


	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	HRESULT Render_ShadowDepth() override;
	void SaveToJson(OUT Json& json) override;
	void LoadFromJson(const Json& json) override;
	void Imgui_RenderProperty() override;
	void SetDelete() override;

public:
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

