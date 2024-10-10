
# Check if target slang exists.
if (NOT TARGET slang)
    message(FATAL_ERROR "Slang not found")
endif()

add_executable(SlangTest main.cpp)
target_link_libraries(SlangTest slang core compiler-core)

# This is required to prevent program dynamic linking to slang bring with VulkanSDK
add_custom_command(TARGET SlangTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:slang>
            $<TARGET_FILE_DIR:SlangTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_PDB_FILE:slang>
            $<TARGET_FILE_DIR:SlangTest>
)
