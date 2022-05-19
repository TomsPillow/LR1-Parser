cmake_minimum_required(VERSION 3.0.0)
project(LR1-Parser VERSION 0.1.0)

include(CTest)
enable_testing()
include_directories(include)
link_directories(lib)
if(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_WIN32)
    set(WINDOWS 1)
endif()

if(MACOS)
    link_libraries(yaml-cpp)
elseif(WINDOWS)
    link_libraries(libyaml-cppmdd.lib)
endif()

add_executable(LR1-Parser src/main.cpp
    src/GrammarPool.cpp
    src/Items.cpp
    src/ItemsPool.cpp
    src/Production.cpp
    src/ProductionPool.cpp
    src/SymbolPool.cpp
    src/LR1ParseTable.cpp
    src/ParseStack.cpp
    src/SyntaxTree.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include(CPack)
