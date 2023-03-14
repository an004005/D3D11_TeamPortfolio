#include "stdafx.h"
#include "..\public\AIController.h"
#include "ScarletCharacter.h"

AICommand::AICommand(const AICommand& other): m_Callback(other.m_Callback),
                                              m_fDuration(other.m_fDuration),
                                              m_strName(other.m_strName)
{
}

AICommand::AICommand(AICommand&& other) noexcept: m_Callback(std::move(other.m_Callback)),
                                                  m_fDuration(other.m_fDuration),
                                                  m_strName(std::move(other.m_strName))
{
}

AICommand& AICommand::operator=(const AICommand& other)
{
	if (this == &other)
		return *this;
	m_Callback = other.m_Callback;
	m_fDuration = other.m_fDuration;
	m_strName = other.m_strName;
	return *this;
}

AICommand& AICommand::operator=(AICommand&& other) noexcept
{
	if (this == &other)
		return *this;
	m_Callback = std::move(other.m_Callback);
	m_fDuration = other.m_fDuration;
	m_strName = std::move(other.m_strName);
	return *this;
}

CAIController::CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CController(pDevice, pContext)
{
}

CAIController::CAIController(const CAIController& rhs)
	: CController(rhs)
{
}

void CAIController::Tick(_double TimeDelta)
{
	Invalidate();
	m_fTurnRemain = 0.f;

	if (m_pTarget == nullptr)
	{
		ClearCommands();
		return;
	}
	if (m_pTarget->IsDeleted())
	{
		ClearCommands();
		SetTarget(nullptr);
		return;
	}	

	m_fTimeDelta = (_float)TimeDelta;

	auto pOwner = TryGetOwner();

	if (pOwner == nullptr)
		return;

	m_fTtoM_Distance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- pOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));
	
	AI_Tick(TimeDelta);

	while (m_Commands.empty() == false)
	{
		_bool bFinish = m_Commands.front().Execute(TimeDelta);
		if (bFinish)
			m_Commands.pop_front();
		else
			break;
	}
}

void CAIController::Imgui_RenderProperty()
{
	static string strDistance;
	if (m_eDistance == DIS_NEAR)
		strDistance = "Near";

	else if (m_eDistance == DIS_MIDDLE)
		strDistance = "Middle";

	else if (m_eDistance == DIS_FAR)
		strDistance = "Far";

	else if (m_eDistance == DIS_OUTSIDE)
		strDistance = "Outside";

	ImGui::Text("Distance : "); ImGui::SameLine(); ImGui::Text(strDistance.c_str());
	static string strSelected;
	if (ImGui::BeginListBox("Current Commands"))
	{
		for (auto& command : m_Commands)
		{
			const bool bSelected = false;
			const string commandInfo = command.GetName() + " / " + to_string(command.GetDuration());
			if (ImGui::Selectable(commandInfo.c_str(), bSelected))
				strSelected = command.GetName();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Clear Commands"))
	{
		ClearCommands();
	}

	ImGui::Separator();

	static EMoveAxis eMoveDir = EMoveAxis::NORTH;
	static _float fMoveDirDuration = 1.f;
	static array<const char*, MOVE_AXIS_CNT> arrMoveAxisName {
		"NORTH", "NORTH_EAST", "EAST", "SOUTH_EAST", "SOUTH", "SOUTH_WEST", "WEST", "NORTH_WEST", "CENTER"
	};

	if (ImGui::BeginCombo("Move Dir", arrMoveAxisName[static_cast<_uint>(eMoveDir)]))
	{
		for (int i = 0; i < MOVE_AXIS_CNT; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(arrMoveAxisName[i], bSelected))
				eMoveDir = static_cast<EMoveAxis>(i);
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("MoveDuration", &fMoveDirDuration);
	if (ImGui::Button("Add Move Command"))
	{
		string moveName = arrMoveAxisName[static_cast<_uint>(eMoveDir)];
		moveName = "Move " + moveName;
		AddCommand(moveName, fMoveDirDuration, &CAIController::Move, eMoveDir);
	}

	ImGui::Separator();

	static _float fTurnDuration = 1.f;
	static _float fTurnSpeedRatio = 1.f;
	ImGui::DragFloat("TurnDuration", &fTurnDuration);
	ImGui::DragFloat("TurnSpeedRatio", &fTurnSpeedRatio);
	if (ImGui::Button("Add Turn Command"))
	{
		AddCommand("Turn", fTurnDuration, &CAIController::TurnToTarget, fTurnSpeedRatio);
	}

	ImGui::Separator();
	
	static EHandleInput eInput = SPACE;
	static array<const char*, HANDLE_END> arrInputName {
		"SPACE", "CTRL", "SHIFT", "MOUSE_LB", "MOUSE_RB", "NUM_1", "NUM_2", "NUM_3", "NUM_4", "C", "Q", "E", "R", "X", "G"
	};
	if (ImGui::BeginCombo("Inputs", arrInputName[eInput]))
	{
		for (int i = 0; i < HANDLE_END; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(arrInputName[i], bSelected))
				eInput = static_cast<EHandleInput>(i);
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Add Input"))
	{
		string inputName = arrInputName[eInput];
		inputName = "Input " + inputName;
		AddCommand(inputName, 0.f, &CAIController::Input, eInput);
	}
}

void CAIController::SetTarget(CScarletCharacter* pTarget)
{
	Safe_Release(m_pTarget);
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

_bool CAIController::IsCommandRunning()
{
	//_bool bTest = m_Commands.empty() == false;
	return m_Commands.empty() == false;
}

void CAIController::ClearCommands()
{
	m_Commands.clear();	
}

void CAIController::Move(EMoveAxis eAxis)
{
	switch (eAxis)
	{
	case EMoveAxis::NORTH:
		m_vMoveAxis.z += 1.f;
		// 거리 비교해서 비교값 이하일때 멈추는 기능 추가하기 	
		if (abs(m_fTtoM_Distance) < 2.5f)
		{
			m_Commands.front().SetFinish();
		}
		break;
	case EMoveAxis::NORTH_EAST: 
		m_vMoveAxis.z += 1.f;
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::EAST: 
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::SOUTH_EAST: 
		m_vMoveAxis.z -= 1.f;
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::SOUTH: 
		m_vMoveAxis.z -= 1.f;
		break;
	case EMoveAxis::SOUTH_WEST: 
		m_vMoveAxis.z -= 1.f;
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::WEST:
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::NORTH_WEST: 
		m_vMoveAxis.z += 1.f;
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::CENTER:
		FALLTHROUGH;
	case EMoveAxis::AXIS_END:
		break;
	default:
		NODEFAULT;
	}
}

void CAIController::TurnToTarget(_float fSpeedRatio)
{
	auto pOwner = TryGetOwner();
	if (m_pTarget == nullptr || pOwner == nullptr)
		return;

	const _vector vLookAt = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	m_fTurnRemain = pOwner->GetTransform()->Get_RemainYawToLookAt(vLookAt);
	pOwner->GetTransform()->LookAt_Smooth(vLookAt, _double(m_fTimeDelta * fSpeedRatio));	
}

void CAIController::Input(EHandleInput eInput)
{
	m_InputState[eInput] = KEY_STATE::DOWN;
}

void CAIController::Move_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CAIController::Move_TurnToTargetStop(EMoveAxis eAxis, _float fSpeedRatio)
{
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
	if (abs(m_fTurnRemain) < 0.1f)
	{
		m_Commands.front().SetFinish();
	}
}

void CAIController::TurnToTargetStop(_float fSpeedRatio)
{
	TurnToTarget(fSpeedRatio);
	if (abs(m_fTurnRemain) < 0.1f)
	{
		m_Commands.front().SetFinish();
	}
}

void CAIController::Wait()
{
	// AddCommand 시간동안 쉰다.
}

void CAIController::Free()
{
	CController::Free();
	ClearCommands();
	Safe_Release(m_pTarget);
}
