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
		if (!m_bDo) {
			func();
			m_bDo = true;
		}
	}

	void Reset() { m_bDo = false; }

private:
	_bool m_bDo = false;

};
END