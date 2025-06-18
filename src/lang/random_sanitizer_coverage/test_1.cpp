#define BAR(x) ((x) || (x))

template <typename T>
void foo(T x) {
    for (unsigned i = 0; i < 10; ++i) {
        BAR(i);
    }
}

int main() {
    foo<int>(0);
    foo<float>(0);

    return 0;
}
