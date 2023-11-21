#ifndef DATA_HOLDER_EXP_H
#define DATA_HOLDER_EXP_H

#include <functional>

template <typename T>
class DataHolder {
  T data_;

 public:
  DataHolder(T data) : data_(data) {}
  const T& getData() const { return data_; }
};

/*
overloading `>>` for DH
      f
DH<U> -> DH<T>
*/

// template <typename U, typename T, typename Func>
// DataHolder<T> operator>>(const DataHolder<U>& dhu, Func&& f) {
//   return f(dhu);
// }

template <typename U, typename T>
DataHolder<T> operator>>(
    const DataHolder<U>& dhu,
    std::function<DataHolder<T>(const DataHolder<U>&)>& f) {
  return f(dhu);
}

/*
Lifting Mechanism
1- Write a lift function
this:
      f
    U -> T
becomes
      f*
DH<U> -> DH<T>

2- Automate lifting. It should automatically infer argument type and return type
of a function of one-argument
2-1 Define function_trait and specialize it
2-2 Write a lift_wrapper to automatically lift the function (item 1).
*/

template <typename U, typename T, typename Func>
auto lift(Func func) {
  return [func](const DataHolder<U>& dhu) -> DataHolder<T> {
    return DataHolder<T>(func(dhu.getData()));
  };
}

// Helper to deduce return type
template <typename Func, typename Arg>
using FuncReturnType = decltype(std::declval<Func>()(std::declval<Arg>()));

// This wrapper works for both function pointers and std::bind result
template <typename Arg, typename Func>
auto lift_wrapper(Func f) {
  using return_type = FuncReturnType<Func, const Arg&>;

  return lift<Arg, return_type>(f);
}

// Keeping these because even though they didn't work fully they were still
// interesting and I could come back to study them later again. template
// <typename U, typename T> DataHolder<T> operator>>(
//     const DataHolder<U>& dhu,
//     std::function<DataHolder<T>(const DataHolder<U>&)>& f) {
//   return f(dhu);
// }

// template <typename Arg, typename R>
// std::function<DataHolder<R>(const DataHolder<Arg>) &> lift(
//     std::function<R(const Arg&)>& f) {
//   return
//       [f](const DataHolder<Arg>& dh_arg) {
//         return DataHolder<R>(f(dh_arg.getData()));
//       };
// }

// template <typename U, typename T>
// std::function<DataHolder<T>(const DataHolder<U>&)> lift(T (*f)(const U&)) {
//   return
//       [f](const DataHolder<U>& dhu) { return DataHolder<T>(f(dhu.getData()));
//       };
// }
#endif