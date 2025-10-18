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
			Engine->ThreadPool.CreateImmediateThread(static_invoker, this, connection);
	}
	else
	{
		for (Connection* connection : connections)
			invoke_func(connection);
	}
}

SignalBase::SignalBase(void* _userdata, bool _multithreaded): Userdata(_userdata), semaphore(SDL_CreateSemaphore(0)), multithreaded(_multithreaded) {}

SignalBase::~SignalBase()
{
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
	signal->connections.remove(this);
}
#pragma endregion
#pragma region Connection Handle
ConnectionHandle::ConnectionHandle(Connection* _connection): connection(_connection) {}
ConnectionHandle::ConnectionHandle(ConnectionHandle&& moving): connection(moving.connection)
{
	moving.connection = nullptr;
}
ConnectionHandle::~ConnectionHandle()
{
	if (connection)
		delete connection;
}
ConnectionHandle& ConnectionHandle::operator=(ConnectionHandle&& moving)
{
	moving.connection = nullptr;
	return *this;
}
void ConnectionHandle::Disconnect()
{
	if (connection)
		delete connection;
	connection = nullptr;
}
#pragma endregion