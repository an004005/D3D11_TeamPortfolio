#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Invisible.h"
#include "Shader.h"
#include "Timeline.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Invisible;
class CRigidBody;
class CTexture;
class CPhysXStaticMesh;
END

BEGIN(Client)

class CInvisibleWall : public CGameObject
{
public:
	CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvisibleWall(const CInvisibleWall& rhs);
	virtual ~CInvisibleWall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;
	virtual void	Imgui_RenderProperty() override;

public:
	void CreateWall();
	void CreateInstanceData();
	void Activate(_bool bActive);
	_bool IsActive();

private:
	void DeletePoint(_uint idx);
	_bool PickPoint();
	void AddPoint_ForTool(_float4 vPos);

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Invisible*				m_pBuffer = nullptr;
	ShaderParams						m_tParams;

	CPhysXStaticMesh* m_pPxMesh = nullptr;

	CSimpleTimeline m_Start;
	CSimpleTimeline m_End;
			
private:
	vector<pair<_float4, CRigidBody*>> m_Points;
	_int m_iPointIdx = 0;

	_float m_fIndicatorSize = 0.2f;

	_bool m_bOnTool = false;
	_float m_fCreateCoolTime = 0.5f;
	_float m_fMaxCreateCoolTime = 0.5f;

	_float m_fSegmentSize = 0.5f;

public:
	static CInvisibleWall*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END