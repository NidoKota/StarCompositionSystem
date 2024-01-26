#pragma once

#include <iostream>
#include <vector>
#include <functional>

using namespace std;

template<class TOutput, class TInput>
class IReadOnlyEvent
{
	using FuncType = function<TOutput(TInput)>;

public:
	inline virtual void Subscribe(FuncType& func) = 0;
	inline virtual void Unsubscribe(FuncType& func) = 0;
	inline ~IReadOnlyEvent() {}
};

template<class TOutput, class TInput>
class Event : public IReadOnlyEvent<TOutput, TInput>
{
	using FuncType = function<TOutput(TInput)>;

private:
	vector<reference_wrapper<FuncType>> _funcs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcs.push_back(func);
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcs), end(_funcs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcs.erase(rm, cend(_funcs));
	}

	inline vector<TOutput> operator()(TInput inputs) const
	{
		vector<TOutput> result = vector<TOutput>(_funcs.size());
		for (size_t i = 0; i < _funcs.size(); i++)
		{
			const FuncType& func = _funcs[i].get();
			result[i] = func(inputs);
		}
		return result;
	}
};

template<class TInput>
class Event<void, TInput> : public IReadOnlyEvent<void, TInput>
{
	using FuncType = function<void(TInput)>;

private:
	vector<reference_wrapper<FuncType>> _funcs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcs.push_back(func);
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcs), end(_funcs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcs.erase(rm, cend(_funcs));
	}

	inline void operator()(TInput inputs) const
	{
		for (size_t i = 0; i < _funcs.size(); i++)
		{
			const FuncType& func = _funcs[i].get();
			func(inputs);
		}
	}
};

template<class TOutput>
class Event<TOutput, void> : public IReadOnlyEvent<TOutput, void>
{
	using FuncType = function<TOutput(void)>;

private:
	vector<reference_wrapper<FuncType>> _funcs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcs.push_back(func);
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcs), end(_funcs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcs.erase(rm, cend(_funcs));
	}

	inline vector<TOutput> operator()() const
	{
		vector<TOutput> result = vector<TOutput>(_funcs.size());
		for (size_t i = 0; i < _funcs.size(); i++)
		{
			const FuncType& func = _funcs[i].get();
			result[i] = func();
		}
		return result;
	}
};

template<>
class Event<void, void> : public IReadOnlyEvent<void, void>
{
	using FuncType = function<void(void)>;

private:
	vector<reference_wrapper<FuncType>> _funcs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcs.push_back(func);
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcs), end(_funcs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcs.erase(rm, cend(_funcs));
	}

	inline void operator()() const
	{
		for (size_t i = 0; i < _funcs.size(); i++)
		{
			const FuncType& func = _funcs[i].get();
			func();
		}
	}
};