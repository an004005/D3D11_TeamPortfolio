#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CBdLm_Controller : public CAIController
{
public:
	CBdLm_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBdLm_Controller(const CBdLm_Controller& rhs);
	virtual ~CBdLm_Controller() = default;
};

END