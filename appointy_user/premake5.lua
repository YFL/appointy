project "appointy_user"
    language "C++"
    kind "SharedLib"
    files
    {
        "./src/**.cc"
    }
    buildoptions
    {
        "-std=c++17"
    }
    includedirs
    {
        "/usr/include/mongocxx/v_noabi",
        "/usr/include/bsoncxx/v_noabi",
        "./include/",
        "../ext/json/include",
        "../appointy_base/include/**",
        "../ext/json/include/nlohmann"
    }
