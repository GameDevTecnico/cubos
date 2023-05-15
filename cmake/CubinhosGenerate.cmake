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

    # Link the generated source file with either:
    # - the passed target, if it is an executable (PRIVATE).
    # - the targets which link to the passed target, if it is a library (INTERFACE).
    # See #378
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL "EXECUTABLE")
        target_sources(${target} PRIVATE ${binary_dir}/${target}-components.cpp)
    else()
        target_sources(${target} INTERFACE ${binary_dir}/${target}-components.cpp)
    endif()
endfunction()    
