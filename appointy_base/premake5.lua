project "appointy_base"
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
        "./include/**",
        "../ext/json/include",
        "../ext/json/include/nlohmann"
    }
