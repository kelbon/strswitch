#pragma once

#include <cassert>
#include <optional>
#include <string_view>

namespace ss {

// inspired by llvm string switch (https://llvm.org/doxygen/classllvm_1_1StringSwitch.html)
//
// cozy and effective (specific compiler optimizations for this case exist) way to map string to value
// primary use case is converting enum to value or something similar
// example:
//
//   std::string_view str = ...;
//   int r = string_switch<int>(str)
//               .case_("hello", 0)
//               .case_("world", 1)
//               .cases("abc", "cba", 5)
//               .orDefault(-1);
//
// case accepts 'T' (T&& for non trivial types) but final result converted to 'R'
template <typename T, typename R = T>
struct string_switch {
 private:
  // string we are matching
  std::string_view const m_str;
  // result of this switch statement
  std::optional<T> m_result;

 public:
  constexpr explicit string_switch(std::string_view s) : m_str(s), m_result(std::nullopt) {
  }

  string_switch(string_switch const&) = delete;

  void operator=(string_switch const&) = delete;
  void operator=(string_switch&&) = delete;

  constexpr string_switch(string_switch&& other) : m_str(other.m_str), m_result(std::move(other.m_result)) {
  }

  ~string_switch() = default;

  static_assert(!std::is_reference_v<T>);
  // make string switch more effective for non trivial types (but anyway its best usage for trivial types)
  using value_t = std::conditional_t<std::is_trivially_copy_constructible_v<T>, T, T&&>;

 private:
  constexpr string_switch& elif (std::string_view s, value_t value) {
    if (!m_result && m_str == s)
      m_result = std::move(value);
    return *this;
  }

 public:
  [[nodiscard]] constexpr string_switch& endsWith(std::string_view s, value_t value) {
    if (!m_result && m_str.ends_with(s))
      m_result = std::move(value);
    return *this;
  }

  [[nodiscard]] constexpr string_switch& startsWith(std::string_view s, value_t value) {
    if (!m_result && m_str.starts_with(s))
      m_result = std::move(value);
    return *this;
  }
  [[nodiscard]] constexpr string_switch& case_(std::string_view s0, value_t value) {
    return elif (s0, (value_t)value);
  }
  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, value_t value) {
    return case_(s0, (value_t)value);
  }
  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, value_t value) {
    return elif (s0, (value_t)value).elif (s1, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               T value) {
    return elif (s0, (value_t)value).cases(s1, s2, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, value_t value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, value_t value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, std::string_view s5,
                                               T value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, s5, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, std::string_view s5,
                                               std::string_view s6, value_t value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, s5, s6, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, std::string_view s5,
                                               std::string_view s6, std::string_view s7, value_t value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, s5, s6, s7, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, std::string_view s5,
                                               std::string_view s6, std::string_view s7, std::string_view s8,
                                               T value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, s5, s6, s7, s8, (value_t)value);
  }

  [[nodiscard]] constexpr string_switch& cases(std::string_view s0, std::string_view s1, std::string_view s2,
                                               std::string_view s3, std::string_view s4, std::string_view s5,
                                               std::string_view s6, std::string_view s7, std::string_view s8,
                                               std::string_view s9, value_t value) {
    return elif (s0, (value_t)value).cases(s1, s2, s3, s4, s5, s6, s7, s8, s9, (value_t)value);
  }

  struct case_t {
    std::string_view key;
    T value;
  };
  // used to expand like
  //   T m_result = (string_switch(m_str) | ... | case_t(keys_pack, values_pack)).or_default(val)
  [[nodiscard]] constexpr string_switch& operator|(case_t p) {
    return case_(p.key, (value_t)p.value);
  }

  [[nodiscard]] constexpr R orDefault(value_t value) {
    if (m_result) {
      return R(std::move(*m_result));
    }
    return R((value_t)value);
  }

  [[nodiscard]] constexpr operator R() {
    assert(m_result && "Fell off the end of a string-switch");
    return R(std::move(*m_result));
  }
};

}  // namespace ss
