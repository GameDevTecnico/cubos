# A function which automatically calls the cubinhos generate tool for the given target.
function(cubinhos_generate target source_dir)
    get_target_property(binary_dir ${target} BINARY_DIR)

    add_custom_command(
        OUTPUT ${binary_dir}/${target}-components.cpp
        COMMAND $<TARGET_FILE:cubinhos> generate ${source_dir} ${binary_dir}/${target}-components.cpp
        DEPENDS $<TARGET_FILE:cubinhos> ${source_dir}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(${target}-generate DEPENDS ${binary_dir}/${target}-components.cpp)
    add_dependencies(${target} ${target}-generate)

    target_sources(${target} PUBLIC ${binary_dir}/${target}-components.cpp)
endfunction()    
