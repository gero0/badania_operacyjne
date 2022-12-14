#include <cstdlib>
#include <sstream>
#include <iostream>
#include <cstdio>

int main(int argc, char** argv) {
    char* data_path = argv[1];
    char* params = argv[2];
    std::stringstream ss(params);

    std::string token;
    getline(ss, token, ' ');

    std::string exec = "./" + token;
    std::string cmd_params("");

    while(getline(ss, token, ' ')) {
        cmd_params = cmd_params + token + " ";
    }

    std::string command = exec + " " + data_path + " " + cmd_params;
    std::cout << "TEXT " << command;
    system(command.c_str());
} 
