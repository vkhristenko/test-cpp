#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>

void test_array() {
    std::array<int, 100> values;
    using array_type = std::array<int, 100>;
    for (unsigned int i=0; i<100; i++)
        values[i] = i;

    auto const& front = values.front();
    auto const& back = values.back();
    auto const* data = values.data();
    auto begin = values.begin();
    auto end = values.end();
    auto empty = values.empty();
    auto size = values.size();
    auto max_size = values.max_size();
    values.fill(10);
    std::array<int, 100> other;
    other.fill(15);
    values.swap(other);
    auto some_size = std::tuple_size<array_type>::value;
}

void test_deque() {
    std::deque<int> values(100);
    using deque_type = std::deque<int>;
    for (unsigned int i=0; i<100; i++)
        values[i] = i;

    auto const& begin = values.begin();
    auto const& end = values.end();
    auto size = values.size();
    auto max_size = values.max_size();

    auto elem = values.back();
    values.pop_back();
    auto another_elem = values.front();
    values.pop_front();
}

int main() {
    test_array();

    return 0;
}
