#pragma once

#include "Component.h"

BEGIN(Engine)
	class CAnimation;

enum class EBoneMask
{
	OFF_PARENT_EQ,
	OFF_PARENT,
	OFF_CHILD_EQ,
	OFF_CHILD,
	OFF_EQ,
	OFF_ALL,
	ON_PARENT_EQ,
	ON_PARENT,
	ON_CHILD_EQ,
	ON_CHILD,
	ON_EQ,
	ON_ALL,
	BONE_MASK_END
};

typedef struct tagOptionalRootMotion
{
	string szAnimName;
	_float fStartTime;
	_float fEndTime;
	_float4 vOptionalRootVector;
}OPTIONAL_ROOTMOTION;

class ENGINE_DLL CModel : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	void Tick(_double TimeDelta);

	// For Particle
	const _float3* Get_VerticesPos();
	const _uint	   Get_NumVertices();

	const VTXMODEL* Get_NonAnimBuffer();
	// const VTXANIMMODEL* Get_AnimBuffer();

	// ~For Particle

	_uint Get_NumMeshes() const { return static_cast<_uint>(m_Meshes.size()); }
	class CBone* Get_BonePtr(const char* pBoneName);
	class CBone* Get_BonePtr(const string& strBoneName);
	_matrix GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true);
	_float4x4 GetPivotMatrix() const { return m_PivotMatrix; }
	CAnimation* Find_Animation(const string& strAnimaName);
	void SetPivot(_float4x4 Pivot) { m_PivotMatrix = Pivot; }
	_vector GetLocalMove(_fmatrix WorldMatrix);
	_vector GetLocalMove(_fmatrix WorldMatrix, const string& srtAnimName);
	_float	GetLocalScalar(_fmatrix WorldMatrix);
	_bool	isLocalMove() { return !XMVector3Equal(m_vLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)); }
	_float	GetLastLocalMoveSpeed() const { return m_fLastLocalMoveSpeed; }
	void	Reset_LocalMove(_bool isSocket = false);
	_vector	Sync_EffectLocalMove();	// Reference가 이동하지 않지만 원점으로부터 벗어난 애니메이션 수정

	_matrix GetLocalRotationMatrix(_fmatrix WorldMatrix);
	_matrix GetLocalEularMatrix(_fmatrix WorldMatrix);

	_vector GetLocalRotationDelta();

public:
	_vector GetSpecialLocalMove(_fmatrix WorldMatrix);
	void	Reset_SpecialLocalMove();

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderProperty() override;

public:
	void SetPlayAnimation(const string& strAnimName);
	CAnimation* GetPlayAnimation();
	void SetCurAnimName(const string& strAnimName);

	void Play_Animation(_double TimeDelta);
	void Play_Animation_Test(_double TimeDelta);
	void Play_Animation_Additive(_double TimeDelta, _float fAddRatio);
	void Play_Animation_Sync(_double fRatio);
	// HRESULT RenderCustomShader(_uint iPass, class CShader* pShader);
	HRESULT Render(class CTransform* pTransform);
	HRESULT Render(const _float4x4& WorldMatrix);
	HRESULT RenderMesh(class CTransform* pTransform, _uint iMeshIdx);
	HRESULT RenderMeshOnly(_uint iMeshIdx) const;
	HRESULT Render_Pass(class CTransform* pTransform, _uint iPass);
	HRESULT Render_Cam(class CTransform* pTransform, class CCamera* pCamera);
	HRESULT Render_NoUpdateBone(const _float4x4& WorldMatrix);
	// HRESULT RenderMesh_Shader(class CTransform* pTransform, class CShader* pShader, _uint iPass);

	HRESULT Render_MeshAlphBlend(_float4x4 WorldMatrix, _uint iMeshIdx);

	HRESULT Render_ShadowDepth(class CTransform* pTransform);
	HRESULT Render_ShadowDepth(const _float4x4& WorldMatrix);

	void LoadAnimations(const char* pAnimDir);

	void AddBoneRotation(const string& strBoneName, Quaternion qQuat);
	void SetBoneMask(EBoneMask eMask, const string& strBoneName = "");
	void Compute_CombindTransformationMatrix();

	const vector<class CMaterial*>& GetMaterials() { return m_Materials; }
	class CMaterial* FindMaterial(const _tchar* pMtrlProtoTag);

	// 현재 실행중인 애니메이션 동작 고정한 모델 카피
	// 스태틱 메쉬처럼 Render_NoUpdateBone 함수로만 랜더 하기(업데이트하면 터짐)
	CModel* CloneModelPose();

public:
	void Set_AdditiveAnim(const string& strAdditive) { m_szAdditiveAnimName = strAdditive; }

private:
	void SaveModifiedData(Json& json);

private:
	void Ready_Bones(const Json& jBone, class CBone* pParent);
	void SetBoneChildren(const Json& jBone);
	HRESULT Ready_Materials(HANDLE hFile);

public:		// 이벤트 실행
	void EventCaller(const string& EventName);
	void Add_EventCaller(const string& EventName, std::function<void(void)> Func);
	_vector GetOptionalMoveVector(_fmatrix WorldMatrix);
	_vector GetOptionalMoveVector(_fmatrix WorldMatrix, const string& srtAnimName);
	void Add_OptionalRootMotion(OPTIONAL_ROOTMOTION RootMotion);
	void Delete_OptionalRootMotion();

public:
	void	Clear_mapEventFunc();

private:	// 이벤트
	unordered_map<string, std::function<void(void)>>	m_EventFunc;
	unordered_map<string, vector<OPTIONAL_ROOTMOTION>>	m_mapOptionalRootMotion;

public:
	static _float4x4 s_DefaultPivot;
private:
	static const string s_ModifyFilePath;

// 플레이어 이펙트 부착 툴을 위한 임시 함수
public:
	unordered_map<string, CAnimation*>	Get_AnimList() const { return m_mapAnimation; }

protected:
	string								m_strName;
	TYPE								m_eType = TYPE_END;
	_float4x4							m_PivotMatrix;
	_bool								m_bPx = false;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	vector<class CMesh*>				m_Meshes;
	vector<class CMaterial*>			m_Materials;

	// Json jBoneOrigin;
	string								 m_BoneObjectDump;
	class CBone*						 m_pRootBone = nullptr;
	unordered_map<string, class CBone*>	 m_mapBones;

	string								m_CurAnimName;
	string								m_LocalMoveAnimName;
	unordered_map<string, CAnimation*>  m_mapAnimation;

	KEYFRAME							m_CurKeyFrame;
	KEYFRAME							m_BefKeyFrame;

	_vector								m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector								m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float								m_fLastLocalMoveSpeed = 0.f;
	_float								m_fBefRatio = 0.f;

	string								m_szAdditiveAnimName = "";
	_float								m_fAdditiveRatio = 0.5f;

	_vector								m_vLocalRotation = XMQuaternionIdentity();
	_vector								m_vBefLocalRotation = XMQuaternionIdentity();

	Quaternion							m_vLocalQuaternion = XMQuaternionIdentity();
	Quaternion							m_vBefLocalQuaternion = XMQuaternionIdentity();

	pair<_vector, _float>				m_LocalEular;
	pair<_vector, _float>				m_BefLocalEular;

	_vector								m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector								m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	string								m_szSocketBefAnimName = "";
	_float								m_fSocketBefRatio = 0.f;

	// 키네틱 오브젝트는 Reference가 루트모션을 담당하지 않아서 새로 만듦
	_vector								m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector								m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	string								m_szBefSpecialAnimName = "";
	_float								m_fBefSpecialRatio = 0.f;

	class CShader* m_pShadowShader = nullptr;

	vector<array<_float4x4, 512>> m_BoneMatrices;


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _float4x4 PivotMatrix = s_DefaultPivot);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END