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
	void	Set_OpenNextRoomName(const _tchar * pNextRoomName);
	void	Set_CloseNextRoomName();

	void	Set_Appeart();

private:
	void	Set_ChildAppeart();

private:
	_bool	m_bCheck_Appeart = { false };

public:
	static CCanvas_Alarm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END