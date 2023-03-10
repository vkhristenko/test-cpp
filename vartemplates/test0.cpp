#include <iostream>
#include <tuple>
#include <string>

// no partial specialization for function templates
/*
template<std::size_t N>
void func1() {}

template<>
void func1<0>() {}

template<std::size_t N, typename... Ts>
void func(std::string const& something, std::tuple<Ts...>& ts) {
    std::cout << "level " << N << " tuple element = "<< std::get<N>(ts) << std::endl;
    func<N-1>(something, ts);
}

template<typename... Ts>
void func<0, Ts...>(std::string const& something, std::tuple<Ts...>& ts) {
    std::cout << "level 0 tuple element " << std::get<0>(ts) << std::endl;
}
*/

template<typename... Ts>
struct Some {};

template<
    template<typename...> typename X, typename... T1s,
    template<typename...> typename Y, typename... T2s
>
struct Some<X<T1s...>, Y<T2s...>> {
};

void test0() {
    auto t = std::make_tuple<int, float, double>(1, 2, 3);
    std::apply([](auto const&... args) { ((std::cout << args), ...); }, t);
    //func<2>(std::string{"something"}, t);
}

int main() {
    test0();

    return 0;
}
