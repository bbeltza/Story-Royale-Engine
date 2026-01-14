set(CORE_SOURCES
"draw.c"
"texture.c"
"defer.c"
"events.cpp"
"thread.c"

"Audio.cpp"
"Containers.cpp"
"DestroyQueue.cpp"
"input.cpp"
"Querying.cpp"
"renderer.cpp"

"InitEngine.c"
"Runtime.c"
"Window.c"

"HeapTracker.cpp"
PARENT_SCOPE)

add_subdirectory(drivers)