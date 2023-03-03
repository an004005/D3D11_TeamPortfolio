#include "stdafx.h"
#include "..\public\AIController.h"
#include "ScarletCharacter.h"

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
		ClearAllCommands();
		return;
	}
	if (m_pTarget->IsDeleted())
	{
		ClearAllCommands();
		SetTarget(nullptr);
		return;
	}

	m_fTimeDelta = (_float)TimeDelta;



	for (auto itr = m_Commands.begin(); itr != m_Commands.end();)
	{
		if (itr->second.IsDeleted())
		{
			itr = m_Commands.erase(itr);
		}
		else if (itr->second.Execute(TimeDelta))
		{
			itr = m_Commands.erase(itr);
			ExecuteExitCommand(itr->first);
		}
		else
		{
			++itr;
		}
	}
}

void CAIController::Imgui_RenderProperty()
{
	static string strSelected;
	if (ImGui::BeginListBox("Current Commands"))
	{
		for (auto& command : m_Commands)
		{
			const bool bSelected = command.first == strSelected;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			const string commandInfo = command.first + " / " + to_string(command.second.GetDuration());
			if (ImGui::Selectable(commandInfo.c_str(), bSelected))
				strSelected = command.first;
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Delete Select Command"))
	{
		if (ExistCommand(strSelected))
		{
			m_Commands.erase(strSelected);
			strSelected.clear();
		}
	}

	if (ImGui::CollapsingHeader("ExitCommands"))
	{
		static string strSelectedExit;
		if (ImGui::BeginListBox("Current ExitCommands"))
		{
			for (auto& command : m_ExitCommands)
			{
				const bool bSelected = command.first == strSelectedExit;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(command.first.c_str(), bSelected))
					strSelectedExit = command.first;
			}
			ImGui::EndListBox();
		}
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
		AddCommand("Move", fMoveDirDuration, &CAIController::Move, eMoveDir);
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
		AddCommand("Input", 0.f, &CAIController::Input, eInput);
	}
}

void CAIController::SetTarget(CScarletCharacter* pTarget)
{
	Safe_Release(m_pTarget);
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CAIController::DeleteCommand(const string& strName)
{
	if (ExistCommand(strName))
	{
		m_Commands.find(strName)->second.SetDelete();
	}
}

_bool CAIController::ExistCommand(const string& strName)
{
	return m_Commands.find(strName) != m_Commands.end();
}

void CAIController::ClearCommands()
{
	m_Commands.clear();
}

_bool CAIController::ExistExitCommand(const string& strName)
{
	return m_ExitCommands.find(strName) != m_ExitCommands.end();
}

void CAIController::DeleteExitCommand(const string& strName)
{
	if (ExistExitCommand(strName))
		m_ExitCommands.erase(strName);
}

void CAIController::ClearExitCommands()
{
	m_ExitCommands.clear();
}

void CAIController::ClearAllCommands()
{
	ClearExitCommands();
	ClearCommands();
}

void CAIController::Move(EMoveAxis eAxis)
{
	switch (eAxis)
	{
	case EMoveAxis::NORTH:
		m_vMoveAxis.z += 1.f;
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

void CAIController::ExecuteExitCommand(const string& strName)
{
	auto itr = m_ExitCommands.find(strName);
	if (itr != m_ExitCommands.end())
	{
		itr->second.Execute();
		m_ExitCommands.erase(strName);
	}
}

void CAIController::Free()
{
	CController::Free();
	ClearAllCommands();
	Safe_Release(m_pTarget);
}
