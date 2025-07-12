
#include <memory>
#include <strswitch/strswitch.hpp>
#include <string>
#include <iostream>

#define error_if(...)  \
  if ((__VA_ARGS__)) { \
    throw __LINE__;    \
  }

constexpr bool test_sswitch() {
  std::string_view str = "abc";
  int i = ss::string_switch<int>(str).case_("a", 1).case_(str, 2).case_("fdsfsdf", 3).case_("", 4);
  error_if(i != 2);
  auto v = ss::string_switch<float, int>(str)
               .case_("a", 1)
               .case_(str, 2)
               .case_("fdsfsdf", 3)
               .case_("", 4)
               .orDefault(0);
  static_assert(std::is_same_v<decltype(v), int>);
  error_if(v != 2);
  i = ss::string_switch<int>(str).orDefault(-1);
  error_if(i != -1);

  i = ss::string_switch<int>(str).case_(str, 0).cases(str, "hello", 1);
  error_if(i != 0);
  i = ss::string_switch<int>(str).case_("abs", 0).cases(str, "hello", 1).case_(str, 2);
  error_if(i != 1);

  return true;
}

void test_sswitch_nontrivial() {
  std::string_view str = "abc";
  std::string expected = "hehh";
  std::string i = ss::string_switch<std::string>(str)
                      .case_("a", "v1")
                      .case_(str, std::string(expected))
                      .case_("fdsfsdf", "v3")
                      .case_("", "v4");
  error_if(i != expected);

  auto v = ss::string_switch<std::string_view, std::string>(str)
               .case_("a", "v1")
               .case_(str, expected)
               .case_("fdsfsdf", "v3")
               .case_("", "v4")
               .orDefault("");
  static_assert(std::is_same_v<decltype(v), std::string>);
  error_if(v != expected);
  std::string v2 = ss::string_switch<std::string_view, std::string>(str)
                       .case_("a", "v1")
                       .case_(str, expected)
                       .case_("fdsfsdf", "v3")
                       .case_("", "v4");
  error_if(v2 != expected);

  i = ss::string_switch<std::string>(str).orDefault("abc");
  error_if(i != str);

  i = ss::string_switch<std::string>(str).case_(str, "1").cases(str, "hello", "2");
  error_if(i != "1");
  i = ss::string_switch<std::string>(str).case_("abs", "1").cases(str, "hello", "2").case_(str, "3");
  error_if(i != "2");
}

void test_sswitch_nonmovable() {
  using T = std::unique_ptr<int>;
  T v1(new int(1));
  T v2(new int(2));
  T v3(new int(3));
  T x = ss::string_switch<T>("2")
            .case_("1", std::move(v1))
            .cases("2", "3", std::move(v2))
            .orDefault(std::move(v3));
  error_if(v2 != nullptr);
  error_if(!x || *x != 2);
  error_if(!v1 || !v3);

  swap(x, v2);

  x = ss::string_switch<T>("1")
          .case_("1", std::move(v1))
          .cases("2", "3", std::move(v2))
          .orDefault(std::move(v3));
  error_if(v1 != nullptr);
  error_if(!x || *x != 1);
  error_if(!v2 || !v3);

  swap(x, v1);

  x = ss::string_switch<T>("4")
          .case_("1", std::move(v1))
          .cases("2", "3", std::move(v2))
          .orDefault(std::move(v3));
  error_if(v3 != nullptr);
  error_if(!x || *x != 3);
  error_if(!v1 || !v2);

  swap(x, v3);
}

template <typename Tpl1, typename Tpl2, size_t... Is>
void test_unpack(Tpl1 t1, Tpl2 t2, std::index_sequence<Is...>, std::string_view str, int expected) {
  using sswitch = ss::string_switch<int>;
  int x = (sswitch(str) | ... | sswitch::case_t{std::get<Is>(t1), std::get<Is>(t2)}).orDefault(-1);
  error_if(x != expected);
}

int main() try {
  static_assert(test_sswitch());
  test_sswitch_nontrivial();
  test_sswitch_nonmovable();
#define TEST_UNPACK(STR, EXPECTED)                                                                        \
  test_unpack(std::tuple("a", "b", "cec", "abba"), std::tuple(1, 2, 3, 4), std::make_index_sequence<4>{}, \
              STR, EXPECTED)
  TEST_UNPACK("a", 1);
  TEST_UNPACK("b", 2);
  TEST_UNPACK("cec", 3);
  TEST_UNPACK("abba", 4);
  TEST_UNPACK("adfssf", -1);

  return 0;
} catch (int line) {
  std::cerr << "\nerror on line " << line << std::endl;
  return -1;
}
