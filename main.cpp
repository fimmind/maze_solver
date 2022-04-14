// Prelude {{{1
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>
using namespace std;

// Maze class {{{1
template <size_t H, size_t W>
class Maze {
   private:
    typedef pair<size_t, size_t> cell_t;

    bool _field[H][W]{};
    cell_t _start_pos;
    cell_t _dest_pos;

    // Восстановление пути из графа родителей
    vector<cell_t> build_path(map<cell_t, cell_t> parents) {
        vector<cell_t> res({_dest_pos});
        cell_t cur_node = _dest_pos;
        while (cur_node != _start_pos) {
            cur_node = parents[cur_node];
            res.push_back(cur_node);
        }
        reverse(res.begin(), res.end());
        return res;
    }

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
    const cell_t start_pos() const { return _start_pos; }
    const cell_t dest_pos() const { return _dest_pos; }
    bool is_obstacle(cell_t pos) const { return _field[pos.first][pos.second]; }

    bool contains(cell_t cell) {
        return 0 <= cell.first && cell.first < height() && 0 <= cell.second &&
               cell.second < width();
    }

    set<cell_t> neighbors(cell_t cell) {
        set<cell_t> res = {};
        cell_t increments[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        for (cell_t inc : increments) {
            cell_t neighbor = {cell.first + inc.first,
                               cell.second + inc.second};
            if (contains(neighbor) && !is_obstacle(neighbor)) {
                res.insert(neighbor);
            }
        }
        return res;
    }

    // Поиск пути в ширину
    vector<cell_t> bfs_path() {
        set<cell_t> visited({_start_pos});
        queue<cell_t> search_queue({_start_pos});
        map<cell_t, cell_t> parents;
        while (!search_queue.empty()) {
            cell_t next = search_queue.front();
            if (next == _dest_pos) {
                return build_path(parents);
            }
            search_queue.pop();
            for (cell_t child : neighbors(next)) {
                if (!visited.contains(child)) {
                    parents.insert({child, next});
                    visited.insert(child);
                    search_queue.push(child);
                }
            }
        }
        return {};
    }

    // Поиск пути в глубину
    vector<cell_t> dfs_path() {
        auto visited = set<cell_t>({_start_pos});
        auto search_stack = stack<cell_t>({_start_pos});
        map<cell_t, cell_t> parents;
        while (!search_stack.empty()) {
            cell_t next = search_stack.top();
            if (next == _dest_pos) {
                return build_path(parents);
            }
            search_stack.pop();
            for (cell_t child : neighbors(next)) {
                if (!visited.contains(child)) {
                    parents.insert({child, next});
                    visited.insert(child);
                    search_stack.push(child);
                }
            }
        }
        return {};
    }
};

// operator<< {{{1
template <class T1, class T2>
ostream& operator<<(ostream& os, const pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template <class T>
ostream& operator<<(ostream& os, vector<T> v) {
    os << " ";
    for (auto elem : v) {
        os << elem << " ";
    }
    return os;
}

template <class T>
ostream& operator<<(ostream& os, set<T> s) {
    os << "{ ";
    for (auto elem : s) {
        os << elem << " ";
    }
    os << "}";
    return os;
}

template <size_t H, size_t W>
ostream& operator<<(ostream& os, const Maze<H, W>& maze) {
    cout << "  ";
    for (size_t i = 0; i < maze.width() + 2; ++i) {
        cout << "▄";
    }
    cout << endl;
    for (size_t i = 0; i < maze.height(); ++i) {
        cout << "  █";
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
    cout << "  ";
    for (size_t i = 0; i < maze.width() + 2; ++i) {
        cout << "▀";
    }
    return os;
}

// main {{{1
int main() {
    const size_t height = 5;
    const size_t width = 6;
    char input_field[height][width + 1] = {"*     ", " #### ", "    # ",
                                           " ## #.", "  #   "};

    Maze<height, width> maze{input_field};
    cout << "Поле: \n" << maze << endl;

    cout << "Путь при поиске в ширину:" << endl
         << "  " << maze.bfs_path() << endl;

    cout << "Путь при поиске в глубину:" << endl
         << "  " << maze.dfs_path() << endl;

    return 0;
}

// {{{1 vim:  fdm=marker
