#include <iostream>
#include <utility>
#include <array>
#include <tuple>
#include <vector>
#include <string>

template<typename Array, std::size_t... I>
auto a2t_impl(Array const& a, std::index_sequence<I...>) {
    return std::make_tuple(a[I]...);
}

template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
auto a2t(std::array<T, N> const& a) {
    return a2t_impl(a, Indices{});
}

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple_impl(
        std::basic_ostream<Ch, Tr>& os,
        Tuple const& t,
        std::index_sequence<Is...>) {
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<class Ch, class Tr, class... Args>
auto& operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t) {
    os << "(";
    print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
    return os << ")";
}

void test0() {
    std::index_sequence<1,2,3,4> seq;
    std::array<int, 4> arr = {1,2,3,4};
    auto tup = a2t(arr);
    std::cout << tup << "\n";
}

template<std::size_t... Ns>
struct IndexSequence {};

template<std::size_t N, std::size_t... Is>
auto makeIndexSequenceImpl() {
    if constexpr (N == 0) return IndexSequence<Is...>{};
    else return makeIndexSequenceImpl<N-1, N-1, Is...>();
}

template<std::size_t N>
using MakeIndexSequence = std::decay<decltype(makeIndexSequenceImpl<N>())>;

void test1() {
    auto seq = MakeIndexSequence<100>{};
}

template
<
    template<typename, typename> class Gen,
    typename Tuple1,
    typename Tuple2
>
struct TypeZipper;

template<template<typename, typename> class Gen, typename Tuple1, typename Tuple2, std::size_t... Is>
auto TypeZipperImpl(
        Tuple1 const& t1,
        Tuple2 const& t2,
        std::index_sequence<Is...>) {
    return std::make_tuple(
        Gen<typename std::tuple_element<Is, Tuple1>::type, 
            typename std::tuple_element<Is, Tuple2>::type>{}...); 
}

template
<
    template<typename, typename> class Gen,
    typename... Ts1,
    typename... Ts2
>
struct TypeZipper<Gen, std::tuple<Ts1...>, std::tuple<Ts2...>> {
    static_assert(sizeof...(Ts1) == sizeof...(Ts2));
    using type = typename std::decay<decltype(
        TypeZipperImpl<Gen>(std::tuple<Ts1...>{}, std::tuple<Ts2...>{}, std::index_sequence_for<Ts1...>{}))>::type;
};

void test2() {
    using Tuple1 = std::tuple<int, float, double>;
    using Tuple2 = std::tuple<char, bool, long>;
    using ZippedType = TypeZipper<std::pair, Tuple1, Tuple2>::type;
    ZippedType x;
}

template<int... Is>
void test3impl() {
    std::cout << sizeof...(Is) << std::endl;
}

void test3() {
    test3impl<1,2,3,4,5>();
}

int main() {
    test0();
    test1();
    test2();
    test3();

    return 0;
}
