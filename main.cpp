#include "src.hpp"

int main() {
    int seed;
    std::cin >> seed;
    sjtu::Tetris server(seed);
    server.run();
    return 0;
}
