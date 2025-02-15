#pragma once
#include <Renderer.h>

#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"
#include "Timeline.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer;
class CModel;
END

BEGIN(Client)

enum class EBillBoardType
{
	AXIS_ALL,
	AXIS_LOOK,
	AXIS_UP,
	AXIS_RIGHT,
	TYPE_END
};

class CEffectSystem : public CGameObject
{
private:
	CEffectSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectSystem(const CEffectSystem& rhs);
	virtual ~CEffectSystem() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT SetParams();
	HRESULT Begin();
	ShaderParams& GetParams() { return m_tParam; }
	CShader* GetShader() { return m_pShaderCom; }

	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void Imgui_RenderProperty() override;

	_uint GetChildNums() const { return static_cast<_uint>(m_ChildBuffers.size()); }
	void SetPivot(_uint iChildIdx, _float4x4 childMatrix)
	{
		Assert(iChildIdx < GetChildNums());
		m_ChildBuffers[iChildIdx] = childMatrix;
	}
	vector<_float4x4>& GetChildWorlds() { return m_ChildBuffers; }

	void SetShadowPass(_uint iPass) { m_iShadowPass = iPass; }

	//For_Curve
public:
	_bool	Check_UseMoveCurve() { return m_bUseMoveCurve; }

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
	void Tick_ColorChange(_float fValue);

	void Tick_IntroTime(_float fValue);
	void Tick_OutroTime(_float fValue);
	// For Int 
	void Tick_Ints_0(_float fValue);

	void SetRenderGroup(CRenderer::RENDERGROUP eRenderGroup) { m_eRenderGroup = eRenderGroup; }


	void Play_MoveCurve();
public:
	void		Check_ModelSprite();
private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer*					m_pBuffer = nullptr;
	CModel*						m_pModel = nullptr;
	
	string m_BufferProtoTag = "Prototype_Component_VIBuffer_Rect";
	string m_ShaderProtoTag = "Prototype_Component_Shader_VtxTex_VFX";
	string m_ModelProtoTag;

	ShaderParams m_tParam;
	_bool m_bBillBoard = false;
	EBillBoardType m_eBillBoardType = EBillBoardType::AXIS_ALL;

	_bool m_bDecal = false;

	vector<_float4x4> m_ChildBuffers;

	_bool m_bUseDepth = false;

	_uint m_iShadowPass = 0;

	CRenderer::RENDERGROUP m_eRenderGroup = CRenderer::RENDER_ALPHABLEND;


private:
	_bool	m_bUseMoveCurve = false;
	CSimpleTimeline m_MoveTimeline;
	_float	m_fMoveRange = 0.5f;

	_float3 m_fTurnAxis = { 0.f,0.f,0.f };
	_float  m_fTurnAngle = 0.f;
	_bool	m_bTurn = false;
private:
	// For Mesh Sprite
	vector<string> m_vecModelDir;
	vector<CModel*> m_vecModelCom;
	_bool			m_bModelSprite = false;
	string			m_ModelFolderDir;// = "../Bin/Resources/Meshes/VFX/";

private:
	_float4			m_vOriginColor = _float4::Zero;
	_bool			m_bNormal = false;
	_float			m_fDefaultY = 0.f;
public:
	static CEffectSystem*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void*);
	virtual void				Free() override;
};



END

//
// // virtual HRESULT Render_ShadowDepth() override;
// 	// virtual void LoadFromJson(const Json& json) override;
// 	// virtual void Imgui_RenderComponentProperties() override;
// 	// virtual void SetDelete() override;