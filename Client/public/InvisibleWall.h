#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer;
END

BEGIN(Client)

class CInvisibleWall : public CGameObject
{
public:
	CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvisibleWall(const CInvisibleWall& rhs);
	virtual ~CInvisibleWall() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;
	virtual void	Imgui_RenderProperty() override;
	HRESULT			SetUp_ShaderResources();

	void			RayPicking();

	void			ClearBuffer();

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	class CVIBuffer_Trail_Instancing*	m_pBuffer = nullptr;

private:
	_bool								m_bActive = true;
	_bool								m_bPick = false;		// Picking
	vector<class CMapObject*>			m_pMapObjects;			// Picking
	CGameObject*						m_pGameObject = nullptr;

	_float								m_fWidth = 0.f;			// 트레일 벽 높이
	list<_vector>						m_TrailPointList;
	_float3								m_vPointPos;

	_uint								m_iPass = 0;			// Shader

public:
	static CInvisibleWall*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END