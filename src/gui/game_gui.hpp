#pragma once

#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/futures/Future.h>

#include <SFML/Graphics.hpp>
#include <memory>

#include "../gamestate.hpp"
#include "../mcts.hpp"
#include "../play.hpp"
#include "board_renderer.hpp"
#include "gui_constants.hpp"
#include "gui_utils.hpp"
#include "input_handler.hpp"

namespace GUI {

class GameGUI {
public:
    GameGUI(int window_width = GUI::DEFAULT_WINDOW_WIDTH,
            int window_height = GUI::DEFAULT_WINDOW_HEIGHT,
            int board_cols = GUI::DEFAULT_BOARD_COLS, int board_rows = GUI::DEFAULT_BOARD_ROWS);

    // Static helper to get player choice before GUI creation
    static bool ask_human_goes_first();

    // Static utility to check X11 display status
    static bool check_display_connection();

    // Main function to run interactive game with GUI
    GameRecorder run_interactive_game(Board board, EvaluationFunction ai_model,
                                      InteractivePlayOptions opts, bool human_goes_first,
                                      folly::CPUThreadPoolExecutor& thread_pool);

private:
    enum class GameState {
        HumanTurn,
        AITurn,
        WaitingForAI,
        GameOver,
    };

    sf::RenderWindow m_window;
    LayoutDimensions m_layout;
    std::unique_ptr<BoardRenderer> m_renderer;
    InputHandler m_input_handler;
    folly::SemiFuture<std::optional<Move>> m_ai_move = std::nullopt;
    folly::CPUThreadPoolExecutor* m_thread_pool = nullptr;  // Reference to thread pool for AI work

    // Game state tracking
    Player m_human_player;
    Player m_ai_player;
    int m_actions_left = 2;
    GameState m_game_state = GameState::HumanTurn;
    float m_score_for_human = 0;
    std::vector<Action> m_human_actions;  // Track human actions for move recording

    // Event processing
    bool process_events(Board& board, MCTS& mcts, GameRecorder& recorder);
    void handle_mouse_click(sf::Vector2i mouse_pos, Board& board, MCTS& mcts,
                            GameRecorder& recorder);
    void handle_key_press(sf::Keyboard::Key key, Board& board, MCTS& mcts, GameRecorder& recorder);

    // AI computation helpers
    void process_ai_turn(MCTS& mcts, Board& board, int samples, GameRecorder& recorder);

    // Game logic helpers
    void advance_action();
    void check_game_over(Board const& board, GameRecorder& recorder);

    // Rendering
    void render(Board const& board, MCTS const& mcts, int total_samples);
    ElementType m_highlight_type = ElementType::NONE;
    int m_highlight_row = -1;
    int m_highlight_col = -1;
};

// Standalone GUI function for interactive play
GameRecorder interactive_play_gui(Board board, InteractivePlayOptions opts,
                                  folly::CPUThreadPoolExecutor& thread_pool);

}  // namespace GUI
