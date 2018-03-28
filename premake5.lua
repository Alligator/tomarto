workspace "tomarto"
    location "build"
    configurations { "Debug", "Release" }

project "tomarto"
    kind "WindowedApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.h", "**.c", "resource.rc" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
