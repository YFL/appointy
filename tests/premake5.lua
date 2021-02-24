project "test"
    language "C++"
    kind "ConsoleApp"
    files
    {
        "./test.cc"
    }
    includedirs
    {
        "/usr/include/mongocxx/v_noabi",
        "../appointy/include/",
        "../appointy_base/include/**",
        "../ext/json/include",
        "../ext/json/include/nlohmann"
    }
    buildoptions
    {
        "-std=c++17"
    }
    libdirs
    {
        "../build/appointy_base/%{cfg.longname}/bin/",
        "../build/appointy/%{cfg.longname}/bin/"
    }
    links
    {
        "appointy_base",
        "appointy",
        "mongocxx"
    }
