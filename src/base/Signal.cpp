#include <SDL.h>
#include "Base/Signal.hpp"
#include "Base/Thread.hpp"
#include "Engine.hpp"

#define get_semaphore static_cast<SDL_sem *>(m_semaphore)

void SignalBase::static_invoker(SignalBase *sig, void *func, void *data) { sig->invoke_func(func, data); }

void SignalBase::base_fire()
{
	if (m_semaphore) SDL_SemPost(get_semaphore);

	for (Connection *connection = m_head; connection; connection = connection->m_next)
	{
		Connection *prev = connection->m_prev;
		if (connection->flags.has(Connection::MULTITHREADED))
			Threads::Create(static_invoker, this, reinterpret_cast<void*>(connection->m_func), connection->userdata);
		else
			invoke_func(reinterpret_cast<void*>(connection->m_func), connection->userdata);

		if (connection->flags.has(Connection::ONCE))
			delete connection;
	}
}

SignalBase::~SignalBase()
{
	if (SDL_WasInit(0))
		SDL_DestroySemaphore(get_semaphore);
	while (m_head)
		delete m_head;
}

Connection *SignalBase::base_connect(void *_func, void *_userdata, uintptr_t _flags)
{
	Connection *ret = new Connection{this, reinterpret_cast<dummy_func_t>(_func), _userdata, _flags};
	return ret;
}

void SignalBase::base_yield()
{
	if (!m_semaphore)
		m_semaphore = SDL_CreateSemaphore(0);

	SDL_SemWait(get_semaphore);
}

Connection::Connection(SignalBase *signal, dummy_func_t func, void *_userdata, uintptr_t _flags) : m_signal(signal), m_func(func), userdata(_userdata), flags(_flags)
{
	m_next = signal->m_head;
	signal->m_head = this;
	if (m_next)
		m_next->m_prev = this;
}

Connection::~Connection()
{
	Connection *prev = m_prev;
	if (m_next)
		m_next->m_prev = prev;
	if (prev)
		prev->m_next = m_next;
	else
		m_signal->m_head = m_next;

	if (m_currhandle)
		m_currhandle->connection = NULL;
}

ConnectionHandle::ConnectionHandle(ConnectionHandle &&moving) noexcept : connection(moving.connection)
{
	moving.connection = nullptr;
	connection->m_currhandle = this;
}
ConnectionHandle::~ConnectionHandle() { Disconnect(); }
ConnectionHandle &ConnectionHandle::operator=(Connection *ptr)
{
	connection = ptr;
	connection->m_currhandle = this;
	return *this;
}
void ConnectionHandle::Disconnect()
{
	if (connection)
		delete connection;
}