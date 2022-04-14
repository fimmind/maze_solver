// Prelude {{{1
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <vector>
using namespace std;

// DirectedGraph class {{{1
template <class T>
class DirectedGraph {
   private:
    map<T, set<T>> nodes{};
    size_t nodes_count = 0;

   public:
    // Добавление в граф элемента, не связанного с другими элементами
    void add_node(const T node) {
        if (!nodes.contains(node)) {
            nodes.insert({node, set<T>()});
            ++nodes_count;
        }
    }

    // Добавление в граф двунаправленного ребра
    void add_double_edge(const T a, const T b) {
        add_node(a);
        add_node(b);
        nodes[a].insert(b);
        nodes[b].insert(a);
    }

    // Добавление в граф однонаправленного ребра
    void add_edge(const T from, const T to) {
        add_node(from);
        add_node(to);
        nodes[from].insert(to);
    }

    // Getters
    const set<T>& get_children(const T node) { return nodes.at(node); }
    const map<T, set<T>>& as_map() { return nodes; }
    const size_t size() { return nodes_count; }
};

// Maze class {{{1
template <size_t H, size_t W>
class Maze {
   private:
    typedef pair<size_t, size_t> cell_t;

    bool _field[H][W]{};
    cell_t _start_pos;
    cell_t _dest_pos;

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

    DirectedGraph<cell_t> as_graph() {
        DirectedGraph<cell_t> graph;
        set<cell_t> visited;
        queue<cell_t> cells_queue;
        cells_queue.push(_start_pos);
        while (!cells_queue.empty()) {
            cell_t next = cells_queue.front();
            cells_queue.pop();
            for (cell_t neighbor : neighbors(next)) {
                graph.add_edge(next, neighbor);
                if (!visited.count(neighbor)) {
                    visited.insert(neighbor);
                    cells_queue.push(neighbor);
                }
            }
        }
        return graph;
    }
};

// operator<< {{{1
template <class T1, class T2>
ostream& operator<<(ostream& os, const pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
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
    const size_t height = 4;
    const size_t width = 5;
    char input_field[height][width + 1] = {
        "*   #",
        " ## #",
        "   ##",
        " #  .",
    };

    Maze<height, width> maze{input_field};
    cout  << "Поле: \n" << maze << "\nНачальная позиция: " <<  maze.start_pos() << "\nФинишная позиция: " << maze.dest_pos() << endl;

    auto graph = maze.as_graph();
    cout << "Граф:" << endl;
    for (auto& i : graph.as_map()) {
        cout << "  " << i.first << " -> " << i.second << endl;
    }

    return 0;
}

// {{{1 vim:  fdm=marker
