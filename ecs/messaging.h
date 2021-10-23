#pragma once

#include <functional>
#include <any>
#include <optional>
#include <unordered_map>
#include <string_view>

namespace messaging
{
	using Callback = std::function<void(std::any)>;
	using Binding_id = size_t;
	class Messenger;

	class Sender
	{
	public:
		explicit Sender(Messenger* messenger) : m_messenger{ messenger } {}
		Sender () : m_messenger { nullptr } {}
		inline void set_messenger (Messenger* m) { m_messenger = m; }
		void add_message(std::string message) noexcept;
		void remove_message(std::string_view message) noexcept;
		bool notify(std::string_view message, std::any payload) const noexcept;
	protected:
		Messenger* m_messenger;
	private:
		std::vector<std::string> m_messages;
	};

	using Callbacks = std::vector<std::optional<Callback>>;

	class Messenger
	{
	public:
		void notify(std::string_view message, const std::any& payload) const noexcept;
		Binding_id bind(std::string message, Callback c) noexcept;
		void unbind(const std::string& message, Binding_id id) noexcept;
	private:
		std::unordered_map<std::string, Callbacks> m_bindings;
	};
}