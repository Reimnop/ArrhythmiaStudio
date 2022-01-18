#pragma once

#include <functional>
#include <unordered_set>

template <typename ...Args>
class Event;

template <typename ...Args>
class EventHandler
{
	typedef std::function<void(Args...)> HandlerFunc;
public:
	friend Event<Args...>;

	EventHandler(const HandlerFunc& handlerFunc) : handlerFunc(handlerFunc)
	{
		handlerId = ++handlerIdCounter;
	}

	void invoke(Args... args) const
	{
		handlerFunc(args...);
	}
private:
	static inline int handlerIdCounter = 0;

	HandlerFunc handlerFunc;
	int handlerId;
};

template <typename ...Args>
class Event
{
public:
	void operator+=(EventHandler<Args...> handler)
	{
		eventHandlers.emplace(handler);
	}

	void operator-=(EventHandler<Args...> handler)
	{
		eventHandlers.erase(handler);
	}

	void invoke(Args... args)
	{
		for (const EventHandler<Args...>& handler : eventHandlers)
		{
			handler.invoke(args...);
		}
	}
private:
	static inline auto hash = [](const EventHandler<Args...>& v)
	{
		return std::hash<int>()(v.handlerId);
	};

	static inline auto equal = [](const EventHandler<Args...>& lhs, const EventHandler<Args...>& rhs)
	{
		return lhs.handlerId == rhs.handlerId;
	};

	std::unordered_set<EventHandler<Args...>, decltype(hash), decltype(equal)> eventHandlers;
};
