#ifndef T3_ACMOJ2455_TETRIS_HPP
#define T3_ACMOJ2455_TETRIS_HPP

#include <iostream>
#include <vector>
#include <string>

namespace sjtu {

    class XorShift {
    private:
        unsigned int x, y, z, w;

        void xor_shift32() {
            unsigned int t = x ^ (x << 11);
            x = y;
            y = z;
            z = w;
            w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        }

    public:
        explicit XorShift(unsigned int seed) {
            x = seed;
            y = seed;
            z = seed;
            w = seed;
            xor_shift32();
        }

        unsigned int next() {
            xor_shift32();
            return w;
        }
    };

    struct Tetris {
        struct Tetromino {
            struct Shape {
                std::vector<std::vector<int>> shape;
                int rotateX, rotateY;
            };
            constexpr static char shapeAlphas[7] = {'I', 'T', 'O', 'S', 'Z', 'J', 'L'};
            const static Shape rotateShapes[7][4];
            int x, y;
            int index;
            int rotation;
        };

        const static int WIDTH = 10;
        const static int HEIGHT = 20;

        XorShift rand;
        Tetromino cur;
        std::vector<std::vector<int>> field;
        int score;

        Tetris(int seed);
        bool new_tetris();
        bool dropping();
        bool move_tetris(const std::string &operation);
        void place_tetris();
        void clear_lines();
        void print_field();
        bool game_over();
        bool check_collision(const Tetromino &next);
        void run();
    };

    const Tetris::Tetromino::Shape Tetris::Tetromino::rotateShapes[7][4] = {
            {
                    {{{1, 1, 1, 1}},         1, -1},
                    {{{1},    {1},    {1}, {1}}, -1, 2},
                    {{{1, 1, 1, 1}},         2, -2},
                    {{{1},    {1},    {1}, {1}}, -2, 1},
            }, // I
            {
                    {{{1, 1, 1}, {0, 1, 0}}, 0, -1},
                    {{{0, 1}, {1, 1}, {0, 1}},   0, 0},
                    {{{0, 1, 0}, {1, 1, 1}}, 1, 0},
                    {{{1, 0}, {1, 1}, {1, 0}},   -1, 1},
            }, // T
            {
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0},
                    {{{1, 1},    {1, 1}},    0, 0}
            }, // O
            {
                    {{{0, 1, 1}, {1, 1, 0}}, 0, -1},
                    {{{1, 0}, {1, 1}, {0, 1}},   0, 0},
                    {{{0, 1, 1}, {1, 1, 0}}, 1, 0},
                    {{{1, 0}, {1, 1}, {0, 1}},   -1, 1}
            }, // S
            {
                    {{{1, 1, 0}, {0, 1, 1}}, 0, -1},
                    {{{0, 1}, {1, 1}, {1, 0}},   0, 0},
                    {{{1, 1, 0}, {0, 1, 1}}, 1, 0},
                    {{{0, 1}, {1, 1}, {1, 0}},   -1, 1}
            }, // Z
            {
                    {{{1, 0, 0}, {1, 1, 1}}, 1, 0},
                    {{{1, 1},    {1, 0}, {1, 0}}, -1, 1},
                    {{{1, 1, 1}, {0, 0, 1}}, 0, -1},
                    {{{0, 1},    {0, 1}, {1, 1}},   0, 0}
            }, // J
            {
                    {{{0, 0, 1}, {1, 1, 1}}, 1, 0},
                    {{{1, 0},    {1, 0}, {1, 1}},   -1, 1},
                    {{{1, 1, 1}, {1, 0, 0}}, 0, -1},
                    {{{1, 1},    {0, 1}, {0, 1}},   0, 0}
            } // L
    };

    Tetris::Tetris(int seed) : rand(seed), score(0) {
        field.resize(HEIGHT, std::vector<int>(WIDTH, 0));
    }

    bool Tetris::new_tetris() {
        cur.index = rand.next() % 7;
        cur.rotation = 0;
        cur.x = (WIDTH >> 1) - (Tetromino::rotateShapes[cur.index][0].shape[0].size() >> 1);
        cur.y = 0;
        
        std::cout << Tetromino::shapeAlphas[cur.index] << std::endl;
        
        return !check_collision(cur);
    }

    bool Tetris::dropping() {
        Tetromino next = cur;
        next.y++;
        if (check_collision(next)) {
            return false;
        }
        cur = next;
        return true;
    }

    bool Tetris::move_tetris(const std::string &operation) {
        if (operation == "e") {
            return true;
        }
        
        if (operation == "space") {
            while (dropping()) {}
            return true;
        }
        
        Tetromino next = cur;
        
        if (operation == "a") {
            next.x--;
        } else if (operation == "d") {
            next.x++;
        } else if (operation == "s") {
            next.y++;
        } else if (operation == "w") {
            const Tetromino::Shape &currentShape = Tetromino::rotateShapes[cur.index][cur.rotation];
            next.rotation = (cur.rotation + 1) % 4;
            next.x += currentShape.rotateX;
            next.y += currentShape.rotateY;
        } else {
            return false;
        }
        
        if (check_collision(next)) {
            return false;
        }
        
        cur = next;
        return true;
    }

    bool Tetris::check_collision(const Tetromino &next) {
        const Tetromino::Shape &shape = Tetromino::rotateShapes[next.index][next.rotation];
        
        for (int i = 0; i < (int)shape.shape.size(); i++) {
            for (int j = 0; j < (int)shape.shape[i].size(); j++) {
                if (shape.shape[i][j]) {
                    int ny = next.y + i;
                    int nx = next.x + j;
                    
                    if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) {
                        return true;
                    }
                    
                    if (field[ny][nx]) {
                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    void Tetris::place_tetris() {
        const Tetromino::Shape &shape = Tetromino::rotateShapes[cur.index][cur.rotation];
        
        for (int i = 0; i < (int)shape.shape.size(); i++) {
            for (int j = 0; j < (int)shape.shape[i].size(); j++) {
                if (shape.shape[i][j]) {
                    field[cur.y + i][cur.x + j] = 1;
                }
            }
        }
    }

    void Tetris::clear_lines() {
        int linesCleared = 0;
        
        for (int i = HEIGHT - 1; i >= 0; i--) {
            bool full = true;
            for (int j = 0; j < WIDTH; j++) {
                if (!field[i][j]) {
                    full = false;
                    break;
                }
            }
            
            if (full) {
                linesCleared++;
                field.erase(field.begin() + i);
                field.insert(field.begin(), std::vector<int>(WIDTH, 0));
                i++;
            }
        }
        
        if (linesCleared == 1) score += 100;
        else if (linesCleared == 2) score += 300;
        else if (linesCleared == 3) score += 600;
        else if (linesCleared == 4) score += 1000;
    }

    void Tetris::print_field() {
        std::vector<std::vector<int>> display = field;
        
        const Tetromino::Shape &shape = Tetromino::rotateShapes[cur.index][cur.rotation];
        for (int i = 0; i < (int)shape.shape.size(); i++) {
            for (int j = 0; j < (int)shape.shape[i].size(); j++) {
                if (shape.shape[i][j]) {
                    int ny = cur.y + i;
                    int nx = cur.x + j;
                    if (ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH) {
                        display[ny][nx] = 1;
                    }
                }
            }
        }
        
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                std::cout << display[i][j];
                if (j < WIDTH - 1) std::cout << " ";
            }
            std::cout << std::endl;
        }
    }

    bool Tetris::game_over() {
        for (int j = 0; j < WIDTH; j++) {
            if (field[0][j]) {
                return true;
            }
        }
        return false;
    }

    void Tetris::run() {
        new_tetris();
        
        while (true) {
            print_field();
            
            std::string operation;
            while (true) {
                std::cin >> operation;
                if (operation == "w" || operation == "a" || operation == "d" || 
                    operation == "s" || operation == "e" || operation == "space" || 
                    operation == "esc") {
                    break;
                }
            }
            
            if (operation == "esc") {
                std::cout << "See you!" << std::endl;
                std::cout << "Your score: " << score << std::endl;
                return;
            }
            
            bool success = move_tetris(operation);
            if (success) {
                std::cout << "Successful operation!" << std::endl;
            } else {
                std::cout << "Failed operation!" << std::endl;
            }
            
            if (!dropping()) {
                place_tetris();
                clear_lines();
                
                if (game_over()) {
                    print_field();
                    std::cout << "Game Over!" << std::endl;
                    std::cout << "Your score: " << score << std::endl;
                    return;
                }
                
                if (!new_tetris()) {
                    std::cout << "Game Over!" << std::endl;
                    std::cout << "Your score: " << score << std::endl;
                    return;
                }
            }
        }
    }

}

#endif //T3_ACMOJ2455_TETRIS_HPP
