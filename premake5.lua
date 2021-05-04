workspace "appointy"
    toolset "clang"
    configurations {"Debug", "Release"}

filter "configurations:Debug"
    buildoptions
    {
        "-g"
    }
    
local base_dir = "./"
objdir (base_dir .. "build/%{prj.name}/%{cfg.longname}/bin_int")
targetdir (base_dir .. "build/%{prj.name}/%{cfg.longname}/bin")

include (base_dir .. "appointy_base")
include (base_dir .. "appointy")
include (base_dir .. "appointy_user")
include (base_dir .. "tests")
