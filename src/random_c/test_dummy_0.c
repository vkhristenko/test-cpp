#include <stdio.h>

#define nullptr NULL

typedef struct {
    char const* data;
    size_t size;
} StringView;

#define CONCAT_IMPL(x, y) x ## y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

/// generic array
#define ARRAY(TYPE, SIZE) CONCAT(CONCAT(CONCAT(Array_, TYPE), _), SIZE)
#define DECLARE_ARRAY(TYPE, SIZE) \
    typedef struct { \
        TYPE data[CONCAT(CONCAT(CONCAT(CONCAT(Array_, TYPE), _), SIZE), _SIZE)]; \
    } ARRAY(TYPE, SIZE); \
    \
    size_t CONCAT(ARRAY(TYPE, SIZE), _size)(ARRAY(TYPE, SIZE) const* arr) { \
        return SIZE; \
    }\
    TYPE const* CONCAT(ARRAY(TYPE, SIZE), _at)(ARRAY(TYPE, SIZE) const* arr, size_t i) { \
        return &(arr->data[i]); \
    }

/// declare the type 
#define Array_int_10_SIZE 10
DECLARE_ARRAY(int, 10);

void PassStructWithArray(ARRAY(int, 10) arr) {
   printf("%p\n", &(arr.data[0]));
}

void PassArray(int arr[10]) {
   printf("%p\n", &(arr[0]));
}

void TestArray() {
    ARRAY(int, 10) ints = {
        .data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };
    printf("%p\n", &(ints.data[0]));
    PassStructWithArray(ints);
    PassArray(ints.data);

    int const* value_0 = Array_int_10_at(&ints, 0);
    for (size_t i=0; i<Array_int_10_size(&ints); i++) {
        printf("[%u] = %d\n", i, *Array_int_10_at(&ints, i));
    }
}

int ProcessString(char const* str) {
    if (str == nullptr) {
        return 0;
    }

    int n = 0;
    while (*str != '\0') {
        n++;
        str++;
    }

    return n;
}

int main(int argc, char** argv) {
    printf("%d\n", ProcessString(argv[1]));

    TestArray();

    return 0;
}
