cmake_minimum_required(VERSION 3.20)

# Genuine PRX/SPRX authoring needs PS3DK's PRX CRT/linker scripts and prx-gen.
# The legacy /usr/local/ps3dev GCC 7.2 toolchain is NOT interchangeable.
project(EsctasyGTA112 LANGUAGES C CXX)

include(ps3-self)

add_executable(Esctasy112
    ../GTA112SPRX/ps3dev/prx_ps3dev.cpp
)
target_include_directories(Esctasy112 PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../GTA112SPRX"
)
target_compile_features(Esctasy112 PRIVATE cxx_std_11)
target_compile_definitions(Esctasy112 PRIVATE ESCTASY_PS3DEV=1)
target_compile_options(Esctasy112 PRIVATE
    -O2
    -fno-exceptions -fno-rtti -fno-threadsafe-statics
    -Wno-write-strings -Wno-unused-function
    -Wno-deprecated-declarations
    -Wno-unused-parameter
)
# PS3DK's PPU driver already supplies its target libraries and the PRX
# crt/metadata object. Avoid explicitly adding lv2/rt/m here: on this SDK
# configuration those entries cause CMake to repeat lv2-prx-crt.o, producing
# "multiple definition of __sys_prx_module_info" at link time.
# The toolchain also injects rt at directory scope.
#
# Should a newer SDK require extra libraries, add only the library actually
# needed, and never add lv2-prx-crt.o manually.

# PS3DK provides the PRX-specific crt, linker script, relocatable image
# converter and validation. SIGN produces both .sprx and .fake.sprx.
# No ELF is renamed to a PRX.
ps3_add_prx(Esctasy112
    NAME Esctasy112
    VERSION 1.0
    OUTPUT_NAME GTA112.prx
    SIGN
)

# Ensure the module metadata CRT object occurs only once. CMake's native
# static-library dependency ordering can duplicate direct link inputs.
# It is not safe to link two PRX CRT copies: both define the same module info.
get_target_property(_esctasy_link_items Esctasy112 LINK_LIBRARIES)
if(_esctasy_link_items)
    list(REMOVE_DUPLICATES _esctasy_link_items)
    set_property(TARGET Esctasy112 PROPERTY LINK_LIBRARIES "${_esctasy_link_items}")
endif()
