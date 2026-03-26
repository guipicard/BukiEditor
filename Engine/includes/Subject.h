#pragma once
#include "Observer.h"
#include <vector>

namespace buki
{
	template<typename T1>
	struct Subject
	{
		void AddListener(Observer<T1>* o)
		{
			m_Observers.push_back(o);
		}
		void RemoveListener(Observer<T1>* o)
		{
			m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), o), m_Observers.end());
		}
		void Invoke(const T1 value)
		{
			for (Observer<T1>* o : m_Observers)
			{
				o->OnNotify(value);
			}
		}
	private:
		std::vector<Observer<T1>*> m_Observers;
	};
}
