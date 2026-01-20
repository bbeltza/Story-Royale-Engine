set(CORE_SOURCES
"audio.c"
"draw.c"
"texture.c"
"defer.c"
"thread.c"

"events.cpp"
"input.cpp"

"Containers.cpp"
"DestroyQueue.cpp"
"Querying.cpp"
"renderer.cpp"

"InitEngine.c"
"Runtime.c"
"display.cpp"
"window.c"
"window.cpp"

"HeapTracker.cpp"
PARENT_SCOPE)

add_subdirectory(drivers)