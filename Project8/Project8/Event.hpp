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
	inline virtual FuncType& Subscribe(FuncType&& func) = 0;
	inline virtual void Unsubscribe(FuncType& func) = 0;
	inline ~IReadOnlyEvent() {}
};

template<class TOutput, class TInput>
class Event : public IReadOnlyEvent<TOutput, TInput>
{
	using FuncType = function<TOutput(TInput)>;

private:
	vector<FuncType> _funcs = vector<FuncType>();
	vector<reference_wrapper<FuncType>> _funcRefs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcRefs.push_back(func);
	}

	inline FuncType& Subscribe(FuncType&& func) override
	{
		_funcs.push_back(func);
		_funcRefs.push_back(_funcs.back());
		return _funcs.back();
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcRefs), end(_funcRefs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcRefs.erase(rm, cend(_funcRefs));
	}

	inline vector<TOutput> operator()(TInput inputs) const
	{
		vector<TOutput> result = vector<TOutput>(_funcRefs.size());
		for (size_t i = 0; i < _funcRefs.size(); i++)
		{
			const FuncType& func = _funcRefs[i].get();
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
	vector<FuncType> _funcs = vector<FuncType>();
	vector<reference_wrapper<FuncType>> _funcRefs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcRefs.push_back(func);
	}

	inline FuncType& Subscribe(FuncType&& func) override
	{
		_funcs.push_back(func);
		_funcRefs.push_back(_funcs.back());
		return _funcs.back();
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcRefs), end(_funcRefs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcRefs.erase(rm, cend(_funcRefs));
	}

	inline void operator()(TInput inputs) const
	{
		for (size_t i = 0; i < _funcRefs.size(); i++)
		{
			const FuncType& func = _funcRefs[i].get();
			func(inputs);
		}
	}
};

template<class TOutput>
class Event<TOutput, void> : public IReadOnlyEvent<TOutput, void>
{
	using FuncType = function<TOutput(void)>;

private:
	vector<FuncType> _funcs = vector<FuncType>();
	vector<reference_wrapper<FuncType>> _funcRefs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcRefs.push_back(func);
	}

	inline FuncType& Subscribe(FuncType&& func) override
	{
		_funcs.push_back(func);
		_funcRefs.push_back(_funcs.back());
		return _funcs.back();
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcRefs), end(_funcRefs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcRefs.erase(rm, cend(_funcRefs));
	}

	inline vector<TOutput> operator()() const
	{
		vector<TOutput> result = vector<TOutput>(_funcRefs.size());
		for (size_t i = 0; i < _funcRefs.size(); i++)
		{
			const FuncType& func = _funcRefs[i].get();
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
	vector<FuncType> _funcs = vector<FuncType>();
	vector<reference_wrapper<FuncType>> _funcRefs = vector<reference_wrapper<FuncType>>();

public:
	inline void Subscribe(FuncType& func) override
	{
		_funcRefs.push_back(func);
	}

	inline FuncType& Subscribe(FuncType&& func) override
	{
		_funcs.push_back(func);
		_funcRefs.push_back(_funcs.back());
		return _funcs.back();
	}

	inline void Unsubscribe(FuncType& func) override
	{
		auto rm = remove_if(begin(_funcRefs), end(_funcRefs), [&func](reference_wrapper<FuncType> f) { return &func == &f.get(); });
		_funcRefs.erase(rm, cend(_funcRefs));
	}

	inline void operator()() const
	{
		for (size_t i = 0; i < _funcRefs.size(); i++)
		{
			const FuncType& func = _funcRefs[i].get();
			func();
		}
	}
};