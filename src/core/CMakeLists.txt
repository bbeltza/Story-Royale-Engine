SRE_BUILD_SRC(sre_core
	"hints.c"

	"audio.c"
	"texture.c"
	"defer.c"
	
	"events.cpp"
	"input.cpp"
	
	"objects.cpp"
	"renderer.cpp"
	
	"init.c"
	"runtime.c"
	"runtime.cpp"
	"display.cpp"
	"window.c"
	"window.cpp"
)

add_subdirectory(drivers)
target_link_libraries(sre_core PRIVATE sre_drivers)