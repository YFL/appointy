project "appointy"
    language "C++"
    kind "SharedLib"
    pic "on"
    files
    {
        "./src/**.cc"
    }
    buildoptions
    {
        "-std=c++17"
    }
    libdirs
    {
        "../build/appointy_base/%{cfg.longname}/bin/"
    }
    links
    {
        "appointy_base",
        "mongocxx",
        "bsoncxx"
    }
    includedirs
    {
        "./include/",
        "../appointy_base/include/**",
        "../ext/json/include/",
        "../ext/json/include/nlohmann",
        "/usr/include/mongocxx/v_noabi",
        "/usr/include/libmongoc-1.0",
        "/usr/include/bsoncxx/v_noabi",
        "/usr/include/libbson-1.0"
    }
