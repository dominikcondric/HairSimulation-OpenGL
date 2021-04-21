add_library(Glad STATIC 
	src/glad.c)

target_include_directories(Glad PUBLIC 
	include/
)

