#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/filesystem.h>

#include <lyra/lyra.hpp>
#include <console.h>

inline bool handle_cli(int argc, char** argv, const lyra::cli& cli, bool exit_on_failure = true, bool exit_on_help = true) {
    bool show_help = false;
    auto cli_final = lyra::help(show_help) | cli;

    auto result = cli_final.parse({ argc, argv });
    if (!result) {
        print<error>(result.message());
        if (exit_on_failure) exit(1);
        return false;
    }

    if (show_help) {
        print<info>("TK : HELP MSG");
        if (exit_on_help) exit(0);
    }
    return true;
}