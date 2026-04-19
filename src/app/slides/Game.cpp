
#include "Game.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"
#include "app/AppResources.hpp"
#include "app/widgets/extra.hpp"
#include "chess/chess.hpp"
#include "core/Units.hpp"
#include "core/tools/Debug.hpp"
#include "core/tools/Error.hpp"
#include "graphics/GladWrap.hpp"
#include "imgui-SFML.h"
#include <format>
#include <imgui.h>
#include <memory>
#include <ranges>
#include <utility>
using namespace phys::app;

GameSlide::GameSlide(AppContext &context) : Slide(context), game_context(chess::createDefaultContext())
{
}

void drawChessBoard()
{
}

const int GRID_ROW = 8;
const int GRID_COLUMN = 8;

phys::vec2f GameSlide::getPosOnBoard(int x, int y)
{
    if (this->reverse_board)
        return this->grid_pos + phys::vec2f(offset.x * x, offset.y * y) + offset / 2.0f;
    else
        return this->grid_pos + offset * 8.0f - (phys::vec2f(offset.x * x, offset.y * y) + offset / 2.0f);
}

bool GameSlide::drawPieceButton(const char *id, int x, int y, phys::gl::Texture &piece_tex, float transparency)
{
    const vec2f PIECE_SIZE = this->offset * 0.8f;

    const auto PIECE_POS = getPosOnBoard(x, y) - PIECE_SIZE / 2.0f;

    ImGui::SetCursorScreenPos(vec2f(this->board_tex_pos + vec2f(PIECE_POS)));
    return ImGui::ImageButton(id, ImTextureID(piece_tex.getID()), vec2f(PIECE_SIZE), vec2f(0, 0), vec2f(1, 1),
                              ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, transparency));
}

void GameSlide::drawBord()
{
    ImGui::SetCursorScreenPos(this->board_tex_pos);
    auto &board_tex = this->context.resources_gl.board_tex;
    ImGui::Image(ImTextureID(board_tex.getID()), this->board_tex_size);

    ImGui::SetCursorScreenPos(board_tex_pos);
    ImGui::DrawRect(sf::FloatRect(this->grid_pos, this->grid_size), sf::Color::Red);
}

phys::gl::Texture &getPieceTexture(phys::chess::PieceType type_p, phys::GlResources &context)
{
    using ptype = phys::chess::PieceType;
    assert(type_p != ptype::empty);

    switch (type_p)
    {
    case ptype::w_pawn:
        return context.w_pawn;

    case ptype::w_rook:
        return context.w_rook;

    case ptype::w_knight:
        return context.w_knight;

    case ptype::w_bishop:
        return context.w_bishop;

    case ptype::w_queen:
        return context.w_queen;

    case ptype::w_king:
        return context.w_king;

    case ptype::b_pawn:
        return context.b_pawn;

    case ptype::b_rook:
        return context.b_rook;

    case ptype::b_knight:
        return context.b_knight;

    case ptype::b_bishop:
        return context.b_bishop;

    case ptype::b_queen:
        return context.b_queen;

    case ptype::b_king:
        return context.b_king;
    default:
        assert(false);
        break;
    }
    return context.default_tex;
}

void GameSlide::drawPieces(phys::chess::Board &board)
{
    for (auto &&[x, inner] : std::views::enumerate(board))
    {
        for (auto &&[y, piece] : std::views::enumerate(inner))
        {
            if (piece != chess::PieceType::empty)
            {
                auto &tex = getPieceTexture(piece, this->context.resources_gl);
                auto id = std::format("{},{}", x, y);
                if (drawPieceButton(id.c_str(), x, y, tex, 1.0f))
                {
                    this->selected_square = {x, y};

                    auto piece_on_square = phys::chess::getPiece(this->selected_square, this->game_context.board);
                    if (chess::isWhite(piece_on_square) == this->game_context.is_white_turn &&
                        phys::chess::isPromotion(this->selected_square, this->game_context))
                    {
                        ImGui::OpenPopup("PromotionMenu");
                    }
                }
            }
        }
    }
}

void GameSlide::drawMoves(phys::chess::Moves &moves)
{
    for (auto &&[i, move] : std::views::enumerate(moves))
    {
        auto id = std::format("{}", i);
        if (drawPieceButton(id.c_str(), move.x, move.y, this->context.resources_gl.move, 0.7f))
        {
            this->game_context = phys::chess::move(this->selected_square, move, this->game_context);
        }
    }
}

void GameSlide::tickContent()
{
    ImGui::Begin("Game", nullptr);

    // Math

    const auto GRID = phys::vec2f(GRID_ROW, GRID_COLUMN);
    const auto OFFSET = phys::vec2f(this->grid_size.x / GRID.x, this->grid_size.y / GRID.y);
    const auto SIZE = ImGui::GetContentRegionAvail();

    float SIZE_aspect = SIZE.x / SIZE.y;

    if (SIZE_aspect > this->aspect)
    {
        float height = SIZE.y;
        float width = height * this->aspect;
        this->board_tex_size = phys::vec2f(width, height);
    }
    else
    {
        float width = SIZE.x;
        float height = width / this->aspect;
        this->board_tex_size = phys::vec2f(width, height);
    }

    this->board_tex_pos =
        phys::vec2f(ImGui::GetCursorScreenPos()) + phys::vec2f(SIZE) / 2.0f - this->board_tex_size / 2.0f;

    this->offset = OFFSET;
    this->grid_size = this->board_tex_size * this->board_size_ratio;
    this->grid_pos = this->board_tex_size * this->board_pos_ratio;

    auto piece_on_selected = phys::chess::getPiece(this->selected_square, this->game_context.board);

    // Popups
    if (ImGui::BeginPopup("PromotionMenu"))
    {
        drawPromote();
        ImGui::EndPopup();
    }

    // Layers
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);

    // Front Layer
    draw_list->ChannelsSetCurrent(1);

    if (!chess::isEmpty(piece_on_selected) && chess::isWhite(piece_on_selected) == this->game_context.is_white_turn)
    {
        chess::Moves moves;
        chess::getMoves(moves, this->selected_square, this->game_context);
        drawMoves(moves);
    }

    // Back Layer
    draw_list->ChannelsSetCurrent(0);
    drawBord();
    drawPieces(this->game_context.board);

    draw_list->ChannelsMerge();

    drawPoints();
    ImGui::End();

    drawModifyWindow();
}

void GameSlide::drawPromote()
{
    const vec2f PIECE_SIZE = this->offset * 0.8f;

    auto imgbutton = [PIECE_SIZE, this](const char *id, ImTextureID texId, chess::PieceType piece)
    {
        if (ImGui::ImageButton(id, texId, vec2f(PIECE_SIZE)))
        {
            this->game_context = phys::chess::promote(this->selected_square, piece, this->game_context);
        }
    };

    using ptype = phys::chess::PieceType;
    auto piece = phys::chess::getPiece(this->selected_square, this->game_context.board);
    if (phys::chess::isEmpty(piece) || phys::chess::isWhite(piece) != this->game_context.is_white_turn)
        return;
    if (phys::chess::isWhite(piece))
    {
        imgbutton("Knight", ImTextureID(this->context.resources_gl.w_knight.getID()), ptype::w_knight);
        imgbutton("Bishop", ImTextureID(this->context.resources_gl.w_bishop.getID()), ptype::w_bishop);
        imgbutton("Rook", ImTextureID(this->context.resources_gl.w_rook.getID()), ptype::w_rook);
        imgbutton("Queen", ImTextureID(this->context.resources_gl.w_queen.getID()), ptype::w_queen);
    }
    else
    {
        imgbutton("Knight", ImTextureID(this->context.resources_gl.b_knight.getID()), ptype::b_knight);
        imgbutton("Bishop", ImTextureID(this->context.resources_gl.b_bishop.getID()), ptype::b_bishop);
        imgbutton("Rook", ImTextureID(this->context.resources_gl.b_rook.getID()), ptype::b_rook);
        imgbutton("Queen", ImTextureID(this->context.resources_gl.b_queen.getID()), ptype::b_queen);
    }
}

void GameSlide::drawPoints()
{
    auto black_str = std::format("Black: {}", this->game_context.black_points);
    auto white_str = std::format("White: {}", this->game_context.white_points);

    ImGui::Text(black_str.c_str());
    ImGui::Text(white_str.c_str());
}

void GameSlide::drawModifyWindow()
{
    ImGui::Begin("Modify");

    ImGui::InputFloat2("Pos Ratio", reinterpret_cast<float *>(&this->board_pos_ratio));
    ImGui::InputFloat2("Size Ratio", reinterpret_cast<float *>(&this->board_size_ratio));

    ImGui::End();
}
