# Copy DLLs from a directory to a target's output directory
function(copy_dlls_from_dir source_dir to_target)

    if(NOT EXISTS "${source_dir}")
        message(FATAL_ERROR "Source folder ${source_dir} does no exist.")
    endif()

    file(GLOB dll_files "${source_dir}/*.dll")

    foreach(dll_file ${dll_files})
        add_custom_command(TARGET ${to_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${dll_file}
                $<TARGET_FILE_DIR:${to_target}>
            COMMENT "Copying ${dll_file} to $<TARGET_FILE_DIR:${to_target}> directory"
        )
    endforeach()

endfunction()


# Copy DLLs from one target to another target's output directory
function(copy_dlls from_target to_target)

    add_custom_command(TARGET ${to_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${from_target}>
            $<TARGET_FILE_DIR:${to_target}>
        COMMENT "Copying ${dll_file} to $<TARGET_FILE_DIR:${to_target}> directory"
    )

endfunction()
