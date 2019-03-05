#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

namespace kstd
{
template <class _T, _T _V>
struct integral_constant
{
    static const _T value = _V;
    typedef _T value_type;
    typedef integral_constant<_T, _V> type;
};

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template <class T, class U>
struct is_same : false_type
{
};

template <class T>
struct is_same<T, T> : true_type
{
};

template <class T>
struct remove_const
{
    typedef T type;
};
template <class T>
struct remove_const<const T>
{
    typedef T type;
};

template <class T>
struct remove_volatile
{
    typedef T type;
};
template <class T>
struct remove_volatile<volatile T>
{
    typedef T type;
};

template <class T>
struct remove_cv
{
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template <class T>
struct add_const
{
    typedef const T type;
};

template <class T>
struct add_volatile
{
    typedef volatile T type;
};

template <class T>
struct add_cv
{
    typedef const volatile T type;
};

template <class T>
struct remove_reference
{
    typedef T type;
};
template <class T>
struct remove_reference<T &>
{
    typedef T type;
};
template <class T>
struct remove_reference<T &&>
{
    typedef T type;
};

template <class T>
struct add_reference
{
    typedef T &type;
};

template <class T>
struct remove_pointer
{
    typedef T type;
};
template <class T>
struct remove_pointer<T *>
{
    typedef T type;
};
template <class T>
struct remove_pointer<T **>
{
    typedef T type;
};

template <class T>
struct add_pointer
{
    typedef T *type;
};

template <class T>
struct remove_extent
{
    typedef T type;
};
template <class T>
struct remove_extent<T[]>
{
    typedef T type;
};
template <class T, size_t N>
struct remove_extent<T[N]>
{
    typedef T type;
};

template <class T>
struct remove_all_extents
{
    typedef T type;
};
template <class T>
struct remove_all_extents<T[]> : remove_all_extents<T>
{
};
template <class T, size_t N>
struct remove_all_extents<T[N]> : remove_all_extents<T>
{
};

template <size_t len, size_t align>
struct aligned_storage
{
    struct type
    {
        alignas(align) unsigned char data[len];
    };
};

template <class T>
struct is_void : is_same<typename remove_cv<T>::type, void>
{
};

template <class T>
struct is_integral_base : false_type
{
};
template <>
struct is_integral_base<bool> : true_type
{
};
template <>
struct is_integral_base<char> : true_type
{
};
template <>
struct is_integral_base<signed char> : true_type
{
};
template <>
struct is_integral_base<unsigned char> : true_type
{
};
template <>
struct is_integral_base<wchar_t> : true_type
{
};
template <>
struct is_integral_base<short> : true_type
{
};
template <>
struct is_integral_base<int> : true_type
{
};
template <>
struct is_integral_base<long> : true_type
{
};
template <>
struct is_integral_base<long long> : true_type
{
};
template <>
struct is_integral_base<unsigned short> : true_type
{
};
template <>
struct is_integral_base<unsigned int> : true_type
{
};
template <>
struct is_integral_base<unsigned long> : true_type
{
};
template <>
struct is_integral_base<unsigned long long> : true_type
{
};

template <class T>
struct is_integral : is_integral_base<typename remove_cv<T>::type>
{
};

template <class T>
struct is_floating_point_base : false_type
{
};
template <>
struct is_floating_point_base<float> : true_type
{
};
template <>
struct is_floating_point_base<double> : true_type
{
};
template <>
struct is_floating_point_base<long double> : true_type
{
};

template <class T>
struct is_floating_point : is_floating_point_base<typename remove_cv<T>::type>
{
};

template <class T>
struct is_reference : false_type
{
};

template <class T>
struct is_reference<T &> : true_type
{
};

template <class T>
struct is_array : false_type
{
};
template <class T>
struct is_array<T[]> : true_type
{
};
template <class T, size_t N>
struct is_array<T[N]> : true_type
{
};

template <class T>
struct is_pointer : false_type
{
};
template <class T>
struct is_pointer<T *> : true_type
{
};

template <class T>
struct is_arithmetic : integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value>
{
};

template <class T>
struct is_fundamental : integral_constant<bool, is_arithmetic<T>::value || is_void<T>::value>
{
};

template <class T>
struct is_const : false_type
{
};
template <class T>
struct is_const<const T> : true_type
{
};

// primary template
template <class>
struct is_function : false_type
{
};

template <class Ret, class... Args>
struct is_function<Ret(Args...)> : true_type
{
};

template <class Ret, class... Args>
struct is_function<Ret(Args......)> : true_type
{
};

template <class Ret, class... Args>
struct is_function<Ret(Args...) const> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile &> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile &&> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile &&> : true_type
{
};

template <class Ret, class... Args>
struct is_function<Ret(Args...) noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) & noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) & noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile &noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) && noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const &&noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) && noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const &&noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile &&noexcept> : true_type
{
};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile &&noexcept> : true_type
{
};

template <class T>
struct is_object : integral_constant<bool, !(is_reference<T>::value || is_void<T>::value || is_function<T>::value)>
{
};

template <class T>
struct is_union : integral_constant<bool, __is_union(T)>
{
};

template <class T>
struct is_class : integral_constant<bool, __is_class(T)>
{
};

template <class T>
struct is_enum : integral_constant<bool, __is_enum(T)>
{
};

template <class T>
struct is_member_function_pointer_helper : false_type
{
};

template <class T, class U>
struct is_member_function_pointer_helper<T U::*> : is_function<T>
{
};

template <class T>
struct is_member_function_pointer : is_member_function_pointer_helper<remove_cv<T>>
{
};

template <typename>
struct is_member_object_pointer_base
    : false_type
{
};

template <typename _Tp, typename _Cp>
struct is_member_object_pointer_base<_Tp _Cp::*>
    : integral_constant<bool, !is_function<_Tp>::value>
{
};

template <typename T>
struct is_member_object_pointer : is_member_object_pointer_base<typename remove_cv<T>::type>::type
{
};

template <class T>
struct is_member_pointer : integral_constant<bool, is_member_function_pointer<T>::value ||
                                                       is_member_object_pointer<T>::value>
{
};

template <class T>
struct is_scalar : integral_constant<bool, is_arithmetic<T>::value ||
                                               is_enum<T>::value ||
                                               is_pointer<T>::value ||
                                               is_member_pointer<T>::value>
{
};

template <class T>
struct is_volatile : false_type
{
};

template <class T>
struct is_volatile<volatile T> : true_type
{
};

template <class T>
struct is_pod : integral_constant<bool, __is_pod(T)>
{
};

template <class T>
struct is_empty : integral_constant<bool, __is_empty(T)>
{
};

template <class T>
struct is_polymorphic : integral_constant<bool, __is_polymorphic(T)>
{
};

template <class T>
struct is_abstract : integral_constant<bool, __is_abstract(T)>
{
};

template <class T>
struct has_trivial_constructor : integral_constant<bool,
                                                   __has_trivial_constructor(T)>
{
};

template <class T>
struct has_trivial_copy : integral_constant<bool, __has_trivial_copy(T)>
{
};

template <class T>
struct has_trivial_assign : integral_constant<bool, __has_trivial_assign(T)>
{
};

template <class T>
struct has_trivial_destructor : integral_constant<bool, __has_trivial_destructor(T)>
{
};

template <class T>
struct has_nothrow_constructor : integral_constant<bool, __has_nothrow_constructor(T)>
{
};

template <class T>
struct has_nothrow_copy : integral_constant<bool, __has_nothrow_copy(T)>
{
};

template <class T>
struct has_nothrow_assign : integral_constant<bool, __has_nothrow_assign(T)>
{
};

template <class T>
struct has_virtual_destructor : integral_constant<bool, __has_virtual_destructor(T)>
{
};

template <class T>
struct is_signed_base : false_type
{
};
template <>
struct is_signed_base<signed char> : true_type
{
};
template <>
struct is_signed_base<short> : true_type
{
};
template <>
struct is_signed_base<int> : true_type
{
};
template <>
struct is_signed_base<long> : true_type
{
};
template <>
struct is_signed_base<long long> : true_type
{
};

template <class T>
struct is_signed : is_signed_base<typename remove_cv<T>::type>
{
};

template <class T>
struct is_unsigned_base : false_type
{
};
template <>
struct is_unsigned_base<unsigned char> : true_type
{
};
template <>
struct is_unsigned_base<unsigned short> : true_type
{
};
template <>
struct is_unsigned_base<unsigned int> : true_type
{
};
template <>
struct is_unsigned_base<unsigned long> : true_type
{
};
template <>
struct is_unsigned_base<unsigned long long> : true_type
{
};

template <class T>
struct is_unsigned : is_unsigned_base<typename remove_cv<T>::type>
{
};

template <class T>
struct alignment_of : integral_constant<size_t, alignof(T)>
{
};

template <class T>
struct rank : integral_constant<size_t, 0>
{
};

template <class T>
struct rank<T[]> : integral_constant<size_t, rank<T>::value + 1>
{
};

template <class T, size_t N>
struct rank<T[N]> : integral_constant<size_t, rank<T>::value + 1>
{
};

template <class T, unsigned N = 0>
struct extent : integral_constant<size_t, 0>
{
};

template <class T>
struct extent<T[], 0> : integral_constant<size_t, 0>
{
};

template <class T, unsigned N>
struct extent<T[], N> : extent<T, N - 1>
{
};

template <class T, size_t I>
struct extent<T[I], 0> : integral_constant<size_t, I>
{
};

template <class T, size_t I, unsigned N>
struct extent<T[I], N> : extent<T, N - 1>
{
};

template <typename _From, typename _To>
struct is_convertible_base
{
  private:
    typedef char __one;
    typedef struct
    {
        char __arr[2];
    } __two;
    static __one __test(_To);
    static __two __test(...);
    static _From __makeFrom();

  public:
    static const bool __value = sizeof(__test(__makeFrom())) == 1;
};

template <class T, class U>
struct is_convertible : integral_constant<bool, is_convertible_base<T, U>::__value>
{
};

template <class T, class U>
struct is_base_of : integral_constant<bool, __is_base_of(T, U)>
{
};

template <bool B, class T = void>
struct enable_if
{
};

template <class T>
struct enable_if<true, T>
{
    typedef T type;
};
} // namespace kstd

#endif