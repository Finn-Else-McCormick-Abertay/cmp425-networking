#include "ansi.h"

using namespace std;
using namespace ANSI;

Effect::Effect(string code, string opposite) : _code(code), _opposite(opposite) {}

Effect Effect::reverse() const { return Effect(_opposite, _code); }
Effect Effect::operator!() const { return reverse(); }

string Effect::to_string() const { return "\033[" + _code + "m"; }
string Effect::code() const { return _code; }

ostream& ANSI::operator<<(ostream& os, const Effect& effect) { return os << effect.to_string(); }