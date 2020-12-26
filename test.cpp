#include <iostream>
#include <string>
#include "inpuft.hpp"

int main(int argc, char** argv) {
    Inpuft get(false);

    std::cout << "\nData: " << get.data << "\n";
    std::cout << "Cursor position(x) : " << get.cursor.x << "\n";
    
    return 0;
}
