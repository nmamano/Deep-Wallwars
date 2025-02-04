#pragma once

#include <array>
#include <compare>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <set>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

enum class Direction { Right, Down, Left, Up };

constexpr std::array<Direction, 4> kDirections = {Direction::Right, Direction::Down,
                                                  Direction::Left, Direction::Up};

[[nodiscard]] Direction flip_horizontal(Direction dir);

enum class Player { Red, Blue };

Player other_player(Player player);

struct Cell {
    int column;
    int row;

    std::strong_ordering operator<=>(Cell const& other) const = default;
    bool operator==(Cell const& other) const = default;

    [[nodiscard]] Cell step(Direction direction) const;
};

struct Wall {
    Cell cell;
    enum Type { Right, Down } type;

    Wall(Cell cell, Type type);
    Wall(Cell cell, Direction direction);

    Direction direction() const;

    std::strong_ordering operator<=>(Wall const& other) const = default;
    bool operator==(Wall const& other) const = default;
};

namespace std {
template <>
struct hash<Cell> {
    std::uint64_t operator()(Cell cell) const;
};

template <>
struct hash<Wall> {
    std::uint64_t operator()(Wall wall) const;
};
}  // namespace std

using Action = std::variant<Direction, Wall>;

struct Move {
    Action first;
    Action second;

    std::string standard_notation(Cell start) const;
};

struct Turn {
    Player player;
    enum { First, Second } action;

    bool operator==(Turn const& other) const = default;

    [[nodiscard]] Turn next() const;
    [[nodiscard]] Turn prev() const;
};

std::ostream& operator<<(std::ostream& out, Direction dir);
std::ostream& operator<<(std::ostream& out, Player player);
std::ostream& operator<<(std::ostream& out, Cell cell);
std::ostream& operator<<(std::ostream& out, Wall all);
std::ostream& operator<<(std::ostream& out, Action const& action);
std::ostream& operator<<(std::ostream& out, Move const& move);
std::ostream& operator<<(std::ostream& out, Turn turn);

std::istream& operator>>(std::istream& out, Cell& cell);
std::istream& operator>>(std::istream& out, Wall& wall);
std::istream& operator>>(std::istream& out, Action& cell);

class Board {
public:
    Board(int columns, int rows);
    Board(int columns, int rows, Cell red_start, Cell red_goal);
    Board(int columns, int rows, Cell red_start, Cell red_goal, Cell blue_start, Cell blue_goal);

    bool is_blocked(Wall wall) const;

    std::vector<Direction> legal_directions(Player player) const;
    std::vector<Wall> legal_walls() const;
    std::vector<Action> legal_actions(Player player) const;

    void take_step(Player player, Direction direction);
    void place_wall(Player player, Wall wall);

    void do_action(Player player, Action action);

    std::optional<Player> winner() const;
    double score_for(Player player) const;

    Cell position(Player player) const;
    Cell goal(Player player) const;

    int distance(Cell start, Cell target) const;
    void fill_relative_distances(Cell start, std::span<float> dists) const;

    std::vector<std::array<bool, 4>> blocked_directions() const;
    void fill_relative_distances(Cell start, std::span<float> dists,
                                 std::vector<std::array<bool, 4>> const& blocked_dirs) const;

    int columns() const;
    int rows() const;

    Cell cell_at_index(int i) const;
    int index_from_cell(Cell cell) const;

    std::uint64_t hash_from_pov(Player player, bool flip_horizontal = false,
                                bool hash_wall_color = false) const;
    [[nodiscard]] Cell flip_horizontal(Cell cell) const;
    [[nodiscard]] Wall flip_horizontal(Wall all) const;

private:
    struct State {
        bool has_red_player : 1 = false;
        bool has_blue_player : 1 = false;
        bool has_red_right_wall : 1 = false;
        bool has_red_down_wall : 1 = false;
        bool has_blue_right_wall : 1 = false;
        bool has_blue_down_wall : 1 = false;
        bool has_red_goal : 1 = false;
        bool has_blue_goal : 1 = false;
    };

    struct {
        Cell position;
        Cell goal;
    } m_red, m_blue;

    int m_columns;
    int m_rows;

    std::vector<State> m_board;

    State& state_at(Cell cell);
    State state_at(Cell cell) const;

    std::pair<bool, int> find_bridges(Cell start, Cell target, int level, std::vector<int>& levels,
                                      std::set<Wall>& bridges) const;
};
