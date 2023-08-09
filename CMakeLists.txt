cmake_minimum_required (VERSION 3.8)
project ("Chip8")

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(dep/glad EXCLUDE_FROM_ALL)
add_subdirectory(dep/glfw-3.3.8 EXCLUDE_FROM_ALL)
add_subdirectory(dep/imgui-1.89.8 EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)

add_executable (
	Chip8 
	"src/Chip8.cpp"
    	"src/Window.cpp"
	"src/Main.cpp"
)

target_compile_options(Chip8 PRIVATE -Wall)

target_link_libraries(Chip8 PRIVATE glad OpenGL::GL GLFW imgui)