#include <cassert>
#include <iostream>

using namespace std;

struct ReserveTag {
    ReserveTag(size_t capacity) {}
    size_t capacity;
};

ReserveTag Reserve(size_t capacity) {
    return ReserveTag(capacity);
}

template<typename T>
struct SimpleVector {
    SimpleVector() {
    }

    SimpleVector(ReserveTag) {

    }

    void Reserve(size_t capacity) {
    }

    size_t GetCapacity() {
        return 0;
    }

    bool IsEmpty() const {
        return true;
    }

    size_t GetSize() {
        return 0;
    }

    void PushBack(T const&) {
    }

    T const& operator[](size_t) {
        static T t;
        return t;
    }
};

void TestReserveConstructor() {
    cout << "TestReserveConstructor"s << endl;
    SimpleVector<int> v(Reserve(5));
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> v;
    // зарезервируем 5 мест в векторе
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());

    // попытаемся уменьшить capacity до 1
    v.Reserve(1);
    // capacity должно остаться прежним
    assert(v.GetCapacity() == 5);
    // поместим 10 элементов в вектор
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    // увеличим capacity до 100
    v.Reserve(100);
    // проверим, что размер не поменялся
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    // проверим, что элементы на месте
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
    cout << "Done!"s << endl;
}

int main() {
    TestReserveConstructor();
    TestReserveMethod();
}
