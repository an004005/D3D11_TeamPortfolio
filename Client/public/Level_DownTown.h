#pragma once
#include "Level_StageDefault.h"
#include "Client_Defines.h"

BEGIN(Client)

/*********************
 * CLevel_DownTown
 **********************/
class CLevel_DownTown : public CLevel_StageDefault
{
private:
	CLevel_DownTown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_DownTown() = default;

public:
	virtual HRESULT Initialize() override;

	
public:
	static CLevel_DownTown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

/*********************
 * CLevel_DownTown_Second
 **********************/
class CLevel_DownTown_Second : public CLevel_StageDefault
{
private:
	CLevel_DownTown_Second(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_DownTown_Second() = default;

public:
	virtual HRESULT Initialize() override;
	
public:
	static CLevel_DownTown_Second* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

/*********************
 * CLevel_DownTown_Third
 **********************/
class CLevel_DownTown_Third : public CLevel_StageDefault
{
private:
	CLevel_DownTown_Third(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_DownTown_Third() = default;

public:
	virtual HRESULT Initialize() override;
	
public:
	static CLevel_DownTown_Third* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END