set(_sentinel_vcpkg_root "$ENV{VCPKG_ROOT}")

if(NOT _sentinel_vcpkg_root)
    set(_sentinel_vcpkg_root "$ENV{HOME}/vcpkg")
endif()

set(
    _sentinel_vcpkg_toolchain
    "${_sentinel_vcpkg_root}/scripts/buildsystems/vcpkg.cmake"
)

if(NOT EXISTS "${_sentinel_vcpkg_toolchain}")
    message(
        FATAL_ERROR
        "vcpkg was not found. Set VCPKG_ROOT to the vcpkg installation."
    )
endif()

include("${_sentinel_vcpkg_toolchain}")
