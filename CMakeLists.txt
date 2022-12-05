# CMakeLists.txt
# Cubos project root build configuration

cmake_minimum_required(VERSION 3.9.0)
project(cubos VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(engine)

# Add doxygen documentation

find_package(Doxygen COMPONENTS dot)

if (Doxygen_FOUND)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HIDE_SCOPE_NAMES YES)
    set(DOXYGEN_PROJECT_NAME "CUBOS.")
    set(DOXYGEN_HTML_EXTRA_FILES ${CMAKE_SOURCE_DIR}/docs/css/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_SOURCE_DIR}/docs/css/doxygen-awesome-css/doxygen-awesome.css)
    set(DOXYGEN_HTML_HEADER ${CMAKE_SOURCE_DIR}/docs/html/header.html)
    set(DOXYGEN_HTML_FOOTER ${CMAKE_SOURCE_DIR}/docs/html/footer.html)
    set(DOXYGEN_IMAGE_PATH ${CMAKE_SOURCE_DIR}/docs/images)
    set(DOXYGEN_EXCLUDE_PATTERNS "css" "html")
    doxygen_add_docs(
        doxygen
        "${CMAKE_SOURCE_DIR}/core/include"
        "${CMAKE_SOURCE_DIR}/engine/include"
        "${CMAKE_SOURCE_DIR}/docs"
        COMMENT "Generate documentation"
    )
endif ()
