#pragma once
#include "ImguiObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

typedef struct tBatchPreset
{
	string strLayer;
	string strJsonPath;
	string strProtoTag;
}BATCH_PRESET;

typedef struct tBatchObject
{
	BATCH_PRESET tPreset;
	CGameObject* pObj = nullptr;
}BATCH_OBJ;

class CImgui_Batch : public CImguiObject
{
private:
	CImgui_Batch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderTab() override;

	void LoadBatchFile();
	void SaveBatchFile();
	static void RunBatchFile(const string& strBatchFilePath);

private:
	_bool GetObjFromRayCast(CGameObject*& pObj, _uint iTargetType = CTB_STATIC);
	void CloneBatchPreset(_float4 vPos, size_t iPresetIdx);

public:
	vector<BATCH_PRESET> m_BatchPresets;
	list<BATCH_OBJ> m_BatchObjList;
	BATCH_OBJ m_SelectedBatchObj;


public:
	static CImgui_Batch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END