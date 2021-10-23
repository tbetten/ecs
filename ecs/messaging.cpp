#include "messaging.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace messaging
{
	void Sender::add_message(std::string message) noexcept
	{
		if (std::find(std::cbegin(m_messages), std::cend(m_messages), message) != std::cend(m_messages)) return;
		m_messages.push_back(std::move(message));
	}

	void Sender::remove_message(std::string_view message) noexcept
	{
		auto itr = std::find(std::begin(m_messages), std::end(m_messages), message);
		if (itr != std::end(m_messages)) m_messages.erase(itr);
	}

	bool Sender::notify(std::string_view message, std::any payload) const noexcept
	{
		if (std::find(std::cbegin(m_messages), std::cend(m_messages), message) == std::cend(m_messages)) return false;
		m_messenger->notify(message, std::move(payload));
		return true;
	}

	Binding_id Messenger::bind(std::string message, Callback c) noexcept
	{
/*		if (message == "entity_modified")
		{
			std::cout << "bound entity modified\n";
		}*/
		auto itr = m_bindings.find(message);
		if (itr == std::cend(m_bindings))
		{
			m_bindings[message] = Callbacks{ c };
			return 0;
		}
		auto& [mess, callbacks] = *itr;
		auto c_itr = std::find(std::begin(callbacks), std::end(callbacks), std::nullopt);
		if (c_itr == std::end(callbacks))
		{
			callbacks.push_back(std::move(c));
			return callbacks.size() - 1;
		}
		*c_itr = c;
		return std::distance(std::begin(callbacks), c_itr);
	}

	void Messenger::unbind(const std::string& message, Binding_id id) noexcept
	{
		auto itr = m_bindings.find(message);
		if (itr == std::cend(m_bindings)) return;
		auto& c = m_bindings[message];
		if (c.size() <= id) return;
		c[id] = std::nullopt;
	}

	void Messenger::notify(std::string_view message, const std::any& payload) const noexcept
	{
		auto itr = m_bindings.find(std::string{ message });
		if (itr == std::cend(m_bindings)) return;
		auto& [mess, callbacks] = *itr;
		std::for_each(std::cbegin(callbacks), std::cend(callbacks), [&payload](const std::optional<Callback>& c) {if (c) c.value()(payload); });
	}
}