#include "chess.hpp"
#include "libs/date_time/include/boost/date_time/time_defs.hpp"
#include <cmath>
#include <glm/geometric.hpp>
#include <iterator>
#include <ranges>
#include <vector>

using namespace phys::chess;

bool phys::chess::isEmpty(PieceType type)
{
    return type == PieceType::empty;
}

bool phys::chess::isWhite(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::w_bishop:
    case phys::chess::PieceType::w_king:
    case phys::chess::PieceType::w_knight:
    case phys::chess::PieceType::w_pawn:
    case phys::chess::PieceType::w_queen:
    case phys::chess::PieceType::w_rook:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isDiagonal(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::w_bishop:
    case phys::chess::PieceType::w_queen:
    case phys::chess::PieceType::b_bishop:
    case phys::chess::PieceType::b_queen:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isStraight(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::w_queen:
    case phys::chess::PieceType::w_rook:
    case phys::chess::PieceType::b_queen:
    case phys::chess::PieceType::b_rook:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isPawn(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_pawn:
    case phys::chess::PieceType::w_pawn:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isKnight(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_knight:
    case phys::chess::PieceType::w_knight:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isKing(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_king:
    case phys::chess::PieceType::w_king:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isBishop(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_bishop:
    case phys::chess::PieceType::w_bishop:
        return true;
    default:
        return false;
    }
}
bool phys::chess::isRook(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_rook:
    case phys::chess::PieceType::w_rook:
        return true;
    default:
        return false;
    }
}

bool phys::chess::isQueen(PieceType type)
{
    assert(type != PieceType::empty);
    switch (type)
    {
    case phys::chess::PieceType::b_queen:
    case phys::chess::PieceType::w_queen:
        return true;
    default:
        return false;
    }
}

Board phys::chess::move(Vec2 pos_origin, Vec2 pos_dest, Board &board)
{
    assert(isInside(pos_origin));
    assert(isInside(pos_dest));

    auto piece_on_origin = getPiece(pos_origin, board);
    assert(!isEmpty(piece_on_origin));

    Board board_dest;
    std::ranges::copy(board, board_dest.begin());

    board_dest[pos_origin.x][pos_origin.y] = PieceType::empty;
    board_dest[pos_dest.x][pos_dest.y] = board[pos_origin.x][pos_origin.y];
    return board_dest;
}

bool phys::chess::isInside(Vec2 pos)
{
    return pos.x < 8 && pos.y < 8 && pos.x >= 0 && pos.y >= 0;
}

PieceType phys::chess::getPiece(Vec2 pos, Board &board)
{
    if (!isInside(pos))
        return PieceType::empty;
    auto type_on_square = board[pos.x][pos.y];
    return type_on_square;
}

std::pair<PieceType, Vec2> phys::chess::rayCast(Vec2 direction, Vec2 origin, Board &board)
{
    assert(isInside(origin));

    auto ray_pos = origin + direction;
    while (isInside(ray_pos))
    {
        auto type_on_square = board[ray_pos.x][ray_pos.y];
        if (type_on_square != PieceType::empty)
            return std::make_pair(type_on_square, ray_pos);
        ray_pos += direction;
    }
    return std::make_pair(PieceType::empty, Vec2(-1, -1));
}

MatchContext phys::chess::move(Vec2 origin, Vec2 dest, MatchContext &context)
{

    MatchContext context_new = context;
    context_new.board = move(origin, dest, context.board);

    auto piece_on_dest = getPiece(dest, context.board);
    if (!isEmpty(piece_on_dest))
    {
        context_new.fallen_pieces.push_back(piece_on_dest);
        auto &points = isWhite(piece_on_dest) ? context_new.black_points : context_new.white_points;
        if (isPawn(piece_on_dest))
            points += 1;
        if (isKnight(piece_on_dest))
            points += 3;
        if (isBishop(piece_on_dest))
            points += 3;
        if (isRook(piece_on_dest))
            points += 5;
        if (isQueen(piece_on_dest))
            points += 9;
        if (isKing(piece_on_dest))
            points += 999999;
    }

    auto piece_on_origin = getPiece(origin, context.board);

    // King cords
    if (piece_on_origin == PieceType::w_king)
    {
        context_new.king_pos_w = dest;
    }
    if (piece_on_origin == PieceType::b_king)
    {
        context_new.king_pos_b = dest;
    }

    // Handle pawn passant killing
    if (isPawn(piece_on_origin) && isEmpty(piece_on_dest))
    {
        auto diff = dest - origin;
        auto passant_cord = origin + Vec2(diff.x, 0);
        auto passant_piece_type = getPiece(passant_cord, context.board);
        if (passant_cord == context.pawn_can_get_passanted)
        {
            context_new.fallen_pieces.push_back(context.board[passant_cord.x][passant_cord.y]);
            context_new.board[passant_cord.x][passant_cord.y] = PieceType::empty;

            auto &points = isWhite(passant_piece_type) ? context_new.black_points : context_new.white_points;
            points += 1;
        }
    }

    // Can be Pasanted
    if (isPawn(piece_on_origin) && std::abs(dest.y - origin.y) == 2)
    {
        context_new.pawn_can_get_passanted = dest;
    }
    else
    {
        context_new.pawn_can_get_passanted = {-1, -1};
    }

    context_new.is_white_turn = !context_new.is_white_turn;

    return context_new;
}

bool phys::chess::canAttackSquare(Vec2 origin, Vec2 dest, MatchContext &context, MatchContext &new_context,
                                  bool is_white)
{
    assert(isInside(dest));
    assert(isInside(origin));
    assert(isInside(context.king_pos_w));
    assert(isInside(context.king_pos_b));

    auto piece_on_dest = getPiece(dest, context.board);
    if (isEmpty(piece_on_dest))
        return true;

    if (isWhite(piece_on_dest) == is_white)
        return false;

    if (!isSafeKing(new_context, is_white))
        return false;

    return true;
}

bool phys::chess::isEmptySquare(Vec2 square_pos, Board &board)
{
    assert(isInside(square_pos));

    auto typeOnSquare = board[square_pos.x][square_pos.y];
    return typeOnSquare == PieceType::empty;
}

bool phys::chess::isSafeSquare(Vec2 square_pos, Board &board, bool is_white)
{
    assert(isInside(square_pos));

    auto isStraightsDangerous = [square_pos, &board, is_white](Vec2 dir)
    {
        auto [piece, pos] = rayCast(dir, square_pos, board);
        auto isDangerous = !isEmpty(piece) && isWhite(piece) != is_white && isStraight(piece);
        return isDangerous;
    };

    auto s_1_d = isStraightsDangerous({0, 1});
    auto s_2_d = isStraightsDangerous({1, 0});
    auto s_3_d = isStraightsDangerous({0, -1});
    auto s_4_d = isStraightsDangerous({-1, 0});

    if (s_1_d || s_2_d || s_3_d || s_4_d)
    {
        return false;
    }

    auto isDiagonalDangerous = [square_pos, &board, is_white](Vec2 dir)
    {
        auto [piece, pos] = rayCast(dir, square_pos, board);
        auto isDangerous = !isEmpty(piece) && isWhite(piece) != is_white && isDiagonal(piece);
        return isDangerous;
    };
    // DIAGONALS

    auto d_1_d = isDiagonalDangerous({1, 1});
    auto d_2_d = isDiagonalDangerous({-1, 1});
    auto d_3_d = isDiagonalDangerous({-1, -1});
    auto d_4_d = isDiagonalDangerous({1, -1});

    if (d_1_d || d_2_d || d_3_d || d_4_d)
    {
        return false;
    }

    // KNIGHTS
    auto horse_check = [square_pos, &board, is_white](Vec2 pos)
    {
        auto horse_1_vec = square_pos + pos;

        if (!isInside(horse_1_vec))
            return false;

        if (isEmptySquare(horse_1_vec, board))
            return false;

        auto piece = getPiece(horse_1_vec, board);
        return isWhite(piece) != is_white && isKnight(piece);
    };

    auto h_1 = horse_check({2, 1});
    auto h_2 = horse_check({2, -1});
    auto h_3 = horse_check({1, -2});
    auto h_4 = horse_check({-1, -2});
    auto h_5 = horse_check({-2, -1});
    auto h_6 = horse_check({-2, 1});
    auto h_7 = horse_check({1, 2});
    auto h_8 = horse_check({-1, 2});

    if (h_1 || h_2 || h_3 || h_4 || h_5 || h_6 || h_7 || h_8)
    {
        return false;
    }

    /// PAWN
    auto pawn_check = [square_pos, &board](Vec2 pos, bool is_white)
    {
        auto pawn_vec = square_pos + pos;
        if (!isInside(pawn_vec))
            return false;
        if (isEmptySquare(pawn_vec, board))
            return false;
        auto piece = getPiece(pawn_vec, board);
        return isWhite(piece) != is_white && isPawn(piece);
    };

    bool p_1;
    bool p_2;

    if (is_white)
    {
        p_1 = pawn_check({1, 1}, is_white);
        p_2 = pawn_check({-1, 1}, is_white);
    }
    else
    {
        p_1 = pawn_check({1, -1}, is_white);
        p_2 = pawn_check({-1, -1}, is_white);
    }

    if (p_1 || p_2)
    {
        return false;
    }

    return true;
}

bool phys::chess::isSafeKing(MatchContext &context, bool is_white)
{
    if (is_white)
    {
        return isSafeSquare(context.king_pos_w, context.board, is_white);
    }
    else
    {
        return isSafeSquare(context.king_pos_b, context.board, is_white);
    }
}

void phys::chess::path(Moves &moves, Vec2 origin, Vec2 direction, int length, MatchContext &context, bool is_white,
                       bool can_attack)
{
    int i = 1;
    Vec2 ray = origin + direction;
    while (i <= length && isInside(ray))
    {
        MatchContext new_context = move(origin, ray, context);
        auto is_king_safe = isSafeKing(new_context, is_white);

        if (isEmptySquare(ray, context.board))
        {
            if (is_king_safe)
                moves.push_back(ray);
            ray += direction;
            i++;
            continue;
        }
        else if (can_attack && canAttackSquare(origin, ray, context, new_context, is_white))
        {
            if (is_king_safe)
                moves.push_back(ray);
        }
        return;
    }
}

void phys::chess::knight(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{

    auto knight_move = [origin, &context, is_white, &moves](Vec2 diff)
    {
        Vec2 dest = origin + diff;
        if (!isInside(dest))
            return;
        auto context_new = move(origin, dest, context);
        auto is_king_safe = isSafeKing(context_new, is_white);

        if (canAttackSquare(origin, dest, context, context_new, is_white))
        {
            if (is_king_safe)
                moves.push_back(dest);
        }
    };

    knight_move({2, 1});
    knight_move({2, -1});
    knight_move({1, -2});
    knight_move({-1, -2});
    knight_move({-2, -1});
    knight_move({-2, 1});
    knight_move({-1, 2});
    knight_move({1, 2});
}
void phys::chess::king(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{

    auto king_move = [origin, &context, is_white, &moves](Vec2 dir)
    { path(moves, origin, dir, 1, context, is_white, true); };

    auto is_rook_castlable = [is_white, context](Vec2 pos)
    {
        bool correct_position = pos == Vec2(0, 0) || pos == Vec2(7, 0) || pos == Vec2(0, 7) || pos == Vec2(7, 7);
        if (!correct_position)
            return false;

        auto piece_on_position = getPiece(pos, context.board);
        bool correct_piece = !isEmpty(piece_on_position) && isRook(piece_on_position) && isWhite(piece_on_position);
    };

    auto king_castle = [origin, &context, is_white, &moves, is_rook_castlable](int dir)
    {
        auto castling_rights = is_white ? context.castling_w : context.castling_b;
        bool king_unmoved = castling_rights.king_unmoved;
        bool rook_unmoved = dir > 0 ? castling_rights.rook_r_unmoved : castling_rights.rook_l_unmoved;

        PieceType target_piece_type = is_white ? PieceType::w_rook : PieceType::b_rook;
        auto [piece_type, cord] = rayCast({dir, 0}, origin, context.board);

        bool can_castle = king_unmoved && rook_unmoved && piece_type == target_piece_type && is_rook_castlable(cord);
        if (!can_castle)
            return;

        // King safety
        auto first_step = move(origin, origin + Vec2(dir, 0), context);
        auto second_step = move(origin, origin + Vec2(dir * 2, 0), context);
        second_step = move(cord, origin + Vec2(-1, 0), second_step);

        bool can_castle_safely = isSafeKing(first_step, is_white) && isSafeKing(second_step, is_white);
        if (can_castle_safely)
            moves.push_back(origin + Vec2(dir * 2, 0));
    };

    king_move({0, 1});
    king_move({1, 1});
    king_move({1, 0});
    king_move({1, -1});
    king_move({0, -1});
    king_move({-1, -1});
    king_move({-1, 0});
    king_move({-1, 1});

    if (isSafeKing(context, is_white))
    {
        king_castle(1);
        king_castle(-1);
    }
}

void phys::chess::rook(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{

    auto rook_move = [origin, &context, is_white, &moves](Vec2 dir)
    { path(moves, origin, dir, 11, context, is_white, true); };

    rook_move({0, 1});
    rook_move({0, -1});
    rook_move({1, 0});
    rook_move({-1, 0});
}
void phys::chess::bishop(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{

    auto bishop_move = [origin, &context, is_white, &moves](Vec2 dir)
    { path(moves, origin, dir, 16, context, is_white, true); };

    bishop_move({1, 1});
    bishop_move({1, -1});
    bishop_move({-1, -1});
    bishop_move({-1, 1});
}

void phys::chess::queen(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{
    rook(moves, origin, context, is_white);
    bishop(moves, origin, context, is_white);
}

constexpr int pawn_y_w = 1;
constexpr int pawn_y_b = 6;

void phys::chess::pawn(Moves &moves, Vec2 origin, MatchContext &context, bool is_white)
{

    auto pawn_diagonal = [origin, &context, is_white, &moves](Vec2 dir)
    {
        auto dest = origin + dir;
        auto piece = getPiece(dest, context.board);
        if (!isEmpty(piece))
            path(moves, origin, dir, 1, context, is_white, true);
    };

    auto pawn_straight = [origin, &context, is_white, &moves](Vec2 dir, int length)
    { path(moves, origin, dir, length, context, is_white, false); };

    auto passant = [origin, &context, is_white, &moves](Vec2 dir, Vec2 dir2)
    {
        auto passant_dest = origin + dir;
        auto piece_on_pasant = getPiece(passant_dest, context.board);
        if (!isEmpty(piece_on_pasant) && isWhite(piece_on_pasant) != is_white &&
            passant_dest == context.pawn_can_get_passanted)
        {
            path(moves, origin, dir2, 1, context, is_white, false);
        }
    };

    if (is_white)
    {
        pawn_diagonal({1, 1});
        pawn_diagonal({-1, 1});
        if (origin.y == pawn_y_w)
        {
            pawn_straight({0, 1}, 2);
        }
        else
        {
            pawn_straight({0, 1}, 1);
        }
        passant({-1, 0}, {-1, 1});
        passant({1, 0}, {1, 1});
    }
    else
    {
        pawn_diagonal({1, -1});
        pawn_diagonal({-1, -1});
        if (origin.y == pawn_y_b)
        {
            pawn_straight({0, -1}, 2);
        }
        else
        {
            pawn_straight({0, -1}, 1);
        }
        passant({-1, 0}, {-1, -1});
        passant({1, 0}, {1, -1});
    }
}

void phys::chess::getMoves(Moves &out, Vec2 origin, MatchContext &context)
{
    auto piece = getPiece(origin, context.board);
    if (piece == PieceType::empty)
        return;
    auto is_white = isWhite(piece);

    if (isPawn(piece))
        pawn(out, origin, context, is_white);

    if (isKnight(piece))
        knight(out, origin, context, is_white);

    if (isKing(piece))
        king(out, origin, context, is_white);

    if (isRook(piece))
        rook(out, origin, context, is_white);

    if (isBishop(piece))
        bishop(out, origin, context, is_white);

    if (isQueen(piece))
        queen(out, origin, context, is_white);
}

void phys::chess::getPromotions(Moves &out, MatchContext &context)
{

    auto is_white_king_safe = isSafeKing(context, true);
    auto is_black_king_safe = isSafeKing(context, false);

    for (int x = 0; x < 8; x++)
    {
        auto pieceBot = getPiece(Vec2(x, 0), context.board);
        auto pieceTop = getPiece(Vec2(x, 7), context.board);

        if (pieceBot == PieceType::b_pawn && is_black_king_safe)
        {
            out.push_back({x, 0});
        }
        if (pieceTop == PieceType::w_pawn && is_white_king_safe)
        {
            out.push_back({x, 7});
        }
    }
}

bool phys::chess::isPromotion(Vec2 origin, MatchContext &context)
{
    auto piece = getPiece(origin, context.board);
    if (piece == PieceType::b_pawn && origin.y == 0)
    {
        if (isSafeKing(context, false))
            return true;
    }
    if (piece == PieceType::w_pawn && origin.y == 7)
    {
        if (isSafeKing(context, true))
            return true;
    }
    return false;
}

MatchContext phys::chess::promote(Vec2 origin, PieceType resultType, MatchContext &context)
{
    auto piece = getPiece(origin, context.board);
    if (!isPromotion(origin, context))
    {
        assert(false);
        return context;
    }

    auto context_new = context;
    context_new.board[origin.x][origin.y] = resultType;

    context_new.pawn_can_get_passanted = {-1, -1};
    context_new.is_white_turn = !context_new.is_white_turn;

    return context_new;
}

Board phys::chess::createDefaultBoard()
{
    Board board{};
    for (int i = 0; i < 8; i++)
    {
        board[i][pawn_y_w] = PieceType::w_pawn;
        board[i][pawn_y_b] = PieceType::b_pawn;
    }

    int w_y = 0;
    int b_y = 7;

    auto createBackRows = [&board, w_y, b_y]()
    {
        board[0][w_y] = PieceType::w_rook;
        board[0][b_y] = PieceType::b_rook;

        board[1][w_y] = PieceType::w_knight;
        board[1][b_y] = PieceType::b_knight;

        board[2][w_y] = PieceType::w_bishop;
        board[2][b_y] = PieceType::b_bishop;

        board[3][w_y] = PieceType::w_queen;
        board[3][b_y] = PieceType::b_queen;

        board[4][w_y] = PieceType::w_king;
        board[4][b_y] = PieceType::b_king;

        board[5][w_y] = PieceType::w_bishop;
        board[5][b_y] = PieceType::b_bishop;

        board[6][w_y] = PieceType::w_knight;
        board[6][b_y] = PieceType::b_knight;

        board[7][w_y] = PieceType::w_rook;
        board[7][b_y] = PieceType::b_rook;
    };

    createBackRows();
    return board;
}
MatchContext phys::chess::createDefaultContext()
{
    MatchContext context;
    context.board = createDefaultBoard();
    context.king_pos_w = {4, 0};
    context.king_pos_b = {4, 7};
    return context;
}
