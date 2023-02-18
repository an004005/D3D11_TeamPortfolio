#include "stdafx.h"
#include "..\public\Imgui_ModelConverter.h"
#include "Imgui/ImGuiFileDialog.h"
#include "JsonLib.h"
#include "MathUtils.h"
#include "GameUtils.h"


CImgui_ModelConverter::CImgui_ModelConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_ModelConverter::Initialize(void* pArg)
{
	m_szTabname = "ModelConverter";
	return S_OK;
}

void CImgui_ModelConverter::Imgui_RenderTab()
{
	ImGui::InputText("Save Directory", m_szSaveDir, MAX_PATH);
	if (ImGui::Button("Chanage Save Directory"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Change Save Directory", "Choose Directory", nullptr,
		                                        "../Bin/Resources/");
	}
	if (ImGuiFileDialog::Instance()->Display("Change Save Directory"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			// 이상하게 filePath 중간에 \0이 들어있어서 경로를 읽지 못하는 경우가 있다.(많이) 걍 \0문자 제거해버린다.
			filePath.erase(std::remove_if(filePath.begin(), filePath.end(), [](char c)
			{
				return c == '\0';
			}), filePath.end());
			strcpy_s(m_szSaveDir, filePath.c_str());
		}
		ImGuiFileDialog::Instance()->Close();
	}


	if (ImGui::Button("Convert Static Model"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Choose StaticModel To Convert", "Choose File", ".fbx",
		                                        "../Bin/Resources/");
	}
	if (ImGuiFileDialog::Instance()->Display("Choose StaticModel To Convert"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			// 이상하게 filePath 중간에 \0이 들어있어서 경로를 읽지 못하는 경우가 있다.(많이) 걍 \0문자 제거해버린다.
			filePath.erase(std::remove_if(filePath.begin(), filePath.end(), [](char c)
			{
				return c == '\0';
			}), filePath.end());
#ifdef USE_ASSIMPT_CONVERTER
			ConvertStaticModel(filePath.c_str());
#endif
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Convert Anim Model"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Choose AnimModel To Convert", "Choose File", ".fbx",
		                                        "../Bin/Resources/");
	}
	if (ImGuiFileDialog::Instance()->Display("Choose AnimModel To Convert"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			// 이상하게 filePath 중간에 \0이 들어있어서 경로를 읽지 못하는 경우가 있다.(많이) 걍 \0문자 제거해버린다.
			filePath.erase(std::remove_if(filePath.begin(), filePath.end(), [](char c)
			{
				return c == '\0';
			}), filePath.end());

#ifdef USE_ASSIMPT_CONVERTER
			ConvertAnimModel(filePath.c_str());
#endif
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

CImgui_ModelConverter* CImgui_ModelConverter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_ModelConverter(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_ModelConverter::Free()
{
	__super::Free();
}

#ifdef USE_ASSIMPT_CONVERTER

void CImgui_ModelConverter::ConvertStaticModel(const char* pModelPath)
{
	constexpr _uint iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	Assimp::Importer Importer;
	const aiScene* pAIScene = Importer.ReadFile(pModelPath, iFlag);

	char szModelName[MAX_PATH];
	char szDirPath[MAX_PATH];
	_splitpath_s(pModelPath, nullptr, 0, szDirPath, MAX_PATH, szModelName, MAX_PATH, nullptr, 0);

	string saveFileName = szDirPath;
	saveFileName += szModelName;
	saveFileName += ".static_model";
	HANDLE hFile = CreateFile(CGameUtils::s2ws(saveFileName).c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", saveFileName.c_str());
		Importer.FreeScene();
		return;
	}
	DWORD dwByte = 0;

	// Name
	CGameUtils::WriteStrFile(hFile, szModelName); /*Write*/
	// num meshes
	WriteFile(hFile, &pAIScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr); /*Write*/
	for (_uint i = 0; i < pAIScene->mNumMeshes; ++i)
	{
		aiMesh* pAIMesh = pAIScene->mMeshes[i];

		// mesh name
		CGameUtils::WriteStrFile(hFile, pAIMesh->mName.data); /*Write*/
		WriteFile(hFile, &pAIMesh->mMaterialIndex, sizeof(_uint), &dwByte, nullptr); /*Write*/
		WriteFile(hFile, &pAIMesh->mNumVertices, sizeof(_uint), &dwByte, nullptr); /*Write*/
		WriteFile(hFile, &pAIMesh->mNumFaces, sizeof(_uint), &dwByte, nullptr); /*Write*/


		VTXMODEL vtx;
		for (_uint j = 0; j < pAIMesh->mNumVertices; ++j)
		{
			memcpy(&vtx.vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
			memcpy(&vtx.vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
			memcpy(&vtx.vTexUV, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&vtx.vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
			WriteFile(hFile, &vtx, sizeof(VTXMODEL), &dwByte, nullptr); /*Write*/
		}

		FACEINDICES32 face;
		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			face._0 = pAIMesh->mFaces[j].mIndices[0];
			face._1 = pAIMesh->mFaces[j].mIndices[1];
			face._2 = pAIMesh->mFaces[j].mIndices[2];
			WriteFile(hFile, &face, sizeof(FACEINDICES32), &dwByte, nullptr); /*Write*/
		}
	}


	WriteFile(hFile, &pAIScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr); /*Write*/
	for (_uint i = 0; i < pAIScene->mNumMaterials; ++i)
	{
		CGameUtils::WriteStrFile(hFile, pAIScene->mMaterials[i]->GetName().data); /*Write*/
	}

	CloseHandle(hFile);
	Importer.FreeScene();
}

void CImgui_ModelConverter::ConvertAnimModel(const char* pModelPath)
{
	constexpr _uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	Assimp::Importer Importer;
	const aiScene* pAIScene = Importer.ReadFile(pModelPath, iFlag);

	char szModelName[MAX_PATH];
	char szDirPath[MAX_PATH];
	_splitpath_s(pModelPath, nullptr, 0, szDirPath, MAX_PATH, szModelName, MAX_PATH, nullptr, 0);


	string saveFileName = szDirPath;
	saveFileName += szModelName;
	saveFileName += ".anim_model";
	HANDLE hFile = CreateFile(CGameUtils::s2ws(saveFileName).c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", saveFileName.c_str());
		Importer.FreeScene();
		return;
	}
	DWORD dwByte = 0;

	// Name
	CGameUtils::WriteStrFile(hFile, szModelName); /*Write*/

	Json jRootBone = Json::object();
	ConvertBone(jRootBone, pAIScene->mRootNode);

	WriteFile(hFile, &pAIScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr); /*Write*/
	for (_uint i = 0; i < pAIScene->mNumMeshes; ++i)
	{
		aiMesh* pAIMesh = pAIScene->mMeshes[i];

		// mesh name
		CGameUtils::WriteStrFile(hFile, pAIMesh->mName.data); /*Write*/
		WriteFile(hFile, &pAIMesh->mMaterialIndex, sizeof(_uint), &dwByte, nullptr); /*Write*/
		WriteFile(hFile, &pAIMesh->mNumVertices, sizeof(_uint), &dwByte, nullptr); /*Write*/
		WriteFile(hFile, &pAIMesh->mNumFaces, sizeof(_uint), &dwByte, nullptr); /*Write*/


		vector<VTXANIMMODEL> vtxes;
		vtxes.resize(pAIMesh->mNumVertices);
		ZeroMemory(vtxes.data(), sizeof(VTXANIMMODEL) * pAIMesh->mNumVertices);

		for (_uint j = 0; j < pAIMesh->mNumVertices; ++j)
		{
			memcpy(&vtxes[j].vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
			memcpy(&vtxes[j].vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
			memcpy(&vtxes[j].vTexUV, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&vtxes[j].vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
		}

		// weights(bone)
		WriteFile(hFile, &pAIMesh->mNumBones, sizeof(_uint), &dwByte, nullptr); /*Write*/
		for (_uint k = 0; k < pAIMesh->mNumBones; ++k)
		{
			aiBone*	pAIBone = pAIMesh->mBones[k];
			CGameUtils::WriteStrFile(hFile, pAIBone->mName.data); /*Write*/

			_float4x4 OffsetMatrix;
			memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
			SetOffsetMatrix(pAIBone->mName.data, jRootBone, OffsetMatrix);

			for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
			{
				_uint iVertexIndex = pAIBone->mWeights[j].mVertexId;

				if(0.0f == vtxes[iVertexIndex].vBlendWeight.x)
				{
					vtxes[iVertexIndex].vBlendIndex.x = k;
					vtxes[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
				}
				else if (0.0f == vtxes[iVertexIndex].vBlendWeight.y)
				{
					vtxes[iVertexIndex].vBlendIndex.y = k;
					vtxes[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
				}
				else if (0.0f == vtxes[iVertexIndex].vBlendWeight.z)
				{
					vtxes[iVertexIndex].vBlendIndex.z = k;
					vtxes[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
				}
				else if (0.0f == vtxes[iVertexIndex].vBlendWeight.w)
				{
					vtxes[iVertexIndex].vBlendIndex.w = k;
					vtxes[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
				}
			}
		}

		for (_uint j = 0; j < pAIMesh->mNumVertices; ++j)
		{
			WriteFile(hFile, &vtxes[j], sizeof(VTXANIMMODEL), &dwByte, nullptr); /*Write*/
		}

		// 이부분은 mesh클론할 때 하도록
		// if (0 == iNumBones)
		// {
		// 	if (FindBone(jModel["RootBone"], pAIMesh->mName.data))
		// 	{
		// 		jMesh["NumBones"] = 1;
		// 		jMesh["Bones"].push_back(pAIMesh->mName.data);
		// 	}
		// }


		FACEINDICES32 face;
		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			face._0 = pAIMesh->mFaces[j].mIndices[0];
			face._1 = pAIMesh->mFaces[j].mIndices[1];
			face._2 = pAIMesh->mFaces[j].mIndices[2];
			WriteFile(hFile, &face, sizeof(FACEINDICES32), &dwByte, nullptr); /*Write*/
		}


	}

	WriteFile(hFile, &pAIScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr); /*Write*/
	for (_uint i = 0; i < pAIScene->mNumMaterials; ++i)
	{
		CGameUtils::WriteStrFile(hFile, pAIScene->mMaterials[i]->GetName().data); /*Write*/
	}

	// bone 정보 저장
	CGameUtils::WriteStrFile(hFile, jRootBone.dump()); /*Write*/

	CloseHandle(hFile);


	for (_uint i = 0; i < pAIScene->mNumAnimations; ++i)
	{
		ConvertAnimation(szDirPath, pAIScene->mAnimations[i]);
	}

	Importer.FreeScene();
}

void CImgui_ModelConverter::ConvertBone(Json& jBone, aiNode* pAINode)
{
	jBone["Name"] = pAINode->mName.data;

	jBone["OffsetMatrix"] = _float4x4::Identity;

	_float4x4 TransformMatrix;
	memcpy(&TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	jBone["TransformMatrix"] = TransformMatrix;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		jBone["Children"].push_back(Json::object());
		ConvertBone(jBone["Children"].at(i), pAINode->mChildren[i]);
	}
}

_bool CImgui_ModelConverter::FindBone(const Json& jBone, const char* pBoneName)
{
	const string boneName = jBone["Name"];
	if (strcmp(pBoneName, boneName.c_str()) == 0)
	{
		return true;
	}

	if (jBone.contains("Children"))
	{
		for (auto& child : jBone["Children"])
		{
			if (FindBone(child, pBoneName))
			{
				return true;
			}
		}
	}

	return false;
}


void CImgui_ModelConverter::SetOffsetMatrix(const char* pBoneName, Json& jBone, const _float4x4& OffsetMatrix)
{
	string boneName = jBone["Name"].get<string>();

	if (strcmp(pBoneName, boneName.c_str()) == 0)
	{
		jBone["OffsetMatrix"] = OffsetMatrix;
	}
	else
	{
		if (jBone.contains("Children"))
		{
			for (auto& child : jBone["Children"])
			{
				SetOffsetMatrix(pBoneName, child, OffsetMatrix);
			}
		}
	}
}

void CImgui_ModelConverter::ConvertAnimation(string strDirName, aiAnimation* pAIAnimation)
{
	// blender로 애니메이션을 추가하면 "~~~|animName" 형태로 추출된다. 이를 수정해본다.
	string animName = CGameUtils::Split(pAIAnimation->mName.data, '|').back();
	string animNameFile = strDirName + animName + ".anim";

	HANDLE hFile = CreateFile(CGameUtils::s2ws(animNameFile).c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", animNameFile.c_str());
		return;
	}

	DWORD dwByte = 0;

	CGameUtils::WriteStrFile(hFile, animName); /*Write*/
	WriteFile(hFile, &pAIAnimation->mDuration, sizeof(double), &dwByte, nullptr); /*Write*/
	WriteFile(hFile, &pAIAnimation->mTicksPerSecond, sizeof(double), &dwByte, nullptr); /*Write*/

	WriteFile(hFile, &pAIAnimation->mNumChannels, sizeof(_uint), &dwByte, nullptr); /*Write*/
	for (_uint j = 0; j < pAIAnimation->mNumChannels; ++j)
	{
		aiNodeAnim*		pAIChannel = pAIAnimation->mChannels[j];

		CGameUtils::WriteStrFile(hFile, pAIChannel->mNodeName.data); /*Write*/

		_uint iNumKeyFrames = 0;
		iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);	
		iNumKeyFrames = max(iNumKeyFrames, pAIChannel->mNumPositionKeys);

		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;


		WriteFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr); /*Write*/
		for (_uint i = 0; i < iNumKeyFrames; ++i)
		{
			KEYFRAME			KeyFrame;
			ZeroMemory(&KeyFrame, sizeof(KEYFRAME));
		
			if(i < pAIChannel->mNumScalingKeys)
			{
				memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(XMFLOAT3));
				KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
			}
		
			if (i < pAIChannel->mNumRotationKeys)
			{
				vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
				vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
				vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
				vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
				KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
			}
		
			if (i < pAIChannel->mNumPositionKeys)
			{
				memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(XMFLOAT3));
				KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
			}
		
			KeyFrame.vScale = vScale;
			KeyFrame.vRotation = vRotation;
			KeyFrame.vPosition = vPosition;

			WriteFile(hFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr); /*Write*/
		}
	}

	CloseHandle(hFile);
}


#endif
