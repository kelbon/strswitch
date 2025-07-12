Small header-only library for efficient string-to-value mapping. Perfect way for converting strings to enums or other values (inspired by LLVM).

### example

```cpp
#include <strswitch/strswitch.hpp>

enum struct Color { Red, Green, Blue, Unknown };

Color parse_color(std::string_view str) {
    return ss::string_switch<Color>(str)
        .case_("red", Color::Red)
        .case_("green", Color::Green)
        .case_("blue", Color::Blue)
        .orDefault(Color::Unknown);
}
...
Color c = parse_color("green");  // Returns Color::Green
