#include <SDL.h>
#include "Classes/Signal.hpp"
#include "Classes/Thread.hpp"
#include "Engine.hpp"

#pragma region SignalBase
#define get_semaphore reinterpret_cast<SDL_sem*>(semaphore)
void SignalBase::static_invoker(SignalBase* sig, dummy_func_t func, void* data)
{
	sig->invoke_func(func, data);
}

void SignalBase::base_fire()
{
	SDL_SemPost(get_semaphore);
	critical = true;
	if (multithreaded)
	{
		for (Connection* connection : connections)
		{
			if (connection->flags & Connection::multithreaded)
				Threads::Create(static_invoker, this, connection->func, connection->Userdata);
			else
				invoke_func(connection->func, connection->Userdata);
			if (connection->flags & Connection::once) delete_queue.push(connection);
		}
	}
	else
	{
		for (Connection* connection : connections)
		{
			invoke_func(connection->func, connection->Userdata);
			if (connection->flags & Connection::once) delete_queue.push(connection);
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

Connection* SignalBase::base_connect(dummy_func_t _func, void* _userdata, intptr_t _flags)
{
	Connection* ret = new Connection{ this, _func, _userdata, _flags };
	return ret;
}

void SignalBase::base_yield()
{
	SDL_SemWait(get_semaphore);
}
#pragma endregion
#pragma region Connection
Connection::Connection(SignalBase* _signal, dummy_func_t _func, void* _userdata, intptr_t _flags) : signal(_signal), func(_func), Userdata(_userdata), flags(_flags)
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