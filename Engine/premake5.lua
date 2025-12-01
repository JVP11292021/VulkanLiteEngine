workspace "VleEngine"
	architecture "x64"
	startproject "Renderer"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
local vulkan_sdk = os.getenv("VULKAN_SDK")
if vulkan_sdk == nil then
    print("[Engine] WARNING: VULKAN_SDK environment variable not set!")
    print("          Vulkan will NOT be included in the build.")
else
    print("[Engine] Using Vulkan SDK at: " .. vulkan_sdk)
end

project "EngineBackend"
	location "EngineBackend"
	kind "staticlib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.inl",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.c",
	}

	includedirs {
		"Libraries/include",
	}

	libdirs {
		"Libraries/lib",
	}

	links {
		"glfw3_mt.lib"
	}

	defines {
		"VLE_BUILD_DLL",
	}

	if vulkan_sdk ~= nil then
        includedirs {
            vulkan_sdk .. "/Include"
        }

        libdirs {
            vulkan_sdk .. "/Lib"
        }

        links {
            "vulkan-1.lib"
        }

        defines {
            "USE_VULKAN"
        }
    end

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VLE_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "VLE_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "VLE_DIST"
		optimize "on"
		runtime "Release"

project "Renderer"
	location "Renderer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.inl",
		"%{prj.name}/**.cpp",
	}

	includedirs {
		"Libraries/include",
		"Engine",
	}

	libdirs {
		"Libraries/lib",
	}

	links {
		"Engine",
	}

	if vulkan_sdk ~= nil then
        includedirs {
            vulkan_sdk .. "/Include"
        }

        libdirs {
            vulkan_sdk .. "/Lib"
        }

        links {
            "vulkan-1.lib"
        }

        defines {
            "USE_VULKAN"
        }
    end

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "APP_VLE_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "APP_VLE_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "APP_VLE_DIST"
		optimize "on"
		runtime "Release"