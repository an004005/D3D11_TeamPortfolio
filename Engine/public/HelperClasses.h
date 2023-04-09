#pragma once
#include <functional>

BEGIN(Engine)
class CDoOnce
{
public:
	_bool IsNotDo()
	{
		if (m_bDo) 
			return false;
		else
		{
			m_bDo = true;
			return true;
		}
	}

	void IsNotDo(std::function<void()> func)
	{
		if (!m_bDoFunc) {
			func();
			m_bDoFunc = true;
		}
	}

	void Reset() { m_bDo = false; }
	void ResetFunc() { m_bDoFunc = false; }

private:
	_bool m_bDo = false;
	_bool m_bDoFunc = false;
};
END