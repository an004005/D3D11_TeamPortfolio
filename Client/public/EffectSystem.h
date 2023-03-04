#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"

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

	_float m_fCurModelChangeTime = 0.f;
	_float m_fModelChangeTime = 2.f;
	_bool	m_bModelChange = false;


private:
	_float4			m_vOriginColor = _float4::Zero;
	
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