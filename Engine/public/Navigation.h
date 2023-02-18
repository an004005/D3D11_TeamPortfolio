#pragma once
#include "Cell.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CGameObject
{
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SaveToJson(Json& json) override;
	void LoadJson(const Json& json);

#ifdef _DEBUG
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void AddCell(const array<_float3, 3>& CellPos);
	void SetCheckCell(_int iCellIdx) { m_CheckedIdxes.push_back(iCellIdx); }
	_bool FindNearCellPoint(_float3& vPos);
	class CCell* PickCell();
	void DeleteCell(_uint iCellIdx);
	void DeleteRecentCell();
	void GetAllNearCellPoints(OUT list<pair<CCell*, CCell::POINT>>& CellPointPairs);
#endif

public:
	_bool IsMove_OnNavigation(_int& iCellIdx_InOut, _fvector TargetPos, _float3& vFailedLineVector);
	_float3 GetCellPos(_int iCellIdx);
	_int GetCellIdx(_float4 vPos) const;
	_float GetCellHeight(_fvector vPos, _int iCellIdx);
	_vector GetCellNormal(_int iCellIdx);

private:	
	vector<class CCell*>	m_Cells;

private:
	HRESULT	Ready_Neighbor();


#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
	class CRenderer*				m_pRendererCom = nullptr;

	list<_int>			m_CheckedIdxes;
#endif // _DEBUG

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END