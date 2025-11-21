#pragma once

#include <utility>

using std::pair;
//template<class T1, class T2> using pr = std::pair<T1, T2>;

using std::forward;
using std::move;
// Forward 'mv' to 'move'
//template <class _Ty> constexpr std::remove_reference_t<_Ty>&& mv(_Ty&& _Arg) noexcept { return static_cast<std::remove_reference_t<_Ty>&&>(_Arg); }

using std::make_pair;
// Forward 'mkpr' to 'make_pair'
//template<class _Ty1, class _Ty2> constexpr pair<std::_Unrefwrap_t<_Ty1>, std::_Unrefwrap_t<_Ty2>> mkpr(_Ty1&& _Val1, _Ty2&& _Val2) noexcept { return std::make_pair<_Ty1, _Ty2>(std::forward<_Ty1>(_Val1), std::forward<_Ty2>(_Val2)); }