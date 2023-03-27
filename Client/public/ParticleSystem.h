#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"
#include "Timeline.h"


BEGIN(Engine)
class CVIBuffer_Point_Instancing;
class CRenderer;
class CVIBuffer_Mesh_Instancing;
class CModel;
END

BEGIN(Client)



enum class ESpawnShape
{
	SPHERE,
	CONE,
};

enum class EPointType
{
	TEXTURE,
	FLIPBOOK
};

enum class EBufferType
{
	POINT,
	MESH
};

enum class EBilboardType
{
	ALL,
	LOOK,
	NONE
};


class CParticleSystem :	public CGameObject
{

public:
	enum FINISHFUNC { FUNC_PLAYFROMSTART, FUNC_RESET, FUNC_STOP, FUNC_REVERSE, FUNC_END };
	enum CURVETYPE { CURVE_SCALE_ALL, CURVE_SCALE_Y, CURVE_SCALE_X, CURVE_FLOATS_0, CURVE_FLOATS_1, CURVE_FLOATS_2, CURVE_FLOATS_3, CURVE_FLOATS_4, CURVE_FLOATS_5, CURVE_FLOATS_6, CURVE_FLOATS_7, CURVE_FLOATS_8, CURVE_FLOATS_9, CURVE_COLORCHANGE, CURVE_INTS_0, CURVE_END };


public:
	CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() = default;

public:
	CShader* GetShader() { return m_pShader; }
	_float	GetDuration() { return m_fDuration; }
	_int	GetLiveParticleCnt();
public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void AfterPhysX() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void Imgui_RenderProperty() override;
	void SetLoop(_bool bLoop) { m_bLoop = bLoop; }

	HRESULT SetParams();
	HRESULT	Begin();

	void		Start_Timeline();
private:
	void AddPoint();
	void UpdatePoints(_float fTimeDelta);

private:
	void AddMesh();
	void UpdateMeshes(_float fTimeDelta);

private:


private:
	ShaderParams m_tParam;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShader = nullptr;
	CVIBuffer_Point_Instancing* m_pPointInstanceBuffer = nullptr;
	CVIBuffer_Mesh_Instancing*	m_pMeshInstanceBuffer = nullptr;
	CModel*						m_pModel = nullptr;
	CModel*						m_pMeshCurveModel = nullptr;

	_float m_fMeshVtxRangeRatio = 0.f;
	_float m_fMeshVtxStartRatio = 0.f;

	_bool m_bCurveDir = false;

	string m_strXDirCurveName = "";
	string m_strYDirCurveName = "";
	string m_strZDirCurveName = "";

	CSimpleTimeline m_VtxStartTimeline;
	CSimpleTimeline m_VtxRangeTimeline;



	_bool m_bUseMeshData = false;

	_bool m_bLocal = true;
	_uint m_iInstanceNum = 50;

	_float m_fDuration = 5.f;
	_bool m_bLoop = true;

	_float m_fSpawnTickTime = 0.1f;
	_float m_fCurSpawnTick = 0.f;
	_int m_iBurstCnt = 1;

	_float2 m_fLifeMinMax{ 1.f, 2.f };
	_bool m_bLifeRand = false;
	_float m_fLife = 1.f;

	_float2 m_fSpeedMinMax{ 1.f, 2.f };
	_float m_fSpeed = 1.f;
	_bool m_bSpeedRand = false;

	_float2 m_fSize{ 0.1f, 0.1f };

	ESpawnShape m_eShape = ESpawnShape::SPHERE;
	EBufferType m_eBufferType = EBufferType::POINT;
	EBilboardType m_eBilboardType = EBilboardType::ALL;

	_float2 m_fGravityPowerMinMax = { 0.f, 0.f };
	_float m_fGravityPower = 0.f;
	_float m_fConeAngleDeg = 30.f;
	_float m_fConeOriginRadius = 1.f;
	_float m_fSphereRadius = 4.f;
	_float3 m_fTurnAxis = { 0.f,0.f,0.f };
	_float  m_fTurnAngle = 0.f;
	_float3 m_vScaleVariation = { 1.f ,1.f,1.f};

	_bool m_bNonAlphaBlend = false;
	_bool m_bTurn = false;

	_bool m_bPointInstance = false;
	_bool m_bMeshInstance = false;
	_bool m_bFromOrigin = true; // false搁 官款爹 酒公单辑 积己
	_bool m_bGravity = false;
	_bool m_bSizeDecreaseByLife = false;
	_bool m_bSizeIncreaseByLife = false;
	_float m_fIncreaseMaxSize = 1.f;

	string m_MeshBufferProtoTag;

	string m_PointBufferProtoTag = "Prototype_Component_PointInstance";
	string m_ShaderProtoTag = "Prototype_Component_Shader_VtxPointInstance_Particle";
	string m_ModelProtoTag = "";
	string m_MeshCurveModelProtoTag = "";

	// string m_MeshBufferProtoTag;

	list<VTXMATRIX> m_PointList;
	list<VTXINSTANCE> m_MeshList;

	vector<pair<float, _uint>> m_vecVerticesDistance;

	_float m_fsurfaceThreshold = 0.1f;
	const _float3* m_vVerticesPos = nullptr;
private:
	// For Gravity

	// _bool	m_bGravity;
	_float	m_fJumpPower;
	_float	m_fDownSpeed;

	// For Rotation
	_bool	  m_bRotate = false;
	_float4x4 m_RotationMatrix = XMMatrixIdentity();
	_float3	 m_vMeshSize{ 1.f,1.f,1.f };

	_float3  m_vRandDir_Max = { 1.f,1.f,1.f };

	_float3  m_vRandDir_Min = { 0.f,0.f,0.f };

	_bool	 m_bSphereDetail = false;

	// _float	  m_ArrayRoationToTime[3];
	_float3	  m_fRotationToTime_Min = {0.f,0.f,0.f};
	_float3	  m_fRotationToTime_Max = { 0.f,0.f,0.f };

	_bool m_bMeshCurve = false;
	_bool m_bPhysX = false;
public:
	// For Curves
	void Tick_Scale_All(_float fValue);
	void Tick_Scale_Y(_float fValue);
	void Tick_Scale_X(_float fValue);

	void Tick_Floats_0(_float fValue);
	void Tick_Floats_1(_float fValue);
	void Tick_Floats_2(_float fValue);
	void Tick_Floats_3(_float fValue);
	void Tick_Floats_4(_float fValue);
	void Tick_Floats_5(_float fValue);
	void Tick_Floats_6(_float fValue);
	void Tick_Floats_7(_float fValue);
	void Tick_Floats_8(_float fValue);
	void Tick_Floats_9(_float fValue);
	void Tick_ColorChange(_float fValue);
	// For Int 
	void Tick_Ints_0(_float fValue);

	void AddTargetCurve(const string& strCurveName, CCurveFloatImpl* pCurve = nullptr);
private:
	// For MainCurve
	_bool		m_bUseMainCurve = false;
	CTimeline	m_Timeline;
	cmap<CCurveFloatImpl*> m_Curves;

	_float		m_fEndTime = 0.f;
	_int	m_iSelectFinishFunc = FUNC_RESET;
	char*		m_szFuncName[FUNC_END] = { "PlayFromStart",  "Reset", "Stop", "Reverse" };
	char*		m_szCurveTag[CURVE_END] = { "ObjectScale_All", "ObjectScale_Y","ObjectScale_X", "Floats_0", "Floats_1", "Floats_2", "Floats_3","Floats_4","Floats_5","Floats_6", "Floats_7",  "Floats_8", "Floats_9", "Color_Change", "Ints_0" };

public:
	static CParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};




END
