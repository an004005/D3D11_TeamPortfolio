#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_WorldMap : public CCanvas
{
protected:
	CCanvas_WorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_WorldMap(const CCanvas_WorldMap& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	void	CloneBar();

	void	KeyInput();
	void	MapBar_Tick();
	void	CurrentLevelName_Tick();

private:
	vector<class CCanvas_MapBar*> vecMapBar;
	_bool	m_bMapUI = { false };

	class CShaderUI* m_pShaderUI = { nullptr };
	_bool		m_bOpen = { false };
	_bool		m_bAlpha = { false };
	_bool		m_bReverse = { false };

	array<_bool, 6> arrMapChake;
	wstring	m_wsName = { L"" };
	_uint		m_iInputCount = { 0 };

public:
	static CCanvas_WorldMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END