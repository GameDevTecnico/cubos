# docs/CMakeLists.txt
# Cubos project documentation build configuration

find_package(Doxygen REQUIRED)
find_package(Python REQUIRED)

set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HIDE_SCOPE_NAMES YES)
set(DOXYGEN_PROJECT_NAME "Cubos")
set(DOXYGEN_PROJECT_VERSION ${CMAKE_PROJECT_VERSION})
set(DOXYGEN_PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${CMAKE_CURRENT_LIST_DIR}/conf.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

add_custom_target(
  generate-docs
  ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
  COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/css/documentation/doxygen.py" "${CMAKE_CURRENT_BINARY_DIR}/conf.py"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
