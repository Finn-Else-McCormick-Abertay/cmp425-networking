#include "console.h"

using namespace console::ANSI;

console::ANSI::Effect::Effect(std::string code, std::string opposite) : _code(code), _opposite(opposite) {}

console::ANSI::Effect console::ANSI::Effect::reverse() const { return Effect(_opposite, _code); }
console::ANSI::Effect console::ANSI::Effect::operator!() const { return reverse(); }

std::string console::ANSI::Effect::to_string() const { return "\033[" + _code + "m"; }
std::string console::ANSI::Effect::code() const { return _code; }

std::ostream& console::ANSI::operator<<(std::ostream& os, const Effect& effect) { return os << effect.to_string(); }