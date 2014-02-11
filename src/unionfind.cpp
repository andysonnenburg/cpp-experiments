#include "unionfind.hpp"

using namespace std;

template <typename T>
inline
unionfind::ptr<T>::ptr(T const& value): link_(make_shared(value)) {}

template <typename T>
inline
unionfind::ptr<T>::~ptr() {}
