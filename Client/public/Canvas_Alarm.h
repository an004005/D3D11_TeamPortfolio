#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Alarm : public CCanvas
{
protected:
	CCanvas_Alarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Alarm(const CCanvas_Alarm& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual void   Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void   Imgui_RenderProperty() override;
	virtual void   SaveToJson(Json& json) override;
	virtual void   LoadFromJson(const Json& json) override;

public:
	// ���� ������ �Ѿ ��
	void	Set_OpenNextRoomName(const wstring wsNextRoomName);

	// ���� ������ ��
	void	Set_Appeart();

	// ���� �� �� ��
	void	Set_LevelUp(const _uint iLevel);

	// ���� ������ �Ѿ�� �� �̸� ���� (�����ð��� ������ ����.) => [0] ���� [1] ����ö [3] ���� [3] ������ (������ ����, �ߴ��� ����)
	void	Set_MapName(const _float & fMapNameIndex) { 
		m_bMapName = true;
		m_bMapName_TimeAcc = 0.0;
		m_fMapNameIndex = fMapNameIndex;
	}

private:
	void	Appeart_Tick();
	void	MapName_Tick(const _double & dTimeDelta);
	void	LevelUpDead_Tick();

private:
	_bool	m_bCheck_Appeart = { false };

	_bool	m_bMapName = { false };
	_double	m_bMapName_TimeAcc = { 0.0 };
	_float	m_fMapNameIndex = { 0.0f };

	_bool	m_bLevelUpDead = { false };

public:
	static CCanvas_Alarm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END