// RUN: %clang_cc1 -fexperimental-new-constant-interpreter -verify %s
// RUN: %clang_cc1 -verify=ref %s

constexpr int m = 3;
constexpr const int *foo[][5] = {
  {nullptr, &m, nullptr, nullptr, nullptr},
  {nullptr, nullptr, &m, nullptr, nullptr},
  {nullptr, nullptr, nullptr, &m, nullptr},
};

static_assert(foo[0][0] == nullptr, "");
static_assert(foo[0][1] == &m, "");
static_assert(foo[0][2] == nullptr, "");
static_assert(foo[0][3] == nullptr, "");
static_assert(foo[0][4] == nullptr, "");
static_assert(foo[1][0] == nullptr, "");
static_assert(foo[1][1] == nullptr, "");
static_assert(foo[1][2] == &m, "");
static_assert(foo[1][3] == nullptr, "");
static_assert(foo[1][4] == nullptr, "");
static_assert(foo[2][0] == nullptr, "");
static_assert(foo[2][1] == nullptr, "");
static_assert(foo[2][2] == nullptr, "");
static_assert(foo[2][3] == &m, "");
static_assert(foo[2][4] == nullptr, "");


/// A init list for a primitive value.
constexpr int f{5};
static_assert(f == 5, "");


constexpr int getElement(int i) {
  int values[] = {1, 4, 9, 16, 25, 36};
  return values[i];
}
static_assert(getElement(1) == 4, "");
static_assert(getElement(5) == 36, "");

constexpr int data[] = {5, 4, 3, 2, 1};
constexpr int getElement(const int *Arr, int index) {
  return *(Arr + index);
}

static_assert(getElement(data, 1) == 4, "");
static_assert(getElement(data, 4) == 1, "");

constexpr int getElementFromEnd(const int *Arr, int size, int index) {
  return *(Arr + size - index - 1);
}
static_assert(getElementFromEnd(data, 5, 0) == 1, "");
static_assert(getElementFromEnd(data, 5, 4) == 5, "");


constexpr static int arr[2] = {1,2};
constexpr static int arr2[2] = {3,4};
constexpr int *p1 = nullptr;
constexpr int *p2 = p1 + 1; // expected-error {{must be initialized by a constant expression}} \
                            // expected-note {{cannot perform pointer arithmetic on null pointer}} \
                            // ref-error {{must be initialized by a constant expression}} \
                            // ref-note {{cannot perform pointer arithmetic on null pointer}}
constexpr int *p3 = p1 + 0;
constexpr int *p4 = p1 - 0;
constexpr int *p5 =  0 + p1;
constexpr int *p6 =  0 - p1; // expected-error {{invalid operands to binary expression}} \
                             // ref-error {{invalid operands to binary expression}}

constexpr int const * ap1 = &arr[0];
constexpr int const * ap2 = ap1 + 3; // expected-error {{must be initialized by a constant expression}} \
                                     // expected-note {{cannot refer to element 3 of array of 2}} \
                                     // ref-error {{must be initialized by a constant expression}} \
                                     // ref-note {{cannot refer to element 3 of array of 2}}

constexpr auto ap3 = arr - 1; // expected-error {{must be initialized by a constant expression}} \
                              // expected-note {{cannot refer to element -1}} \
                              // ref-error {{must be initialized by a constant expression}} \
                              // ref-note {{cannot refer to element -1}}
constexpr int k1 = &arr[1] - &arr[0];
static_assert(k1 == 1, "");
static_assert((&arr[0] - &arr[1]) == -1, "");

constexpr int k2 = &arr2[1] - &arr[0]; // expected-error {{must be initialized by a constant expression}} \
                                       // ref-error {{must be initialized by a constant expression}}

static_assert((arr + 0) == arr, "");
static_assert(&arr[0] == arr, "");
static_assert(*(&arr[0]) == 1, "");
static_assert(*(&arr[1]) == 2, "");

constexpr const int *OOB = (arr + 3) - 3; // expected-error {{must be initialized by a constant expression}} \
                                          // expected-note {{cannot refer to element 3 of array of 2}} \
                                          // ref-error {{must be initialized by a constant expression}} \
                                          // ref-note {{cannot refer to element 3 of array of 2}}

template<typename T>
constexpr T getElementOf(T* array, int i) {
  return array[i];
}
static_assert(getElementOf(foo[0], 1) == &m, "");


template <typename T, int N>
constexpr T& getElementOfArray(T (&array)[N], int I) {
  return array[I];
}
static_assert(getElementOfArray(foo[2], 3) == &m, "");


static_assert(data[0] == 4, ""); // expected-error{{failed}} \
                                 // expected-note{{5 == 4}} \
                                 // ref-error{{failed}} \
                                 // ref-note{{5 == 4}}


constexpr int dynamic[] = {
  f, 3, 2 + 5, data[3], *getElementOf(foo[2], 3)
};
static_assert(dynamic[0] == f, "");
static_assert(dynamic[3] == 2, "");


constexpr int dependent[4] = {
  0, 1, dependent[0], dependent[1]
};
static_assert(dependent[2] == dependent[0], "");
static_assert(dependent[3] == dependent[1], "");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma clang diagnostic ignored "-Winitializer-overrides"
constexpr int DI[] = {
  [0] = 10,
  [1] = 20,
  30,
  40,
  [1] = 50
};
static_assert(DI[0] == 10, "");
static_assert(DI[1] == 50, "");
static_assert(DI[2] == 30, "");
static_assert(DI[3] == 40, "");

constexpr int addThreeElements(const int v[3]) {
  return v[0] + v[1] + v[2];
}
constexpr int is[] = {10, 20, 30 };
static_assert(addThreeElements(is) == 60, "");

struct fred {
  char s [6];
  int n;
};

struct fred y [] = { [0] = { .s[0] = 'q' } };
#pragma clang diagnostic pop

namespace indices {
  constexpr int first[] = {1};
  constexpr int firstValue = first[2]; // ref-error {{must be initialized by a constant expression}} \
                                       // ref-note {{cannot refer to element 2 of array of 1}} \
                                       // expected-error {{must be initialized by a constant expression}} \
                                       // expected-note {{cannot refer to element 2 of array of 1}}

  constexpr int second[10] = {17};
  constexpr int secondValue = second[10];// ref-error {{must be initialized by a constant expression}} \
                                         // ref-note {{read of dereferenced one-past-the-end pointer}} \
                                         // expected-error {{must be initialized by a constant expression}} \
                                         // expected-note {{read of dereferenced one-past-the-end pointer}}

  constexpr int negative = second[-2]; // ref-error {{must be initialized by a constant expression}} \
                                       // ref-note {{cannot refer to element -2 of array of 10}} \
                                       // expected-error {{must be initialized by a constant expression}} \
                                       // expected-note {{cannot refer to element -2 of array of 10}}
};

namespace DefaultInit {
  template <typename T, unsigned N>
  struct B {
    T a[N];
  };

  int f() {
     constexpr B<int,10> arr = {};
     constexpr int x = arr.a[0];
  }
};

namespace IncDec {
  // FIXME: Pointer arithmethic needs to be supported in inc/dec
  //   unary operators
#if 0
  constexpr int getNextElem(const int *A, int I) {
    const int *B = (A + I);
    ++B;
    return *B;
  }
  constexpr int E[] = {1,2,3,4};

  static_assert(getNextElem(E, 1) == 3);
#endif
};
