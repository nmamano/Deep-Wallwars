#include "input_handler.hpp"

namespace GUI {

InputHandler::InputHandler(LayoutDimensions const& layout) : m_layout(layout) {}

std::tuple<ElementType, int, int> InputHandler::get_element_at_position(
    sf::Vector2i mouse_pos) const {
    // Direct port from Python elemAtCoords function
    int x = mouse_pos.x - (m_layout.margin_width + m_layout.perimeter_width);
    int y = mouse_pos.y - (m_layout.margin_height + m_layout.perimeter_height);

    if (x < 0 || x >= m_layout.board_width || y < 0 || y >= m_layout.board_height) {
        return {ElementType::NONE, 0, 0};
    }

    int row = y / m_layout.cell_plus_wall_height;
    int col = x / m_layout.cell_plus_wall_width;
    bool beyond_cell_x = (x % m_layout.cell_plus_wall_width) >= m_layout.cell_width;
    bool beyond_cell_y = (y % m_layout.cell_plus_wall_height) >= m_layout.cell_height;

    if (!beyond_cell_x && !beyond_cell_y) {
        return {ElementType::CELL, row, col};
    }
    if (beyond_cell_x && !beyond_cell_y) {
        return {ElementType::VWALL, row, col};
    }
    if (!beyond_cell_x && beyond_cell_y) {
        return {ElementType::HWALL, row, col};
    }
    return {ElementType::CORNER, row, col};
}

bool InputHandler::is_cell_reachable_in_1_action(Board const& board, Player player,
                                                 Cell target) const {
    return board.distance(board.position(player), target) == 1;
}

bool InputHandler::is_cell_reachable_in_2_actions(Board const& board, Player player,
                                                  Cell target) const {
    return board.distance(board.position(player), target) == 2;
}

InputHandler::MouseAction InputHandler::handle_mouse_click(sf::Vector2i mouse_pos,
                                                           Board const& board,
                                                           Player current_player) const {
    auto [element_type, row, col] = get_element_at_position(mouse_pos);

    switch (element_type) {
        case ElementType::CELL: {
            Cell target{col, row};  // Note: Python uses (row, col), C++ Board uses (col, row)

            // Check if target is reachable (similar to Python moveAction logic)
            if (is_cell_reachable_in_1_action(board, current_player, target) ||
                is_cell_reachable_in_2_actions(board, current_player, target)) {
                return {InputHandler::MouseAction::MOVE_TO_CELL, target, {}};
            }
            break;
        }

        case ElementType::VWALL: {
            // Check bounds
            if (col < m_layout.board_cols - 1 && row < m_layout.board_rows) {
                Wall target{Cell{col, row}, Wall::Right};

                // Check if wall placement is legal (not blocked and not forbidden)
                if (!board.is_blocked(target)) {
                    auto legal_walls = board.legal_walls();
                    for (auto const& wall : legal_walls) {
                        if (wall == target) {
                            return {InputHandler::MouseAction::PLACE_WALL, {}, target};
                        }
                    }
                }
            }
            break;
        }

        case ElementType::HWALL: {
            // Check bounds
            if (col < m_layout.board_cols && row < m_layout.board_rows - 1) {
                Wall target{Cell{col, row}, Wall::Down};

                // Check if wall placement is legal (not blocked and not forbidden)
                if (!board.is_blocked(target)) {
                    auto legal_walls = board.legal_walls();
                    for (auto const& wall : legal_walls) {
                        if (wall == target) {
                            return {InputHandler::MouseAction::PLACE_WALL, {}, target};
                        }
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    return {InputHandler::MouseAction::NONE, {}, {}};
}

std::optional<Direction> InputHandler::handle_key_press(sf::Keyboard::Key key) const {
    // Map keyboard input to directions (from Python keyboard handling)
    switch (key) {
        case sf::Keyboard::Left:
        case sf::Keyboard::A:
            return Direction::Left;
        case sf::Keyboard::Right:
        case sf::Keyboard::D:
            return Direction::Right;
        case sf::Keyboard::Up:
        case sf::Keyboard::W:
            return Direction::Up;
        case sf::Keyboard::Down:
        case sf::Keyboard::S:
            return Direction::Down;
        default:
            return std::nullopt;
    }
}

}  // namespace GUI
