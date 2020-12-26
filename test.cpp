#include <iostream>
#include <string>
#include "inpuft.hpp"

int main(int argc, char** argv) {
    Inpuft input;

   
    input.InitFile((std::string)getenv("PWD") + "/history");
       
     input.Init(false); 

    std::cout << input.data << " " << input.current_line << "\n";
    
    return 0;
}
