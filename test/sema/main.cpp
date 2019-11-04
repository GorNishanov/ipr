#include <iostream>
#include <ipr/io>

using namespace ipr;

int main() {
    Printer pp(std::cout);
    pp << "Hello, world\n";
}