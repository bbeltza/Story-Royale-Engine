find_package(Python REQUIRED)
set(PYTHON_COMMAND 
        ${Python_EXECUTABLE} ${SRE_DIR}/scripts/
        CACHE STRING "Command to run the python interpreter on one of the engine's scripts")