#pragma once

struct construct_noinit_t { constexpr explicit construct_noinit_t(int) {} };
inline constexpr construct_noinit_t construct_noinit {0};