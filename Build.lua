-- premake5.lua
workspace "Sentinel"
   architecture "ARM64"
   configurations { "Debug", "Release", "Dist" }
   platforms { "Mac", "Linux", "Windows" }
   startproject "App"

   toolset "clang"

   local vcpkg_triplet = ""

   filter "platforms:Mac"
      system "macosx"
      architecture "ARM64"
      vcpkg_triplet = "arm64-osx"
    
   filter "platforms:Linux"
      system "linux"
      architecture "x86_64"
      vcpkg_triplet = "x64-linux"
       
    
   filter "platforms:Windows"
      system "windows"
      architecture "x86_64"
      vcpkg_triplet = "x64-windows"

   filter {}

   includedirs { 
       "vcpkg_installed/" .. vcpkg_triplet .. "/include" 
   }
   libdirs { 
       "vcpkg_installed/" .. vcpkg_triplet .. "/lib",
       "vcpkg_installed/" .. vcpkg_triplet .. "/lib/manual-link"
   }

   links { 
       "SDL3" 
   }
   
OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "Core/Build-Core.lua"
group ""

include "App/Build-App.lua"