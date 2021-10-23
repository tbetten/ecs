#pragma once
#pragma once
#include "ecs_types.h"
#include "messaging.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <variant>

struct Shared_context;

namespace ecs
{
	enum class Component_type : size_t;

	struct Share
	{
		Share (std::string k, Entity_id e, size_t c) : key { std::move (k) }, entity { e }, component_index { c }{}
		std::string key;
		Entity_id entity;
		size_t component_index;
	};

	struct C_base
	{
		using Ptr = std::unique_ptr<C_base>;
		C_base() = default;
		virtual ~C_base () = default;
		virtual void add_row () = 0;
		virtual void reset (size_t index) = 0;
		virtual void load (size_t index, const std::string& key) = 0;
		virtual size_t get_size() = 0;

		std::vector<size_t> m_free_indices;
		std::vector<Share> m_shares;
		Shared_context* m_context;
	};

	template <typename T>
	struct Component : public C_base
	{
		void add_row () override
		{
			//m_data.push_back (T{});
			m_data.emplace_back ();
		}

		void reset (size_t index) override
		{
			m_data[index].reset ();
		}

		void load (size_t index, const std::string& key) override
		{
			m_data [index].load (key, m_context);
		}

		size_t get_size() override
		{
			return m_data.size();
		}

		std::vector<T> m_data;
	};

	class Entity_manager : public messaging::Sender
	{
	public:
		using Component_index = std::optional<size_t>;          // index into m_components
		using Component_indices = std::vector<Component_index>; // all component indices of an entity
		using Entity_index = std::vector<Component_indices>;    // component indices of all entities; entity id is index into this vector

		explicit Entity_manager(messaging::Messenger* messenger);
		void add_component (Component_type component_type, C_base::Ptr component);
		Entity_id add_entity (Bitmask b, std::string key = "");
	//	void update_entity (Entity_id id, Bitmask b);
		bool has_component (Entity_id entity, Component_type component) const;
		bool add_component_to_entity(Entity_id entity, Component_type component, std::string key = "", bool will_notify = true);
		bool remove_component_from_entity(Entity_id entity, Component_type component, bool will_notify = true);
		void remove_entity (Entity_id id);
		messaging::Messenger* get_messenger() { return m_messenger; }
	//	Dispatcher& get_event () { return m_entity_modified; }

		C_base* get_component_by_id (size_t id) const;

		template <typename T>
		T* get_component (Component_type c_id)
		{
			C_base* c = m_components[static_cast<size_t>(c_id)].get ();
			T* res = dynamic_cast<T*>(c);
			return res;
		}

		std::optional<size_t> get_index(Component_type c_id, Entity_id e_id) const;
		std::optional<Entity_id> idex_to_entity(Component_type c_id, std::size_t index) const;

		template <typename T>
		auto get_data (Component_type c_id, Entity_id e_id)
		{
			T* comp = get_component<T> (c_id);
			auto index = get_index(c_id, e_id);
			return index ? &comp->m_data[*index] : nullptr;
		}

	private:
		std::optional<size_t> take_free_index(size_t component_index);
		void do_notify(Entity_id entity) const;

	//	Dispatcher m_entity_modified;
		std::vector<C_base::Ptr> m_components;
		Entity_index m_entity_index;
	};
}