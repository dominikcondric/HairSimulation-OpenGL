# GLAD
add_subdirectory(GLAD)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Don't install" FORCE)
add_subdirectory(GLFW)
