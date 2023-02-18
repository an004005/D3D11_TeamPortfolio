#pragma once
#include "Client_Defines.h"
#include "LoadingLevel.h"

BEGIN(Client)

class CLevel_Loading_MultiPlay final : public CLoadingLevel
{
private:
	CLevel_Loading_MultiPlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::function<CLevel*()>&& LevelCreator);
	virtual ~CLevel_Loading_MultiPlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bLoadingFinished = false;

public:
	virtual void Free() override;

	template<typename LEVEL>
	static CLevel_Loading_MultiPlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		CLevel_Loading_MultiPlay*		pInstance = new CLevel_Loading_MultiPlay(pDevice, pContext, [pDevice, pContext]()
		{
			return LEVEL::Create(pDevice, pContext);
		});

		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("Failed to Created : CLevel_Loading_MultiPlay");
			Safe_Release(pInstance);
		}
		return pInstance;
	}
};

END