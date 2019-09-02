#pragma once
#include "ecs_types.h"
#include "messaging.h"

#include <vector>
#include <SFML/System.hpp>

namespace ecs
{
	using Entity_list = std::vector<Entity_id>;
	using Requirements = std::vector<Bitmask>;
	enum class System_type : size_t;

	class System_manager;
	class S_base
	{
	public:
		using Ptr = std::unique_ptr<S_base>;
		S_base (System_type id, System_manager* manager) : m_id{ id }, m_system_manager{ manager }{}
		virtual ~S_base () = default;

		bool add_entity (Entity_id entity);
		bool has_entity (Entity_id entity) const;
		bool remove_entity (Entity_id entity);

		System_type get_id () { return m_id; }
		bool fits_requirements (Bitmask bits) const;

		virtual void update (sf::Int64 dt) = 0;
		virtual void setup_events () = 0;
		virtual Dispatcher& get_event (const std::string& event) = 0;

	protected:
		System_type m_id;
		Requirements m_requirements;
		Entity_list m_entities;
		System_manager* m_system_manager;
	};
}