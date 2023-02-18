#pragma once
#include "Controller.h"
#include "CorePch.h"

BEGIN(Client)

class CRemoteController : public CController
{
private:
	CRemoteController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRemoteController(const CRemoteController& rhs);
	virtual ~CRemoteController() override = default;

public:
	virtual void Tick(_double TimeDelta) override;

public:
	void AddNextWorldMatrix(const _float4x4& WorldMatrix);
	void AddNextInputSyncData(INPUT_SYNC_DATA tInputSyncData);

private:
	void UpdateRemoteInputState(EInputMask eMask, EHandleInput eInput);

private:
	USE_LOCK;
	queue<_float4x4> m_NextWorldMatrices;
	queue<INPUT_SYNC_DATA> m_NextInputs;


public:
	virtual CRemoteController* Clone(void* pArg) override;
	static CRemoteController* Create();
};

END