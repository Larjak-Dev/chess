#pragma once

#include "glm/ext/vector_int2.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <vector>

namespace phys::chess
{
enum class PieceType
{
    empty,

    w_pawn,
    w_rook,
    w_queen,
    w_knight,
    w_king,
    w_bishop,

    b_pawn,
    b_rook,
    b_queen,
    b_knight,
    b_king,
    b_bishop
};

using Board = std::array<std::array<PieceType, 8>, 8>;
using Vec2 = glm::ivec2;
using Moves = boost::container::static_vector<Vec2, 30>;
using Pieces = boost::container::static_vector<PieceType, 32>;

bool isEmpty(PieceType type);
bool isWhite(PieceType type);
bool isDiagonal(PieceType type); // ex. bishop, queen
bool isStraight(PieceType type); // ex. rook, queen
bool isPawn(PieceType type);
bool isKnight(PieceType type);
bool isKing(PieceType type);
bool isBishop(PieceType type);
bool isRook(PieceType type);
bool isQueen(PieceType type);

bool isInside(Vec2 pos);
PieceType getPiece(Vec2 pos, Board &board);
std::pair<PieceType, Vec2> rayCast(Vec2 direction, Vec2 origin, Board &board);
Board move(Vec2 pos_origin, Vec2 pos_dest, Board &board);

bool isSafeSquare(Vec2 pos, Board &board, bool is_white);
bool isEmptySquare(Vec2 pos, Board &board);

// Advanced MatchContext Functions

struct CastlingRights
{
    bool king_unmoved{true};
    bool rook_l_unmoved{true};
    bool rook_r_unmoved{true};
};

struct MatchContext
{
    Board board;
    bool is_white_turn{true};

    // King Placement Context
    Vec2 king_pos_w;
    Vec2 king_pos_b;

    // Passant Context
    Vec2 pawn_can_get_passanted{-1, -1};

    // Casteling Context
    CastlingRights castling_w;
    CastlingRights castling_b;

    // Points
    Pieces fallen_pieces;
    int white_points{0};
    int black_points{0};
};

MatchContext move(Vec2 origin, Vec2 dest, MatchContext &context);
bool canAttackSquare(Vec2 origin, Vec2 dest, MatchContext &context, MatchContext &new_context, bool is_white);
bool canAttackSquare_King(Vec2 origin, Vec2 dest, MatchContext &context, MatchContext &new_context, bool is_white);
bool isSafeKing(MatchContext &context, bool is_white);

void path(Moves &out, Vec2 origin, Vec2 direction, int length, MatchContext &context, bool is_white, bool can_attack);
void knight(Moves &out, Vec2 origin, MatchContext &context, bool is_white);
void king(Moves &out, Vec2 origin, MatchContext &context, bool is_white);
void rook(Moves &out, Vec2 origin, MatchContext &context, bool is_white);
void bishop(Moves &out, Vec2 origin, MatchContext &context, bool is_white);
void queen(Moves &out, Vec2 origin, MatchContext &context, bool is_white);
void pawn(Moves &out, Vec2 origin, MatchContext &context, bool is_white);

void getMoves(Moves &out, Vec2 origin, MatchContext &context);

void getPromotions(Moves &out, MatchContext &context);
bool isPromotion(Vec2 origin, MatchContext &context);
MatchContext promote(Vec2 origin, PieceType resultType, MatchContext &context);

Board createDefaultBoard();
MatchContext createDefaultContext();

} // namespace phys::chess
