#pragma once

// You need to do this to get the preprocessor to expand the arguments for reasons I don't understand
#define TOKEN_PASTE_DIRECT(x, y) x ## y
#define TOKEN_PASTE_INDIRECT(x, y) TOKEN_PASTE_DIRECT(x, y)
#define TOKEN_PASTE_INDIRECT_2(x, y) TOKEN_PASTE_INDIRECT(x, y)