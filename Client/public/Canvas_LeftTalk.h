#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagTalkInfo
{
	_float2	fFace = { 0.0f, 0.0f };
	wstring wsTalk0 = L"";
	wstring wsTalk1 = L"";

}TALKINFO;

class CCanvas_LeftTalk : public CCanvas
{
protected:
	CCanvas_LeftTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_LeftTalk(const CCanvas_LeftTalk& rhs);

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
	void	Set_Talk(const size_t iIndex);

	void	TalkInfo_Initialize();

private:
	vector<TALKINFO>	m_vecTalkInfo;

	_bool	m_bTalk = { false };
	_double	m_dTalk_TimeAcc = { 0.0 };
	
public:
	static CCanvas_LeftTalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END