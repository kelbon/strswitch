
#include <strswitch/strswitch.hpp>

#include <benchmark/benchmark.h>
#include <unordered_map>
#include <string_view>
#include <array>
#include <iostream>

#if not defined(NDEBUG)
  #error benchmark only runs in release mode
#endif

enum Color { Red, Green, Blue, Yellow, Black, White, Unknown };

constexpr std::array<std::string_view, 7> color_strings = {
    "red", "green", "blue", "yellow", "black", "white", "notacolor",
};

Color string_switch_impl(std::string_view str) {
  return ss::string_switch<Color>(str)
      .case_("red", Red)
      .case_("green", Green)
      .case_("blue", Blue)
      .case_("yellow", Yellow)
      .case_("black", Black)
      .case_("white", White)
      .orDefault(Unknown);
}

Color unordered_map_impl(std::string_view str) {
  static const auto map = [] {
    std::unordered_map<std::string_view, Color> m;
    m["red"] = Red;
    m["green"] = Green;
    m["blue"] = Blue;
    m["yellow"] = Yellow;
    m["black"] = Black;
    m["white"] = White;
    return m;
  }();

  auto it = map.find(str);
  return it != map.end() ? it->second : Unknown;
}

Color if_else_impl(std::string_view str) {
  if (str == "red")
    return Red;
  if (str == "green")
    return Green;
  if (str == "blue")
    return Blue;
  if (str == "yellow")
    return Yellow;
  if (str == "black")
    return Black;
  if (str == "white")
    return White;
  return Unknown;
}

static void BM_string_switch(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto& str : color_strings) {
      benchmark::DoNotOptimize(string_switch_impl(str));
    }
  }
}

static void BM_unordered_map(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto& str : color_strings) {
      benchmark::DoNotOptimize(unordered_map_impl(str));
    }
  }
}

static void BM_if_else(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto& str : color_strings) {
      benchmark::DoNotOptimize(if_else_impl(str));
    }
  }
}

// order important (for console reporter)
BENCHMARK(BM_string_switch);
BENCHMARK(BM_unordered_map);
BENCHMARK(BM_if_else);

// autocheck results
struct BenchmarkReporter : benchmark::ConsoleReporter {
 public:
  void ReportRuns(const std::vector<Run>& reports) override {
    ConsoleReporter::ReportRuns(reports);

    // ReportRuns called after each BENCHMARK done, required to compare only after all
    if (reports.size() != 3)
      return;

    const auto& string_switch_time = reports[0].real_accumulated_time;
    const auto& unordered_map_time = reports[1].real_accumulated_time;
    const auto& if_else_time = reports[2].real_accumulated_time;

    std::cout << "\n=== Results ===\n";
    std::cout << "string_switch vs unordered_map: "
              << (string_switch_time < unordered_map_time ? "FASTER" : "SLOWER") << " (by "
              << unordered_map_time / string_switch_time << " times)\n";

    std::cout << "string_switch vs if/else: " << (string_switch_time < if_else_time ? "FASTER" : "SLOWER")
              << " (by " << if_else_time / string_switch_time << " times)\n";

    if (string_switch_time >= unordered_map_time) {
      std::cerr << "\nERROR: string_switch not faster than unordered_map!\n";
      std::exit(1);
    }
  }
};

int main(int argc, char** argv) {
  BenchmarkReporter reporter;
  benchmark::RunSpecifiedBenchmarks(&reporter);

  return 0;
}
