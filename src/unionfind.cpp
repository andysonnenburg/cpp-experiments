#include "unionfind.cpp"

template <typename T>
inline
unionfind::ptr::ptr(T const& value): link_(value) {}
