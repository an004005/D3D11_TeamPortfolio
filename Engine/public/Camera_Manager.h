#pragma once
#include "Base.h"
#include "Camera.h"

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	void Tick();

public:
	CCamera* Add_Camera(const string& strCamTag, _uint iLevelIdx, const wstring& pLayerTag, const wstring& pPrototypeTag, const Json* camJson);

	void SetMainCamera(const string& strCamTag);
	void SetMainCamera(CCamera* pCamera);

	CCamera* GetMainCam();

	_matrix GetCamViewMatrix(const string& strCamTag);
	_matrix GetCamProjMatrix(const string& strCamTag);

	_float4 Get_CamPosition(const string& strCamTag);
	CCamera* FindCamera(const string& strCamTag);

	void	Set_Far(_float fFar) { m_fFar = fFar; }

	void Clear();

	void Imgui_Render();

private:
	// void	Set_Cascade();
private:
	unordered_map<string, CCamera*> m_Cameras;
	string m_strMainCamTag;

	_float		m_fFar = 6.f;

public:
	virtual void Free() override;
	
};


END