set(PROJECT_NAME )

set(SDK_ROOT )

set(PROJ_DIR )

set(SRC_FILES )

set(INC_FOLDERS )

set(CFLAGS )

set(CXXFLAGS )

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
