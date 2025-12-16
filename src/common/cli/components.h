#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/filesystem.h>

#include <lyra/lyra.hpp>

namespace cli {
    lyra::cli dirs();
    lyra::cli client();
    lyra::cli world();
}