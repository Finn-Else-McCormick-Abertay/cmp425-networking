#pragma once

#include <map>
#include <unordered_map>

template<typename K, typename V> using hashmap = std::unordered_map<K, V>;
using std::unordered_map;

template<typename K, typename V> using bstmap = std::map<K, V>;
template<typename K, typename V> using ordered_map = std::map<K, V>;