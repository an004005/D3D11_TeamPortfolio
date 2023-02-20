#pragma once
#include "ImguiObject.h"
#include "Bone.h"

BEGIN(Engine)
class ENGINE_DLL CImgui_ModelConverter : public CImguiObject
{
private:
	CImgui_ModelConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;
	static CImgui_ModelConverter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

#ifdef USE_ASSIMPT_CONVERTER
public:


private:
	void ConvertStaticModel(const char* pModelPath);
	void ConvertAnimModel(const char* pModelPath);
	void ConvertBone(Json& jBone, aiNode* pAINode);
	_bool FindBone(const Json& jBone, const char* pBoneName);
	void SetOffsetMatrix(const char* pBoneName, Json& jBone, const _float4x4& OffsetMatrix);
	void ConvertAnimation(string strDirName, aiAnimation* pAIAnimation);
#endif
private:
	char m_szSaveDir[MAX_PATH]{};
};


END
