#include <functional>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

template <size_t H, size_t W>
class Maze {
   private:
    bool _field[H][W]{};
    pair<size_t, size_t> _start_pos;
    pair<size_t, size_t> _dest_pos;

   public:
    static const char space_char = ' ';
    static const char obstacle_char = '#';
    static const char start_pos_char = '*';
    static const char dest_pos_char = '.';

    Maze(const char (&input_field)[H][W + 1]) {
        bool found_start_pos = false;
        bool found_dest_pos = false;
        for (size_t i = 0; i < H; ++i) {
            for (size_t j = 0; j < W; ++j) {
                char cell = input_field[i][j];
                switch (cell) {
                    case start_pos_char:
                        if (found_start_pos) {
                            cout << "error: Multiple starting position "
                                    "characters are not supported"
                                 << endl;
                            exit(1);
                        }
                        found_start_pos = true;
                        _start_pos = {i, j};
                        break;
                    case dest_pos_char:
                        if (found_dest_pos) {
                            cout << "error: Multiple destination characters "
                                    "are not supported"
                                 << endl;
                            exit(1);
                        }
                        found_dest_pos = true;
                        _dest_pos = {i, j};
                        break;
                    case obstacle_char:
                        _field[i][j] = true;
                        break;
                    case space_char:
                        break;
                    default:
                        cout << "error: Wrong character in the input field: '"
                             << cell << "'" << endl;
                        exit(1);
                }
            }
        }
        if (!found_start_pos) {
            cout << "error: No starting position character found" << endl;
            exit(1);
        }
        if (!found_dest_pos) {
            cout << "error: No destination character found" << endl;
            exit(1);
        }
    }
    ~Maze() {}

    inline size_t width() const { return W; }
    inline size_t height() const { return H; }
    const pair<size_t, size_t> start_pos() const { return _start_pos; }
    const pair<size_t, size_t> dest_pos() const { return _dest_pos; }
    bool is_obstacle(pair<size_t, size_t> pos) const {
        return _field[pos.first][pos.second];
    }
};

template <class T1, class T2>
ostream& operator<<(ostream& os, const pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template<size_t H, size_t W>
ostream& operator<<(ostream& os, const Maze<H, W>& maze) {
    for (size_t i = 0; i < maze.width() + 2; ++i) {
        cout << "▄";
    }
    cout << endl;
    for (size_t i = 0; i < maze.height(); ++i) {
        cout << "█";
        for (size_t j = 0; j < maze.width(); ++j) {
            pair<size_t, size_t> pos = {i, j};
            if (maze.start_pos() == pos)
                os << maze.start_pos_char;
            else if (maze.dest_pos() == pos)
                os << maze.dest_pos_char;
            else if (maze.is_obstacle(pos))
                os << maze.obstacle_char;
            else
                os << maze.space_char;
        }
        cout << "█" << endl;
    }
    for (size_t i = 0; i < maze.width() + 2; ++i) {
        cout << "▀";
    }
    return os;
}

int main() {
    const size_t height = 4;
    const size_t width = 5;
    char input_field[height][width + 1] = {
        "*   #",
        "### #",
        "  # #",
        "  # .",
    };

    Maze<height, width> maze{input_field};
    cout << maze << endl << maze.start_pos() << endl << maze.dest_pos();

    return 0;
}
