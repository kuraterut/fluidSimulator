#pragma once

#include "type_list.hpp"

#include <string_view>
#include <tuple>

using namespace std;

template<typename T>
concept is_type_marker = requires(const T& t, string_view s) {
    typename T::type;

    { t.matches(s) } -> same_as<bool>;
} && is_default_constructible<T>::value;

template<typename T>
struct matches_list;

template<is_type_marker... Ms>
struct matches_list<type_list<Ms...>> {
public:    
    bool operator()(const array<string_view, sizeof...(Ms)>& s) {
        return helper(s, make_tuple<Ms...>(Ms{}...), make_index_sequence<sizeof...(Ms)>{});
    }

private:
    template<size_t... Is>
    bool helper(
        const array<string_view, sizeof...(Ms)>& s,
        const tuple<Ms...>& ms,
        index_sequence<Is...>
    ) {return (get<Is>(ms).matches(s[Is]) && ...);}
};

template<typename T>
struct is_type_marker_list_ : false_type {};

template<is_type_marker... Ms>
struct is_type_marker_list_<type_list<Ms...>> : true_type {};

template<typename T>
concept is_type_marker_list = is_type_marker_list_<T>::value;

template<typename... Ts>
struct run_for_matching;

template<typename F, is_type_marker... Ms, is_type_marker_list... Ls>
struct run_for_matching<F, type_list<type_list<Ms...>, Ls...>> {
public:
    bool operator()(F& func, const array<string_view, sizeof...(Ms)>& types) {
        if (matches_list<type_list<Ms...>>{}(types)) {
            func.template execute<typename Ms::type...>();
            return true;
        }
        return run_for_matching<F, type_list<Ls...>>{}(func, types);
    }
};

template<typename F>
struct run_for_matching<F, type_list<>> {
public:
    template<size_t N>
    bool operator()(F& func, const array<string_view, N>& types) {
        return false;
    }
};
