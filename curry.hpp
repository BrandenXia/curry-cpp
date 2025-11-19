#ifndef CURRY_HPP
#define CURRY_HPP

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace curry {

template <typename F>
struct __func_info {
  template <typename R, typename... Args>
  struct __arg_type_extractor {
    using return_type = R;
    using argument_type = std::tuple<Args...>;

    __arg_type_extractor(std::function<R(Args...)>);
  };

  using func_t = decltype(std::function{std::declval<F>()});
  using __extractor = decltype(__arg_type_extractor{std::declval<func_t>()});
  using return_type = typename __extractor::return_type;
  using argument_type = typename __extractor::argument_type;
};

// since std::strong_ordering doesn't exist before cpp20
enum class __ordering { LT, GT, EQ };
inline constexpr __ordering cmp(std::size_t a, std::size_t b) {
  if (a < b) return __ordering::LT;
  if (a > b) return __ordering::GT;
  return __ordering::EQ;
}

template <typename Tuple, std::size_t Begin, std::size_t End>
struct __tuple_slice {
  static_assert(Begin <= End, "Begin must be less than or equal to End");

  struct __helper {
    template <std::size_t... Is>
    static auto slice(std::index_sequence<Is...>)
        -> std::tuple<std::tuple_element_t<Begin + Is, Tuple>...>;
  };

  using type =
      decltype(__helper::slice(std::make_index_sequence<End - Begin>{}));
};

template <typename Tuple, std::size_t Begin, std::size_t End>
using __tuple_slice_t = typename __tuple_slice<Tuple, Begin, End>::type;

template <typename F, typename Applied = std::tuple<>,
          typename Remain = typename __func_info<F>::argument_type>
struct __curried {
  using func_info = __func_info<F>;
  typename func_info::func_t func;
  Applied applied_args;

  using return_t = typename func_info::return_type;

  template <typename... Args>
  static constexpr __ordering __cmp_remain() {
    return cmp(sizeof...(Args), std::tuple_size_v<Remain>);
  }

  template <typename... Args,
            std::enable_if_t<__cmp_remain<Args...>() == __ordering::EQ &&
                                 std::is_same_v<std::tuple<Args...>, Remain>,
                             bool> = true>
  auto operator()(Args... args) -> return_t {
    auto new_applied = std::tuple_cat(applied_args, std::make_tuple(args...));
    return std::apply(func, new_applied);
  };

  template <typename... Args>
  using __new_applied_t = decltype(std::tuple_cat(
      std::declval<Applied>(), std::declval<std::tuple<Args...>>()));
  template <typename... Args>
  using __new_remain_t =
      __tuple_slice_t<Remain, sizeof...(Args), std::tuple_size_v<Remain>>;
  template <typename... Args,
            std::enable_if_t<
                __cmp_remain<Args...>() == __ordering::LT &&
                    std::is_same_v<std::tuple<Args...>,
                                   __tuple_slice_t<Remain, 0, sizeof...(Args)>>,
                bool> = true>
  auto operator()(Args... args) {
    auto new_applied = std::tuple_cat(applied_args, std::make_tuple(args...));
    return __curried<F, __new_applied_t<Args...>, __new_remain_t<Args...>>{
        func, new_applied};
  };

  __curried(F f) : func(f), applied_args{} {}
  __curried(typename func_info::func_t f, Applied applied)
      : func(f), applied_args(applied) {}
};

template <typename F>
inline constexpr auto curried(F t) {
  using func_info = __func_info<F>;
  return __curried<typename func_info::func_t>{t};
}

} // namespace curry

#endif
