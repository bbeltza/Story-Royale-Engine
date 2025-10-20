#include <SDL.h>
#include "Classes/Signal.hpp"
#include "Classes/Thread.hpp"
#include "Engine.hpp"

#pragma region SignalBase
#define get_semaphore reinterpret_cast<SDL_sem*>(semaphore)
void SignalBase::static_invoker(SignalBase* sig, Connection* connection)
{
	sig->invoke_func(connection);
}

void SignalBase::base_fire()
{
	SDL_SemPost(get_semaphore);
	critical = true;
	if (multithreaded)
	{
		for (Connection* connection : connections)
		{
			if (connection->multithreaded)
				Threads::Create(static_invoker, this, connection);
			else
				invoke_func(connection);
		}
	}
	else
	{
		for (Connection* connection : connections)
		{
			invoke_func(connection);
		}
	}
	critical = false;

	while (!delete_queue.empty())
	{
		delete delete_queue.front();
		delete_queue.pop();
	}
}

SignalBase::SignalBase(void* _userdata, bool _multithreaded): Userdata(_userdata), semaphore(SDL_CreateSemaphore(0)), multithreaded(_multithreaded) {}

SignalBase::~SignalBase()
{
	if (SDL_WasInit(0)) SDL_DestroySemaphore(get_semaphore);
	while (!connections.empty())
		delete connections.front();
}

Connection* SignalBase::base_connect(dummy_func_t _func, void* _userdata, bool _multithreaded)
{
	Connection* ret = new Connection{ this, _func, _userdata, _multithreaded };
	return ret;
}

void SignalBase::base_yield()
{
	SDL_SemWait(get_semaphore);
}
#pragma endregion
#pragma region Connection
Connection::Connection(SignalBase* _signal, dummy_func_t _func, void* _userdata, bool _multithreaded) : signal(_signal), func(_func), Userdata(_userdata), multithreaded(_multithreaded)
{
	signal->connections.push_back(this);
}

Connection::~Connection()
{
	if (current_handle) current_handle->connection = nullptr;
	signal->connections.remove(this);
}
#pragma endregion
#pragma region Connection Handle
ConnectionHandle::ConnectionHandle(ConnectionHandle&& moving) noexcept: connection(moving.connection)
{
	moving.connection = nullptr;
	connection->current_handle = this;
}
ConnectionHandle::~ConnectionHandle() { Disconnect(); }
ConnectionHandle& ConnectionHandle::operator=(Connection* ptr)
{
	connection = ptr;
	connection->current_handle = this;
	return *this;
}
void ConnectionHandle::Disconnect()
{
	if (connection)
	{
		if (connection->signal->critical)
			connection->signal->delete_queue.push(connection);
		else
			delete connection;
	}
}
#pragma endregion