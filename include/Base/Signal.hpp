#ifndef SRE_SIGNAL_HPP
#define SRE_SIGNAL_HPP
#include <Base/Signal.h>
#include <Datatypes/Flags.hpp>
#include <utils/sequence.hpp>
#include <standard>

namespace sre
{
	class Connection
	{
		sre_Connection* m_ptr=NULL;
	public:
		using ptr = sre_Connection*;
	public:
		constexpr Connection() = default;
		Connection(sre_Connection* ptr): m_ptr(sre_signalaquire(ptr)) {}
		~Connection() { sre_signalunaquire(m_ptr); }

		void disconnect() { sre_signalunaquire(m_ptr); sre_signaldisconnect(m_ptr); m_ptr = NULL; }
		bool connected() { return m_ptr != NULL; }
	};

	template <typename... Args>
	class Signal
	{
		sre_Signal* m_ptr;
	public:
		using TupleType = std::tuple<Args...>;
	public:
		Signal(void* userdata=NULL): m_ptr(sre_signalcreate(userdata)) {}
		~Signal() { sre_signaldestroy(m_ptr); }

		void fire(Args... args)
		{
			TupleType tuple{args...};
			sre_signalfire(m_ptr, &tuple);
		}
		
		void fire(Args&&... args)
		{
			TupleType tuple{std::forward<Args>(args)...};
			sre_signalfire(m_ptr, &tuple);
		}

		TupleType wait(unsigned timeout=-1)
		{
			auto arg = static_cast<TupleType*>(sre_signalwait(m_ptr, timeout));
			return *arg;
		}

		template <typename Fn, typename S, typename C>
		Connection::ptr connect(Fn fn(S*, C*, Args...), void* userdata)
		{
			void** data;
			sre_Connection* connection = sre_signalconnectEx(
				m_ptr,
				get_invoke<Fn, S, C>(typename ut::make_sequence<sizeof...(Args)>::type {}),
				sizeof(void*) * 2,
				reinterpret_cast<void**>(&data)
			);

			data[0] = reinterpret_cast<void*>(fn);
			data[1] = userdata;

			return connection;
		}

	private:
		template <typename Fn, typename S, typename C, size_t... Indices>
		static void invoke(void* signal_data, void* connection_data, void* fire_data)
		{
			struct ConnectionData
			{
				Fn (*fn)(S*, C*, Args...);
				C* data;
			} *data = static_cast<ConnectionData*>(connection_data);

			auto tuple = static_cast<TupleType*>(fire_data);
			return data->fn(static_cast<S*>(signal_data), data->data, std::get<Indices>(*tuple)...);
		}

		template <typename Fn, typename S, typename C, size_t... Indices>
		sre_signalfunction get_invoke(ut::sequence<Indices...>) { return &invoke<Fn, S, C, Indices...>; }
	};

	template <typename T>
	class Signal<T*>
	{
		sre_Signal* m_ptr;
	public:
		Signal(void* userdata=NULL): m_ptr(sre_signalcreate(userdata)) {}
		~Signal() { sre_signaldestroy(m_ptr); }

		void fire(T* arg=NULL) { sre_signalfire(m_ptr, arg); }
		T* wait(unsigned timeout=-1) { return static_cast<T*>(sre_signalwait(m_ptr, timeout)); }

		template <typename Fn, typename S, typename C>
		Connection::ptr connect(Fn fn(S*, C*, T*), void* userdata) { return sre_signalconnect(m_ptr, userdata, reinterpret_cast<sre_signalfunction>(fn)); }
		template <typename Fn, typename S, typename C>
		Connection::ptr connect(Fn fn(S*, C*), void* userdata) { return connect(reinterpret_cast<Fn(*)(S*, C*, T*)>(fn), userdata); }
		template <typename Fn>
		Connection::ptr connect(Fn fn(), void* userdata) { return connect(reinterpret_cast<Fn(*)(void*, void*, T*)>(fn), userdata); }
	};

	using empty_t = nullptr_t*;
	using EmptySignal = Signal<empty_t>;
}

#endif

/*
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

*/