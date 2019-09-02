#pragma once

#include <any>
#include <functional>
#include <memory>

using Callback = std::function<void (std::any)>;

class Dispatcher
{
public:
	void bind (Callback c)
	{
		m_callbacks.push_back (c);
	}
	void notify (std::any value) const
	{
		for (auto callback : m_callbacks)
		{
			callback (value);
		}
	}
private:
	std::vector<Callback> m_callbacks;
};