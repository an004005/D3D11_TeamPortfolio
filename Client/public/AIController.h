#pragma once
#include "Controller.h"

BEGIN(Client)

class AICommand
{
using CallbackType = std::function<void()>;
public:
	AICommand(CallbackType&& callback, _float fDuration)
		: m_Callback(std::move(callback))
		, m_fDuration(fDuration)
	{
	}

	template<typename T, typename Ret, typename... Args>
	AICommand(_float fDuration, T* owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		m_Callback = [owner, memFunc, args...]()
		{
			(owner->*memFunc)(args...);
		};
		m_fDuration = fDuration;
	}

	_bool Execute(_double TimeDelta)
	{
		m_Callback();
		m_fDuration -= (_float)TimeDelta;
		return m_fDuration <= 0.f;
	}
	_float GetDuration() const { return m_fDuration; }
	void SetDelete() { m_bDelete = true; }
	_bool IsDeleted() { return m_bDelete; }

private:
	CallbackType m_Callback;
	_float m_fDuration = 0.f;
	_bool m_bDelete = false;
};

class AIExitCommand
{
using CallbackType = std::function<void()>;
public:
	AIExitCommand(CallbackType&& callback)
		: m_Callback(std::move(callback))
	{
	}
	template<typename T, typename Ret, typename... Args>
	AIExitCommand(T* owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		m_Callback = [owner, memFunc, args...]()
		{
			(owner->*memFunc)(args...);
		};
	}
	void Execute() const
	{
		m_Callback();
	}
private:
	CallbackType m_Callback;
};

class CAIController abstract : public CController
{
protected:
	CAIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAIController(const CAIController& rhs);
	virtual ~CAIController() override = default;

public:
	virtual void Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	void SetTarget(class CScarletCharacter* pTarget);
	_float GetTurnRemain() const { return m_fTurnRemain; }

	// duration = 0 �̸� �ѹ��� ������
	// OnStart, OnExit �ʿ��ϸ� �����
	template<typename T, typename Ret, typename... Args>
	void AddCommand(const string& strName, _float fDuration, Ret(T::*memFunc)(Args...), Args... args)
	{
		if (ExistCommand(strName))
			return;
		m_Commands.emplace(strName, AICommand(fDuration, this, memFunc, std::forward<Args>(args)...));
	}

	_bool ExistCommand(const string& strName);
	void DeleteCommand(const string& strName);
	void ClearCommands();

	template<typename T, typename Ret, typename... Args>
	void AddExitCommand(const string& strName, Ret(T::*memFunc)(Args...), Args... args)
	{
		if (ExistExitCommand(strName))
			return;
		m_ExitCommands.emplace(strName, AIExitCommand(this, memFunc, std::forward<Args>(args)...));
	}

	_bool ExistExitCommand(const string& strName);
	void DeleteExitCommand(const string& strName);
	void ClearExitCommands();
	void ClearAllCommands();


	// �⺻ command �Լ�, Ŀ�ǵ� �Լ��� ���ڴ� ���۷��� ��� ����!
	void Move(EMoveAxis eAxis);
	void TurnToTarget(_float fSpeedRatio = 1.f);
	void Input(EHandleInput eInput);

private:
	void ExecuteExitCommand(const string& strName);


protected:
	class CScarletCharacter* m_pTarget = nullptr;
	unordered_map<string, AICommand> m_Commands;
	unordered_map<string, AIExitCommand> m_ExitCommands;
	_float m_fTurnRemain = 0.f;

	// command ������ ����� TimeDelta
	_float m_fTimeDelta = 0.f;

public:
	virtual void Free() override;
};

END
