#include <iostream>
#include "cmdline.hpp" // parse_command
#include "app.hpp" // handle_expr_line

int main(int argc, char *argv[])
{
    std::vector<Commands::Command> cmd;
    for(int i = 1; i < argc; i++){
        cmd.push_back(parse_command(argv[i]));
    }

    std::string in;
    while(!std::cin.eof()){
        getline(std::cin,in);
        try{
            handle_expr_line(std::cout, in, cmd);
        }catch(...){
            std::cout << "! exception" << std::endl;
        }
    }
}
