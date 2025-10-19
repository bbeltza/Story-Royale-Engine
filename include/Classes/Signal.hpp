#pragma once
#include <standard>

#define _make_t template <typename... _args>

class Connection;

class SignalBase
{
	_make_t friend class Signal;
	friend class Connection;
	typedef void (*dummy_func_t)(void*, void*, ...);

	SignalBase(void*, bool);
	~SignalBase();

	virtual void invoke_func(Connection* connection) = 0;
	static void static_invoker(SignalBase* sig, Connection* connection);

	void base_fire();
	void base_yield();
	Connection* base_connect(dummy_func_t _fun, void* _userdata);

	std::list<Connection*> connections;
public:
	void* Userdata;
private:
	void* semaphore;
	bool multithreaded;
};

class Connection
{
	_make_t friend class Signal;
	friend class SignalBase;
	friend class ConnectionHandle;
	typedef SignalBase::dummy_func_t dummy_func_t;

	Connection(SignalBase* _signal, dummy_func_t _func, void* _userdata);
	~Connection();

	dummy_func_t func;
	SignalBase* const signal;
	ConnectionHandle* current_handle = nullptr;
public:
	void* Userdata;
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

	template <size_t... _indices> void invoke(Connection* connection, std::index_sequence<_indices...>) { (reinterpret_cast<callable_t>(connection->func))(this->Userdata, connection->Userdata, std::get<_indices>(ret_args)...); }
	void invoke_func(Connection* connection) override { invoke(connection, std::make_index_sequence<sizeof...(_args)>{}); }

public:
	Signal(void* Userdata=NULL, bool Multithreaded=true): SignalBase(Userdata, Multithreaded) {}
	void Fire(_args... args) { ret_args = { args... }; base_fire(); }
	template <typename _retype, typename _sigdata, typename _connectdata> Connection* Connect(_retype(*Func)(_sigdata*, _connectdata*, _args...), void* Userdata = NULL) { return base_connect(reinterpret_cast<dummy_func_t>(Func), Userdata); }
	template <typename _retype> Connection* Connect(_retype(*Func)(void), void* Userdata = NULL) { return base_connect(reinterpret_cast<dummy_func_t>(Func), Userdata); }
	std::tuple<_args...> Wait() { base_yield(); return ret_args; }
};

#undef _make_t