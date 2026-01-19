#pragma once
#include <standard>
#include <Datatypes/Flags.hpp>
#include <Base/object.hpp>

#include <utils/sequence.hpp>

namespace sre
{
	
}

#define _make_t template <typename... Args>

class Connection;
class ConnectionHandle;

class SignalBase
{
	friend class ConnectionHandle;
	friend class Connection;

protected:
	constexpr SignalBase(void *userdata) : userdata(userdata) {}
	~SignalBase();
	typedef void (*dummy_func_t)(void*, void*, ...);
private:
	static sre_sptr static_invoker(SignalBase* sig, void* func, void* data);
	Connection* m_head = NULL;
	void* m_semaphore = NULL;
protected:
	void base_fire();
	void base_yield();
	Connection* base_connect(void* fun, void* userdata, uintptr_t flags);
	template <typename _Callable> Connection* base_connect(_Callable fun, void* userdata, uintptr_t flags) { return base_connect(reinterpret_cast<void*>(fun), userdata, flags); }

	virtual void invoke_func(void* func, void* data) = 0;
public:
	void* userdata;
};

class Connection
{
	_make_t friend class Signal;
	friend class SignalBase;
	friend class ConnectionHandle;
	typedef SignalBase::dummy_func_t dummy_func_t;

	Connection(SignalBase* signal, dummy_func_t func, void* userdata, uintptr_t flags);
	~Connection();

	SignalBase* m_signal;
	dummy_func_t m_func;
	ConnectionHandle* m_currhandle = NULL;

	Connection* m_next = NULL;
	Connection* m_prev = NULL;

	enum flags
	{
		NONE = 0,
		MULTITHREADED = (1 << 0),
		ONCE = (1 << 1)
	};
public:
	void* userdata;
	sre::flagsptr flags;
};

class ConnectionHandle
{
	friend class Connection;
	Connection* connection;
public:
	constexpr ConnectionHandle(): connection(NULL) {}
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
	template <typename Ret, typename ST, typename CT> using Callable = Ret(*)(ST*, CT*, Args...);
	template <typename Ret> using ShortCallable = Ret(*)(void);
	using DefaultCallable = Callable<void, void, void>;

	std::tuple<Args...> ret_args;

	template <size_t... _indices> void invoke(void* func, void* data, ut::sequence<_indices...>) { reinterpret_cast<DefaultCallable>(func)(this->userdata, data, std::move(std::get<_indices>(ret_args))...); }
	void invoke_func(void* func, void* data) override { invoke(func, data, typename ut::make_sequence<sizeof...(Args)>::type {}); }
	
	#define _T1 template <typename Ret, typename ST, typename CT>
	#define _T2 template <typename Ret>
public:
	constexpr Signal(void* Userdata=NULL): SignalBase(Userdata) { }
	void Fire(Args... args) { ret_args = std::make_tuple( args... ); base_fire(); }


	_T1 Connection* Connect(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::NONE); }
	_T2 Connection* Connect(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::NONE); }

	_T1 Connection* Once(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE); }
	_T2 Connection* Once(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE); }

	_T1 Connection* ConnectParallel(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED); }
	_T2 Connection* ConnectParallel(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED); }

	_T1 Connection* OnceParallel(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE | Connection::MULTITHREADED); }
	_T2 Connection* OnceParallel(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE | Connection::MULTITHREADED); }

	std::tuple<Args...> Wait() { base_yield(); return ret_args; }
};

// Special signal class with no arguments
template <> class Signal<> : public SignalBase
{
	template <typename Ret, typename ST, typename CT> using Callable = Ret(*)(ST*, CT*);
	template <typename Ret> using ShortCallable = Ret(*)(void);
	using DefaultCallable = Callable<void, void, void>;

	void invoke_func(void* func, void* data) override { reinterpret_cast<DefaultCallable>(func)(this->userdata, data); }
public:
	constexpr Signal(void* userdata = NULL): SignalBase(userdata) {}
	void Fire(void) { base_fire(); }
	void Wait(void) { base_yield(); }

	_T1 Connection* Connect(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::NONE); }
	_T2 Connection* Connect(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::NONE); }

	_T1 Connection* Once(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE); }
	_T2 Connection* Once(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::ONCE); }

	_T1 Connection* ConnectParallel(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED); }
	_T2 Connection* ConnectParallel(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED); }

	_T1 Connection* OnceParallel(Callable<Ret, ST, CT> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED | Connection::ONCE); }
	_T2 Connection* OnceParallel(ShortCallable<Ret> function, void* userdata = NULL) { return base_connect(function, userdata, Connection::MULTITHREADED | Connection::ONCE); }
};

#undef _T1
#undef _T2
#undef _make_t