#pragma once
#include <cstdint>
#include <stdexcept>
#include <initializer_list>
#include <string>
#include <sstream>
#include <ostream>

//#pragma warning(disable:4244) 

/*
Author: Timothy Williams
Created On       : 10-14-2018
Last Modified On : 05-24-2019
*/

template<class T>
class Array {

	T* arr;
	uintmax_t m_length;

	void alloc();
	void dealloc();

	void sort(uintmax_t start, uintmax_t end);
	void swap(T& a, T& b);

public:

	Array();
	Array(uintmax_t size);
	Array(std::initializer_list<T> il);
	Array(const Array<T>& other);
	Array(Array<T>&& other);
	~Array();

	Array<T>& operator=(const Array<T>& rhs);
	Array<T>& operator=(Array<T>&& rhs);

	T& operator[](uintmax_t i);
	const T& operator[](uintmax_t i) const;

	T* begin();
	T* end();

	uintmax_t length() const;
	uintmax_t size() const;
	bool empty() const;

	bool in(const T& val) const;

	Array<T> clone() const;
	void swap(Array<T>& other);

	Array<T> splice(uintmax_t start, uintmax_t end) const;

	void sort();

	std::string toString() const;

};


template<class T>
void Array<T>::alloc() {
	if (m_length > 0)
		arr = new T[m_length];
	else
		arr = NULL;
}
template<class T>
void Array<T>::dealloc() {
	if (arr != NULL)
		delete[] arr;
}

template<class T>
Array<T>::Array() {
	m_length = 0;
	arr = NULL;
}

template<class T>
Array<T>::Array(uintmax_t size) {
	m_length = size;
	alloc();
}

template<class T>
Array<T>::Array(std::initializer_list<T> il) {
	this->m_length = il.size();
	alloc();
	for (int i = 0; i < this->m_length; i++)
		arr[i] = il.begin()[i];
}

template<class T>
Array<T>::Array(const Array<T>& other) {
	m_length = other.m_length;
	alloc();
	for (int i = 0; i < m_length; i++)
		arr[i] = other.arr[i];
}

template<class T>
Array<T>::Array(Array<T>&& other) {
	m_length = other.m_length;
	this->arr = other.arr;
	other.arr = NULL;
}

template<class T>
Array<T>::~Array() {
	dealloc();
}

template<class T>
Array<T>& Array<T>::operator=(const Array<T>& rhs) {
	m_length = rhs.m_length;
	dealloc();
	alloc();
	for (int i = 0; i < m_length; i++) {
		arr[i] = rhs.arr[i];
	}
	return *this;
}

template<class T>
Array<T>& Array<T>::operator=(Array<T>&& rhs) {
	m_length = rhs.m_length;
	arr = rhs.arr;
	rhs.arr = NULL;
	return *this;
}

template<class T>
T& Array<T>::operator[](uintmax_t i) {
	if (i >= m_length)
		throw std::out_of_range("index out of range");
	return arr[i];
}

template<class T>
const T& Array<T>::operator[](uintmax_t i) const {
	if (i >= m_length)
		throw std::out_of_range("index out of range");
	return arr[i];
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Array<T>& a) {
	os << "[";
	for (uintmax_t i = 0; i < a.size(); i++) {
		if (i == 0)
			os << " " << a[i];
		else
			os << ", " << a[i];

		if (i == a.size() - 1)
			os << " ";
	}
	os << "]";
	return os;
}

template<class T>
T* Array<T>::begin() {
	return arr;
}

template<class T>
T* Array<T>::end() {
	return arr + m_length;
}

template<class T>
uintmax_t Array<T>::length() const {
	return m_length;
}
template<class T>
uintmax_t Array<T>::size() const {
	return m_length;
}

template<class T>
bool Array<T>::empty() const {
	return m_length == 0;
}

template<class T>
bool Array<T>::in(const T& val) const {
	for (uintmax_t i = 0; i < m_length; i++)
		if (arr[i] == val)
			return 1;
	return 0;
}

template<class T>
Array<T> Array<T>::clone() const {
	return *this;
}

template<class T>
void Array<T>::swap(Array<T>& other) {
	int tmpLength = m_length;
	m_length = other.m_length;
	other.m_length = tmpLength;

	T* tmpArr = arr;
	arr = other.arr;
	other.arr = tmpArr;
}

template<class T>
Array<T> Array<T>::splice(uintmax_t start, uintmax_t end) const {
	if (start >= m_length)
		throw std::out_of_range("start index out of range");
	if (end > m_length)
		throw std::out_of_range("end index out of range");
	if (start > end)
		return Array<T>();
	Array<T> sub(end - start);
	for (uintmax_t i = start; i < end; i++)
		sub[i - start] = arr[i];
	return sub;
}

template <class T>
std::string Array<T>::toString() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

template <class T>
void Array<T>::swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}

template <class T>
void Array<T>::sort() {
	sort(0, m_length - 1);
}

template <class T>
void Array<T>::sort(uintmax_t start, uintmax_t end) {
	//end is inclusive
	if (m_length == 0)
		return;
	if (start == end) //size == 1
		return;
	if (start == end - 1) { //size == 2
		if (arr[start] <= arr[end])
			return;
		else {
			swap(arr[start], arr[end]);
			return;
		}
	}
	//pivot
	uintmax_t lastSwap = (end + start) / 2;
	if (arr[start] > arr[lastSwap])
		swap(arr[start], arr[lastSwap]);
	if (arr[lastSwap] > arr[end])
		swap(arr[lastSwap], arr[end]);
	if (arr[start] > arr[lastSwap])
		swap(arr[start], arr[lastSwap]);
	T comp = arr[lastSwap];
	lastSwap--;
	uintmax_t j;
	bool j_init = 0;
	//sort
	for (uintmax_t i = start; i <= end; i++) {
		if (arr[i] >= comp) {
			bool swapped = false;
			if (!j_init) {
				j = i + 1;
				j_init = 1;
			}
			for (; j <= end; j++) {
				if (arr[j] <= comp) {
					swap(arr[i], arr[j++]);
					lastSwap = i;
					swapped = true;
					break;
				}
			}
			if (!swapped)
				break;
		}
	}
	sort(start, lastSwap);
	sort(lastSwap + 1, end);
}
