-- premake5.lua
workspace "Sentinel"
   architecture "ARM64"
   configurations { "Debug", "Release", "Dist" }
   platforms { "Mac", "Linux", "Windows" }
   startproject "App"

   toolset "clang"

   filter "platforms:Mac"
      system "macosx"
      architecture "ARM64"
      includedirs { "vcpkg_installed/arm64-osx/include" }
      libdirs { "vcpkg_installed/arm64-osx/lib" }
      links {
         "CoreMedia.framework",
         "CoreVideo.framework",
         "Cocoa.framework",
         "UniformTypeIdentifiers.framework",
         "IOKit.framework",
         "ForceFeedback.framework",
         "Carbon.framework",
         "CoreAudio.framework",
         "AudioToolbox.framework",
         "AVFoundation.framework",
         "Foundation.framework",
         "GameController.framework",
         "Metal.framework",
         "QuartzCore.framework",
         "CoreHaptics.framework",
         "pthread",
         "m",
         "objc"
      }
    
   filter "platforms:Linux"
      system "linux"
      architecture "x86_64"
      includedirs { "vcpkg_installed/x64-linux/include" }
      libdirs { "vcpkg_installed/x64-linux/lib", "vcpkg_installed/x64-linux/lib/manual-link" }
     
   filter "platforms:Windows"
      system "windows"
      architecture "x86_64"
      includedirs { "vcpkg_installed/x64-windows/include" }
      libdirs { "vcpkg_installed/x64-windows/lib", "vcpkg_installed/x64-windows/lib/manual-link" }

   filter {}

   links { 
       "SDL3" 
   }
   
OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "Core/Build-Core.lua"
group ""

include "App/Build-App.lua"