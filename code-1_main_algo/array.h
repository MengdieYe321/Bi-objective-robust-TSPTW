#pragma once

//=====================================================================================================================================================================

template <typename Scalar>
class array1d
{
	bool _allc;

	Scalar* _data;
	size_t _n;

public:

	array1d()
		: _n(0)
		,_allc(false)
		,_data(NULL)
	{
		//empty
	}

	void alloc(const size_t n)
	{
		_n = n;
		_data = new Scalar[_n];
		_allc = true;
	}

	void fill(const Scalar x)
	{
		if (_allc) {
			std::fill(_data, _data + _n, x);
		}
	}

	~array1d()
	{
		if (_allc) {
			delete[] _data;
		}
	}

	inline const Scalar& operator()(const size_t i) const
	{
		return _data[i];
	}

	inline Scalar& operator()(const size_t i)
	{
		return _data[i];
	}
};

//=====================================================================================================================================================================

template <typename Scalar>
class biarray
{
	bool _allc;

	Scalar* _data;
	size_t _n1;
	size_t _n2;
	size_t _nn;

public:

	biarray()
		: _n1(0)
		, _n2(0)
		, _nn(0)
		, _allc(false)
		, _data(NULL)
	{
		//empty
	}

	void alloc(const size_t n1, const size_t n2)
	{
		_n1 = n1;
		_n2 = n2;
		_nn = n1 * n2;
		_data = new Scalar[_nn];
		_allc = true;
	}

	void fill(const Scalar x)
	{
		if (_allc)
		{
			std::fill(_data, _data + _nn, x);
		}
	}

	void fill(const size_t n, const Scalar x)
	{
		if (_allc)
		{
			std::fill(_data, _data + n * _n2, x);
		}
	}

	~biarray()
	{
		if (_allc)
		{
			delete[] _data;
		}
	}

	inline const Scalar& operator()(const size_t i, const size_t j) const
	{
		return _data[i * _n2 + j];
	}

	inline Scalar& operator()(const size_t i, const size_t j)
	{
		return _data[i * _n2 + j];
	}
};

//=====================================================================================================================================================================

template <typename Scalar>
class triarray
{
	bool _allc;

	Scalar* _data;

	size_t _n1;
	size_t _n2;
	size_t _n3;
	size_t _nn;

public:

	triarray()
		: _n1(0)
		, _n2(0)
		, _n3(0)
		, _nn(0)
		, _allc(false)
		, _data(NULL)
	{}

	void alloc(const size_t n1, const size_t n2, const size_t n3)
	{
		_n1 = n1;
		_n2 = n2;
		_n3 = n3;
		_nn = n1 * n2 * n3;
		_data = new Scalar[_nn];
		_allc = true;
	}

	void fill(const Scalar x)
	{
		if (_allc) {
			std::fill(_data, _data + _nn, x);
		}
	}

	~triarray()
	{
		if (_allc) {
			delete[] _data;
		}
	}

	inline const Scalar& operator()(const size_t i, const size_t j, const size_t k) const
	{
		return _data[i * _n2 * _n3 + j * _n3 + k];
	}

	inline Scalar& operator()(const size_t i, const size_t j, const size_t k)
	{
		return _data[i * _n2 * _n3 + j * _n3 + k];
	}

};