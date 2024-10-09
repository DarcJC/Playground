
# Check if target slang exists.
if (NOT TARGET slang)
    message(FATAL_ERROR "Slang not found")
endif()

add_executable(SlangTest main.cpp)
target_link_libraries(SlangTest slang core compiler-core)
