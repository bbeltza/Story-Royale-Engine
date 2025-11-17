#pragma once
#include <standard>
#include "utils/sequence.hpp"

#define _make_t template <typename... _args>

class Connection;
class ConnectionHandle;

class SignalBase
{
	_make_t friend class Signal;
	friend class ConnectionHandle;
	friend class Connection;
	typedef void (*dummy_func_t)(void*, void*, ...);

	SignalBase(void*, bool);
	~SignalBase();

	virtual void invoke_func(dummy_func_t func, void* data) = 0;
	static void static_invoker(SignalBase* sig, dummy_func_t func, void* data);

	void base_fire();
	void base_yield();
	Connection* base_connect(dummy_func_t _fun, void* _userdata, intptr_t _flags);

	std::list<Connection*> connections;
	std::queue<Connection*> delete_queue;
public:
	void* Userdata;
private:
	void* semaphore;
	bool multithreaded;
	bool critical;
};

class Connection
{
	_make_t friend class Signal;
	friend class SignalBase;
	friend class ConnectionHandle;
	typedef SignalBase::dummy_func_t dummy_func_t;

	Connection(SignalBase* _signal, dummy_func_t _func, void* _userdata, intptr_t _flags);
	~Connection();

	SignalBase* const signal;
	dummy_func_t func;
	ConnectionHandle* current_handle = nullptr;

	enum flags
	{
		multithreaded = (1 << 0),
		once = (1 << 1)
	};
public:
	void* Userdata;
	intptr_t flags;
};

class ConnectionHandle
{
	_make_t friend class Signal;
	friend class Connection;
	Connection* connection;
public:
	ConnectionHandle(): connection(NULL) {}
	ConnectionHandle(Connection* connection) { *this = connection; }
	ConnectionHandle(const ConnectionHandle& other) = delete;
	ConnectionHandle(ConnectionHandle&& moving) noexcept;
	~ConnectionHandle();

	ConnectionHandle& operator=(Connection* ptr);
	inline Connection* operator->() { return connection; }

	inline bool Connected() { return connection != nullptr; };
	void Disconnect();
};

_make_t
class Signal: public SignalBase
{
	typedef void(*callable_t)(void*, void*, _args...);
	std::tuple<_args...> ret_args;

	template <size_t... _indices> void invoke(Connection::dummy_func_t func, void* data, ut::sequence<_indices...>) { reinterpret_cast<callable_t>(func)(this->Userdata, data, std::move(std::get<_indices>(ret_args))...); }
	void invoke_func(dummy_func_t func, void* data) override { invoke(func, data, typename ut::make_sequence<sizeof...(_args)>::type {}); }

public:
	Signal(void* Userdata=NULL, bool Multithreaded=true): SignalBase(Userdata, Multithreaded) {}
	void Fire(_args... args) { ret_args = { args... }; base_fire(); }

	#define _csyntax1(n)  template <typename _retype, typename _sigdata, typename _connectdata> Connection* n(_retype(*Func)(_sigdata*, _connectdata*, _args...), void* Userdata = NULL, bool multithreaded=true)
	#define _csyntax2(n)  template <typename _retype> Connection* n(_retype(*Func)(void), void* Userdata = NULL, bool multithreaded=true)

	#define _basecreateconnect(flags) base_connect(reinterpret_cast<dummy_func_t>(Func), Userdata, flags)
	#define _baseconnect _basecreateconnect(multithreaded ? Connection::multithreaded : 0)
	#define _baseonce _basecreateconnect((multithreaded ? Connection::multithreaded : 0) | Connection::once)


	_csyntax1(Connect) { return _baseconnect; }
	_csyntax2(Connect) { return _baseconnect; }

	_csyntax1(Once) { return _baseonce; }
	_csyntax2(Once) { return _baseonce; }

	std::tuple<_args...> Wait() { base_yield(); return ret_args; }
};

#undef _csyntax1
#undef _csyntax2
#undef _basecreateconnect
#undef _baseconnect
#undef _baseonce
#undef _make_t