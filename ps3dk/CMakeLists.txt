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
target_link_libraries(Esctasy112 PRIVATE lv2 rt m)

# PS3DK provides the PRX-specific crt, linker script, relocatable image
# converter and validation. SIGN produces both .sprx and .fake.sprx.
# No ELF is renamed to a PRX.
ps3_add_prx(Esctasy112
    NAME Esctasy112
    VERSION 1.0
    OUTPUT_NAME GTA112.prx
    SIGN
)
