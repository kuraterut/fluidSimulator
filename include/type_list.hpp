#pragma once

#include <type_traits>

using namespace std;

/// list
/// A list of types. Is written via [...] in further comments:
/// list<A, B, C> is [A, B, C]
template<typename... Ts>
struct type_list {};

/// is_list<T>
/// Checks if T is list.
template<typename T>
struct is_list_ : false_type {};

template<typename... Ts>
struct is_list_<type_list<Ts...>> : true_type {};

template<typename T>
concept is_list = is_list_<T>::value;

/// push_front<T, [Ts...]>
/// Pushed T to list.
/// Example:
///     push_front<A, [B, C]> -> [A, B, C]
template<typename... Ts>
struct push_front;

template<typename T, typename... Ts>
struct push_front<T, type_list<Ts...>> {
    using type = type_list<T, Ts...>;
};

/// push_front_for_each<T, [[Ts...]...]>
/// Pushes T to each item in the list of lists.
/// Example:
///     push_front_for_each<A, [[B, C], [D, E]]> -> [[A, B, C], [A, D, E]]
template<typename... Ts>
struct push_front_for_each;

template<typename T, is_list... Ls>
struct push_front_for_each<T, type_list<Ls...>> {
    using type = type_list<typename push_front<T, Ls>::type...>;
};

/// concat<[Ts...]...>
/// Concatenates N lists.
/// Example:
///     concat<[A, B], [C, D], [E, F]> -> [A, B, C, D, E, F]
template<typename... Ts>
struct concat;

template<typename... Ts, typename... Ls>
struct concat<type_list<Ts...>, Ls...> {
    using type = typename concat<type_list<Ts...>, typename concat<Ls...>::type>::type;
};

template<typename... Ts, typename... Us>
struct concat<type_list<Ts...>, type_list<Us...>> {
    using type = type_list<Ts..., Us...>;
};

template<typename... Ts>
struct concat<type_list<Ts...>> {
    using type = type_list<Ts...>;
};

template<>
struct concat<> {
    using type = type_list<>;
};

/// product<[Ts...]...>
/// Creates cartesian product of N lists.
/// Example:
///     product<[A, B], [C, D], [E, F]> -> [
///         [A, C, E],
///         [A, C, F],
///         [A, D, E],
///         [A, D, F],
///         [B, C, E],
///         [B, C, F],
///         [B, D, E],
///         [B, D, F],
///     ]
template<typename... Ts>
struct product;

template<typename... Ts, is_list... Ls>
struct product<type_list<Ts...>, Ls...> {
    using type = typename concat<
            typename push_front_for_each<
                Ts,
                typename product<Ls...>::type
            >::type...
        >::type;
};

template<typename... Ts>
struct product<type_list<Ts...>> {
    using type = type_list<type_list<Ts>...>;
};

template<>
struct product<> {
    using type = type_list<>;
};
