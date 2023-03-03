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

private:
	CallbackType m_Callback;
	_float m_fDuration;
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

	// duration = 0 이면 한번만 실행함
	// OnStart, OnExit 필요하면 만들기
	template<typename T, typename Ret, typename... Args>
	void AddCommand(const string& strName, _float fDuration, Ret(T::*memFunc)(Args...), Args... args)
	{
		if (ExistCommand(strName))
			return;
		m_Commands.emplace(strName, AICommand(fDuration, this, memFunc, std::forward<Args>(args)...));
	}

	_bool ExistCommand(const string& strName);
	void PopCommand(const string& strName);
	void ClearCommands();


	// 기본 command 함수, 커맨드 함수의 인자는 레퍼런스 사용 금지!
	void Move(EMoveAxis eAxis);
	void TurnToTarget(_float fSpeedRatio = 1.f);
	void Input(EHandleInput eInput);

protected:
	class CScarletCharacter* m_pTarget = nullptr;
	_float4 m_vLookPos;
	map<string, AICommand> m_Commands;
	_float m_fTurnRemain = 0.f;

	// command 내에서 사용할 TimeDelta
	_float m_fTimeDelta = 0.f;

public:
	virtual void Free() override;
};

END
