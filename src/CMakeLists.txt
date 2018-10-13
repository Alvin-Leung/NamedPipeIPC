cmake_minimum_required (VERSION 3.8)

add_executable (Client "Client.cpp" "ClientNamedPipe.cpp" "ClientNamedPipe.h")
add_executable (Server "Server.cpp" "ServerNamedPipe.cpp" "ServerNamedPipe.h")