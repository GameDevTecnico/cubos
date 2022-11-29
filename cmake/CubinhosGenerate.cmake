# A function which automatically calls the cubinhos generate tool for the given target.
function(cubinhos_generate target source_dir)
    get_target_property(binary_dir ${target} BINARY_DIR)

    add_custom_command(
        OUTPUT ${binary_dir}/include
        COMMAND $<TARGET_FILE:cubinhos> generate ${source_dir} ${binary_dir}/include/components
        DEPENDS $<TARGET_FILE:cubinhos> ${source_dir}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(${target}-generate DEPENDS ${binary_dir}/include)
    add_dependencies(${target} ${target}-generate)

    target_include_directories(${target} PUBLIC ${binary_dir}/include)
endfunction()    
