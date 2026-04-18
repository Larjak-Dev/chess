#include "AppResources.hpp"
using namespace phys;

GlResources::GlResources()
{
    this->sphere.bufferSphere(64);
    this->grid.bufferLines(this->grid_amount, this->grid_amount, 0);
    this->default_tex.createColor({1.0, 1.0, 1.0, 1.0});
    this->quad.bufferQuad();

    this->board_tex.loadFromImage("assets/board.jpeg");

    this->b_pawn.loadFromImage("assets/b_pawn.png");
    this->b_rook.loadFromImage("assets/b_rook.png");
    this->b_queen.loadFromImage("assets/b_queen.png");
    this->b_knight.loadFromImage("assets/b_knight.png");
    this->b_king.loadFromImage("assets/b_king.png");
    this->b_bishop.loadFromImage("assets/b_bishop.png");

    this->w_pawn.loadFromImage("assets/w_pawn.png");
    this->w_rook.loadFromImage("assets/w_rook.png");
    this->w_queen.loadFromImage("assets/w_queen.png");
    this->w_knight.loadFromImage("assets/w_knight.png");
    this->w_king.loadFromImage("assets/w_king.png");
    this->w_bishop.loadFromImage("assets/w_bishop.png");

    this->move.loadFromImage("assets/move.jpg");
}
