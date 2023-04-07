#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Invisible.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Invisible;
class CRigidBody;
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
	HRESULT			SetUp_ShaderResources();

private:
	_bool	RayPicking();
	void	DeletePoint(_uint idx);
	void	AddPoint_ForTool(_float3 vPos);

public:
	void CreateInvWall();

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Invisible*				m_pBuffer = nullptr;
			
private:
	_bool								m_bActive = false;
	_bool								m_bPick = false;		// Picking
	vector<class CMapObject*>			m_pMapObjects;			// Picking
	CGameObject*						m_pGameObject = nullptr;

	_float								m_fHeight = 0.f;			// 트레일 벽 높이
	list<_float3>						m_TrailPointList;
	list< CVIBuffer_Invisible*>			m_VIBuffInvList;

	vector<pair<_float3, CRigidBody*>>	m_Points;
	_int								m_iPointIdx = 0;
	_float								m_fIndicatorSize = 0.2f;

	_bool								m_bOnTool = false;
	_float								m_fCreateCoolTime = 0.5f;
	_float								m_fMaxCreateCoolTime = 0.5f;

	_uint								m_iPass = 0;			// Shader

public:
	static CInvisibleWall*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END