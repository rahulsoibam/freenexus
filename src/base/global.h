#pragma once

#include <type_traits>
#include <QtGlobal>

const char C_TORRENT_FILE_EXTENSION[] = ".torrent";


#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
template <typename T>
constexpr typename std::add_const<T>::type &qAsConst(T &t) noexcept { return t; }

// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) = delete;
#endif

// returns a const object copy
template <typename T>
constexpr typename std::add_const<T>::type copyAsConst(T &&t) noexcept { return std::move(t); }
