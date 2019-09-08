#ifndef OVERLOADED_H
#define OVERLOADED_H 1

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;
template <typename... Ts> struct print;

#endif