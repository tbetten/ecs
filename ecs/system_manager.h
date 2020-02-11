#pragma once
#include "system.h"
#include "messaging.h"
#include "SFML/Graphics.hpp"
#include <SFML/System.hpp>

#include <any>
#include <unordered_map>

struct Shared_context;
class Sender;

namespace ecs
{
	using System_map = std::unordered_map<System_type, S_base::Ptr>;

	class Entity_manager;
	

	class System_manager
	{
	public:
		explicit System_manager(Shared_context* context, messaging::Messenger* m) : m_context{ context }, m_entity_mgr{ nullptr }, m_messenger{ m } {}
		void setup_events ();

		void add_entity_manager (Entity_manager* mgr);
		Entity_manager* get_entity_mgr ();

		void set_context (Shared_context* context);
		Shared_context* get_context ();

		void add_system (System_type id, S_base::Ptr system);

		template<typename T>
		T* get_system (System_type system) const
		{
			return m_systems.count (system) ? dynamic_cast<T*>(m_systems.at(system).get()) : nullptr;
		}

		void update (sf::Int64 dt);

		messaging::Messenger* get_messenger() { return m_messenger; }
//		void register_events (System_type system_id, const std::vector<std::string>& events);
//		System_type find_event (const std::string& event) const;
//		Dispatcher& get_event (System_type system, const std::string& event) const;
//		Sender* get_sender(System_type system) const;

		void entity_modified (std::any payload);

	private:
		System_map m_systems;
		Entity_manager* m_entity_mgr;
		Shared_context* m_context;
		messaging::Messenger* m_messenger;

		std::unordered_map<std::string, System_type> m_events;
	};
}