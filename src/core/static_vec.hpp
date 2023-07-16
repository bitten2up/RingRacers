#ifndef __SRB2_CORE_STATIC_VEC_HPP__
#define __SRB2_CORE_STATIC_VEC_HPP__

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>

#include "../cxxutil.hpp"

namespace srb2
{

template <typename T, size_t Limit>
class StaticVec;

// Silly hack to avoid GCC standard library algorithms bogus compile warnings
template <typename T, size_t Limit>
class StaticVecIter
{
	T* p_;

	StaticVecIter(T* p) noexcept : p_(p) {}

	friend class StaticVec<T, Limit>;
	friend class StaticVec<typename std::remove_const<T>::type, Limit>;

public:
	using difference_type = ptrdiff_t;
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	using iterator_category = std::random_access_iterator_tag;

	T& operator*() const noexcept { return *p_; }
	T* operator->() const noexcept { return p_; }
	StaticVecIter& operator++() noexcept { p_++; return *this; }
	StaticVecIter operator++(int) noexcept { StaticVecIter copy = *this; ++(*this); return copy; }
	StaticVecIter& operator--() noexcept { p_--; return *this; }
	StaticVecIter operator--(int) noexcept { StaticVecIter copy = *this; --(*this); return copy; }
	StaticVecIter& operator+=(ptrdiff_t ofs) noexcept { p_ += ofs; return *this; }
	StaticVecIter& operator-=(ptrdiff_t ofs) noexcept { p_ -= ofs; return *this; }
	T& operator[](ptrdiff_t ofs) noexcept { return *(p_ + ofs); }

	friend ptrdiff_t operator+(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return lhs.p_ + rhs.p_; }
	friend ptrdiff_t operator-(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return lhs.p_ - rhs.p_; }
	friend StaticVecIter operator+(const StaticVecIter& lhs, ptrdiff_t rhs) noexcept { return lhs.p_ + rhs; }
	friend StaticVecIter operator-(const StaticVecIter& lhs, ptrdiff_t rhs) noexcept { return lhs.p_ - rhs; }

	friend bool operator==(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return lhs.p_ == rhs.p_; }
	friend bool operator!=(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return !(lhs.p_ == rhs.p_); }
	friend bool operator>(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return lhs.p_ > rhs.p_; }
	friend bool operator<=(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return !(lhs.p_ > rhs.p_); }
	friend bool operator<(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return lhs.p_ < rhs.p_; }
	friend bool operator>=(const StaticVecIter& lhs, const StaticVecIter& rhs) noexcept { return !(lhs.p_ < rhs.p_); }
};

template <typename T, size_t Limit>
class StaticVec
{
	std::array<T, Limit> arr_ {{}};
	size_t size_ = 0;

public:
	using value_type = T;

	constexpr StaticVec() {}

	StaticVec(const StaticVec& rhs)
	{
		for (size_t i = size_; i > 0; i--)
		{
			arr_[i] = T();
		}
		size_ = rhs.size();
		for (size_t i = 0; i < size_; i++)
		{
			arr_[i] = rhs.arr_[i];
		}
	}

	StaticVec(StaticVec&& rhs) noexcept(std::is_nothrow_move_assignable_v<T>)
	{
		for (size_t i = size_; i > 0; i--)
		{
			arr_[i] = T();
		}
		size_ = rhs.size();
		for (size_t i = 0; i < size_; i++)
		{
			arr_[i] = std::move(rhs.arr_[i]);
		}
		while (rhs.size() > 0)
		{
			rhs.pop_back();
		}
	}

	constexpr StaticVec(std::initializer_list<T> list) noexcept(std::is_nothrow_move_assignable_v<T>)
	{
		size_ = list.size();
		size_t i = 0;
		for (auto itr = list.begin(); itr != list.end(); itr++)
		{
			arr_[i] = *itr;
			i++;
		}
	}

	~StaticVec() = default;

	StaticVec& operator=(const StaticVec& rhs)
	{
		for (size_t i = size_; i > 0; i--)
		{
			arr_[i] = T();
		}
		size_ = rhs.size();
		for (size_t i = 0; i < size_; i++)
		{
			arr_[i] = rhs.arr_[i];
		}
		return *this;
	}

	StaticVec& operator=(StaticVec&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		for (size_t i = size_; i > 0; i--)
		{
			arr_[i] = T();
		}
		size_ = rhs.size();
		for (size_t i = 0; i < size_; i++)
		{
			arr_[i] = std::move(rhs.arr_[i]);
		}
		while (rhs.size() > 0)
		{
			rhs.pop_back();
		}
		return *this;
	}

	void push_back(const T& value) { arr_[size_++] = value; }

	void pop_back() { arr_[--size_] = T(); }

	void resize(size_t size, T value = T())
	{
		if (size >= Limit)
		{
			throw std::length_error("new size >= Capacity");
		}

		if (size == size_)
		{
			return;
		}
		else if (size < size_)
		{
			while (size_ > size)
			{
				pop_back();
			}
		}
		else
		{
			while (size_ < size)
			{
				push_back(value);
			}
		}
	}

	void clear()
	{
		arr_ = {{}};
		size_ = 0;
	}

	constexpr StaticVecIter<T, Limit> begin() noexcept { return &arr_[0]; }

	constexpr StaticVecIter<const T, Limit> begin() const noexcept { return cbegin(); }

	constexpr StaticVecIter<const T, Limit> cbegin() const noexcept { return &arr_[0]; }

	constexpr StaticVecIter<T, Limit> end() noexcept { return &arr_[size_]; }

	constexpr StaticVecIter<const T, Limit> end() const noexcept { return cend(); }

	constexpr StaticVecIter<const T, Limit> cend() const noexcept { return &arr_[size_]; }

	constexpr std::reverse_iterator<StaticVecIter<T, Limit>> rbegin() noexcept { return &arr_[size_]; }

	constexpr std::reverse_iterator<StaticVecIter<const T, Limit>> crbegin() const noexcept { return &arr_[size_]; }

	constexpr std::reverse_iterator<StaticVecIter<T, Limit>> rend() noexcept { return &arr_[0]; }

	constexpr std::reverse_iterator<StaticVecIter<const T, Limit>> crend() const noexcept { return &arr_[0]; }

	constexpr bool empty() const noexcept { return size_ == 0; }

	constexpr size_t size() const noexcept { return size_; }

	constexpr size_t capacity() const noexcept { return Limit; }

	constexpr size_t max_size() const noexcept { return Limit; }

	constexpr T& operator[](size_t index) noexcept { return arr_[index]; }

	T& at(size_t index)
	{
		if (index >= size_)
		{
			throw std::out_of_range("index >= size");
		}
		return this[index];
	}

	constexpr const T& operator[](size_t index) const noexcept { return arr_[index]; }

	const T& at(size_t index) const
	{
		if (index >= size_)
		{
			throw std::out_of_range("index >= size");
		}
		return this[index];
	}

	T& front() { return *arr_[0]; }

	T& back() { return *arr_[size_ - 1]; }
};

} // namespace srb2

template <typename T, size_t L1, size_t L2>
bool operator==(const srb2::StaticVec<T, L1>& lhs, const srb2::StaticVec<T, L2>& rhs)
{
	const size_t size = lhs.size();
	if (size != rhs.size())
	{
		return false;
	}
	for (size_t i = 0; i < lhs; i++)
	{
		if (rhs[i] != lhs[i])
		{
			return false;
		}
	}
	return true;
}

template <typename T, size_t L1, size_t L2>
bool operator!=(const srb2::StaticVec<T, L1>& lhs, const srb2::StaticVec<T, L2>& rhs)
{
	return !(lhs == rhs);
}

template <typename T, size_t Limit>
struct std::hash<srb2::StaticVec<T, Limit>>
{
	uint64_t operator()(const srb2::StaticVec<T, Limit>& input) const
	{
		constexpr const uint64_t prime {0x00000100000001B3};
		std::size_t ret {0xcbf29ce484222325};

		for (auto itr = input.begin(); itr != input.end(); itr++)
		{
			ret = (ret * prime) ^ std::hash<T>(*itr);
		}
		return ret;
	}
};

#endif // __SRB2_CORE_STATIC_VEC_HPP__
