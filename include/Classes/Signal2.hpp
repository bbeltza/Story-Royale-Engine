#pragma once
#include <standard>

#define _make_t template <typename... _args>

class SignalBase
{
	_make_t friend class _Signal;
	friend class _Connection;

	virtual void invoke_func(_Connection* connection) = 0;

	void base_fire();

	std::list<_Connection*> connections;
public:
	void* Userdata;
};

class _Connection
{
	_make_t friend class _Signal;
	typedef void (*dummy_func_t)(void*, void*, ...);

	_Connection();
	~_Connection();

	dummy_func_t func;
public:
	void* Userdata;
};

_make_t
class _Signal: public SignalBase
{
	std::tuple<_args...> ret_args;
	void invoke_func(_Connection* connection) override { connection->func(this->Userdata, connection->Userdata); }

public:
	void Fire(_args... args)
	{
		ret_args = { args... };
		base_fire();
	}
};

#undef _make_t