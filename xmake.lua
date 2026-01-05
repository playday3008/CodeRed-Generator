add_rules(
    -- Normal build modes
    "mode.debug",       -- Debug mode
    "mode.release",     -- Release mode
    "mode.releasedbg",  -- Release with debug info
    "mode.minsizerel",  -- Minimum size release
    -- Special build modes
    "mode.check",       -- Sanitizers enabled (address, thread, memory, leak, undefined)
    "mode.profile",     -- Profiling mode
    "mode.coverage"     -- Code coverage mode
)

-- Generate compile_commands.json for IDE integration
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

-- Set C++ language standard to C++20, C17
set_languages("c++20", "c17")

-- Enable all, extra, and pedantic warnings
set_warnings("all", "extra", "pedantic")

-- Enable C++ exception handling
set_exceptions("cxx")

-- Enable sanitizers in check mode
if is_mode("check") then
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.thread", true)
    set_policy("build.sanitizer.memory", true)
    set_policy("build.sanitizer.leak", true)
    set_policy("build.sanitizer.undefined", true)
end

-- Engine configuration option
option("engine")
    set_default("Template")
    set_showmenu(true)
    set_description("Select the engine configuration")
    set_values("Template", "Dishonered", "BatmanAK")

-- Main library
target("CodeRedGenerator")
    set_kind("shared")
    set_basename("CodeRedGenerator")

    -- Static runtime linking for GNU toolchains on Windows
    if is_plat("mingw") then
        set_runtimes("stdc_static", "stdc++_static")
        add_shflags("-static", "-static-libgcc", "-static-libstdc++", {force = true})
    end

    -- Unicode support
    add_defines("UNICODE", "_UNICODE", "WIN32", "_CONSOLE")

    -- Windows system libraries
    add_syslinks("kernel32")
    add_syslinks("psapi")

    -- Precompiled header
    set_pcxxheader("pch.hpp")

    -- Core source files
    add_files(
        "dllmain.cpp",
        "Engine/Engine.cpp",
        "Framework/Member.cpp",
        "Framework/Printer.cpp",
        "pch.cpp"
    )

    -- Core header files (for IDE integration)
    add_headerfiles(
        "dllmain.hpp",
        "Engine/Engine.hpp",
        "Framework/Member.hpp",
        "Framework/Printer.hpp",
        "pch.hpp"
    )

    -- Engine-specific files and defines
    on_load(function (target)
        local engine = target:extraconf("options", "engine") or get_config("engine")

        -- Add preprocessor define for engine path
        target:add("defines", "ENGINE=" .. engine)

        target:add("files",
            "Engine/" .. engine .. "/Configuration.cpp",
            "Engine/" .. engine .. "/GameDefines.cpp",
            "Engine/" .. engine .. "/PiecesOfCode.cpp"
        )
        target:add("headerfiles",
            "Engine/" .. engine .. "/Configuration.hpp",
            "Engine/" .. engine .. "/GameDefines.hpp",
            "Engine/" .. engine .. "/PiecesOfCode.hpp"
        )
    end)
