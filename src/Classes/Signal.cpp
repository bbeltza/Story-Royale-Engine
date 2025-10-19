#include <SDL.h>
#include "Classes/Signal.hpp"
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
	if (multithreaded)
	{
		for (Connection* connection : connections)
		{
			Engine->ThreadPool.CreateImmediateThread(static_invoker, this, connection);
		}
	}
	else
	{
		for (Connection* connection : connections)
		{
			invoke_func(connection);
		}
	}
}

SignalBase::SignalBase(void* _userdata, bool _multithreaded): Userdata(_userdata), semaphore(SDL_CreateSemaphore(0)), multithreaded(_multithreaded) {}

SignalBase::~SignalBase()
{
	if (SDL_WasInit(0)) SDL_DestroySemaphore(get_semaphore);
	while (!connections.empty())
		delete connections.front();
}

Connection* SignalBase::base_connect(dummy_func_t _func, void* _userdata)
{
	Connection* ret = new Connection{ this, _func, _userdata };
	return ret;
}

void SignalBase::base_yield()
{
	SDL_SemWait(get_semaphore);
}
#pragma endregion
#pragma region Connection
Connection::Connection(SignalBase* _signal, dummy_func_t _func, void* _userdata) : signal(_signal), func(_func), Userdata(_userdata)
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
ConnectionHandle::~ConnectionHandle()
{
	if (connection)
		delete connection;
}
ConnectionHandle& ConnectionHandle::operator=(Connection* ptr)
{
	connection = ptr;
	connection->current_handle = this;
	return *this;
}
void ConnectionHandle::Disconnect()
{
	if (connection)
		delete connection;
}
#pragma endregion