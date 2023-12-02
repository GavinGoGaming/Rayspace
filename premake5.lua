-- premake5.lua
workspace "Rayspace"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Rayspace"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Rayspace"