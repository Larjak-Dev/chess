#pragma once
#include "app/AppResources.hpp"
#include "app/slides/Slide.hpp"
#include "app/widgets/Scene.hpp"
#include "chess/chess.hpp"
#include "core/Units.hpp"
namespace phys::app
{

class GameSlide : public Slide
{

  public:
    GameSlide(AppContext &context);
    void tickContent();
    void tickRightBar();
    bool reverse_board{false};

  protected:
    vec2f board_tex_pos{};
    vec2f board_tex_size{};
    float aspect{1};

    vec2f board_pos_ratio{0.085f, 0.085f};
    vec2f board_size_ratio{0.82f, 0.82f};

    vec2f grid_pos{};
    vec2f grid_size{100.0f, 100.0f};
    vec2f offset{};

    // Chess
    phys::chess::Match_Context game_context;
    phys::chess::Vec2 selected_square{4, 1};

    phys::vec2f getPosOnBoard(int x, int y);
    bool drawPieceButton(const char *id, int x, int y, phys::gl::Texture &piece, float transparency);

    void drawBord();
    void drawPieces(phys::chess::Board &board);
    void drawMoves(phys::chess::Moves &moves);

    void drawModifyWindow();
    void drawPromote();
    void drawPoints();

  private:
};

} // namespace phys::app
