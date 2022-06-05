// Prelude {{{1
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

// Supplementary function  {{{1
template <class T>
vector<T>& operator+=(vector<T>& v1, const vector<T>& v2) {
    v1.reserve(v1.size() + v2.size());
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

template <class T>
T average(vector<T>& v) {
    return reduce(v.begin(), v.end()) / v.size();
}

// Maze class {{{1
template <size_t H, size_t W>
class Maze {
   public:
    typedef pair<size_t, size_t> cell_t;

   private:
    bool _field[H][W]{};
    cell_t _start_pos;
    cell_t _dest_pos;

    // Восстановление пути из графа родителей
    vector<cell_t> build_reversed_path(map<cell_t, cell_t> parents,
                                       cell_t start_pos, cell_t dest_pos) {
        vector<cell_t> res({dest_pos});
        cell_t cur_node = dest_pos;
        while (cur_node != start_pos) {
            cur_node = parents[cur_node];
            res.push_back(cur_node);
        }
        return res;
    }

    vector<cell_t> build_path(map<cell_t, cell_t> parents, cell_t start_pos,
                              cell_t dest_pos) {
        auto res = build_reversed_path(parents, start_pos, dest_pos);
        reverse(res.begin(), res.end());
        return res;
    }

    // Один шаг поиска в ширину
    void bfs_step(set<cell_t>* visited_forward,
                  const set<cell_t>* visited_backward,
                  queue<cell_t>* search_queue, map<cell_t, cell_t>* parents,
                  bool* is_done, cell_t* intersection) {
        if (search_queue->empty() || *is_done) return;
        cell_t next = search_queue->front();
        if (visited_backward->contains(next)) {
            *is_done = true;
            if (intersection) *intersection = next;
            return;
        }
        search_queue->pop();

        for (cell_t child : neighbors(next)) {
            if (!visited_forward->contains(child)) {
                parents->insert({child, next});
                visited_forward->insert(child);
                search_queue->push(child);
            }
        }
    }

   public:
    static const char space_char = ' ';
    static const char obstacle_char = '#';
    static const char start_pos_char = '@';
    static const char dest_pos_char = '*';

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

    Maze(bool field[H][W], cell_t start_pos, cell_t dest_pos) {
        _field = field;
        _start_pos = start_pos;
        _dest_pos = dest_pos;
    }

    Maze() { generate_random(); }

    void generate_random() {
        random_device r;
        default_random_engine generator(r());
        uniform_int_distribution<unsigned int> bool_distribution(0, 3);
        for (auto i = 0; i < H; ++i) {
            for (auto j = 0; j < W; ++j) {
                _field[i][j] = !bool_distribution(generator);
            }
        }

        uniform_int_distribution<size_t> height_distribution(0, H - 1);
        uniform_int_distribution<size_t> width_distribution(0, W - 1);
        auto random_position = [&]() -> cell_t {
            return {height_distribution(generator),
                    width_distribution(generator)};
        };

        _start_pos = random_position();
        _dest_pos = random_position();
        _field[_start_pos.first][_start_pos.second] = 0;
        _field[_dest_pos.first][_dest_pos.second] = 0;
    }

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
        bool is_done = false;

        const set<cell_t> destination_set({_dest_pos});

        while (!search_queue.empty()) {
            bfs_step(&visited, &destination_set, &search_queue, &parents,
                     &is_done, nullptr);
            if (is_done) {
                return build_path(parents, _start_pos, _dest_pos);
            }
        }
        return {};
    }

    // Двусторонний поиск в ширину
    vector<cell_t> bi_bfs_path() {
        set<cell_t> visited_forward({_start_pos});
        set<cell_t> visited_backward({_dest_pos});
        queue<cell_t> forward_search_queue({_start_pos});
        queue<cell_t> backward_search_queue({_dest_pos});
        map<cell_t, cell_t> parents;
        map<cell_t, cell_t> children;
        bool is_done = false;
        cell_t intersection;

        while (!forward_search_queue.empty() &&
               !backward_search_queue.empty()) {
            bfs_step(&visited_forward, &visited_backward, &forward_search_queue,
                     &parents, &is_done, &intersection);
            bfs_step(&visited_backward, &visited_forward,
                     &backward_search_queue, &children, &is_done,
                     &intersection);
            if (is_done) {
                vector<cell_t> res_path =
                    build_path(parents, _start_pos, intersection);
                res_path.pop_back();
                res_path +=
                    build_reversed_path(children, _dest_pos, intersection);
                return res_path;
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
                return build_path(parents, _start_pos, _dest_pos);
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

    void print_with_path(set<cell_t> maze_path) {
        set<cell_t> path_cells(maze_path.begin(), maze_path.end());
        cout << "  ";
        for (size_t i = 0; i < width() + 2; ++i) {
            cout << "─";
        }
        cout << endl;
        for (size_t i = 0; i < height(); ++i) {
            cout << "  │";
            for (size_t j = 0; j < width(); ++j) {
                pair<size_t, size_t> pos = {i, j};
                if (start_pos() == pos)
                    cout << start_pos_char;
                else if (dest_pos() == pos)
                    cout << dest_pos_char;
                else if (is_obstacle(pos))
                    cout << obstacle_char;
                else if (path_cells.contains(pos))
                    cout << "⋅";
                else
                    cout << space_char;
            }
            cout << "│" << endl;
        }
        cout << "  ";
        for (size_t i = 0; i < width() + 2; ++i) {
            cout << "─";
        }
    }

    void print() { print_with_path({}); }
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

// Timing {{{1
template <class F>
microseconds timed(F f) {
    auto start = high_resolution_clock::now();
    f();
    auto stop = high_resolution_clock::now();
    return duration_cast<microseconds>(stop - start);
}

// main {{{1
int main() {
    const size_t height = 60;
    const size_t width = 120;
    const size_t runs = 10000;

    cout << "Running a " << runs << " runs test for a " << height << "x"
         << width << " map...\n";

    vector<microseconds> bfs_times = {};
    vector<microseconds> dfs_times = {};
    vector<microseconds> bi_bfs_times = {};

    Maze<height, width> maze;
    for (auto i = 0; i < runs; ++i) {
        maze.generate_random();
        bfs_times.push_back(timed([&]() { maze.bfs_path(); }));
        dfs_times.push_back(timed([&]() { maze.dfs_path(); }));
        bi_bfs_times.push_back(timed([&]() { maze.bi_bfs_path(); }));
    }

    cout << "Averate time:\n   BFS  : " << average(bfs_times)
         << "\n   DFS  : " << average(dfs_times)
         << "\n  biBFS : " << average(bi_bfs_times) << "\n";

    return 0;
}

// {{{1 vim:  fdm=marker
