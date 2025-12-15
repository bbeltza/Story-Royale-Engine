add_library(sre_os STATIC "os.c" "output.c" "sync.c" "resources.c" "threads.c")
target_link_libraries(sre_os PRIVATE "ntdll.lib")