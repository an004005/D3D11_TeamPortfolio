#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Acquisition : public CCanvas
{
protected:
	CCanvas_Acquisition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Acquisition(const CCanvas_Acquisition& rhs);

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
	void	Set_EnemyUI(const EEnemyName eEnemyName, const _uint iLevel);

private:
	void	EnemyUIAdd_Tick(const _double & TimeDelta);
	void	AllPositionChange();

	void	DeleteUI();

private:
	list<class CAcquisitionUI*> m_AcquisitionUIList;
	
	_double	m_dEnemyClone_TimeAcc = { 0.0 };

public:
	static CCanvas_Acquisition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END