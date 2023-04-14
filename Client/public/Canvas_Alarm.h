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
	// 다음 레벨로 넘어갈 때
	void	Set_OpenNextRoomName(const wstring wsNextRoomName);

	// 보스 등장할 때
	void	Set_Appeart();

	// 레벨 업 할 때
	void	Set_LevelUp(const _uint iLevel);

	// 다음 맵으로 넘어가서 맵 이름 띄우기 (일정시간이 지나면 삭제.) => [0] 도시 [1] 지하철 [3] 병원 [3] 공사장 (나오미 보스, 중대장 보스)
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