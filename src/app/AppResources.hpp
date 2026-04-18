#pragma once
#include "graphics/GladWrap.hpp"
#include "imgui.h"

namespace phys
{

class AppResources
{
  public:
    ImFont *font_regular;
    ImFont *font_small;
};

class GlResources
{
  public:
    static constexpr int grid_amount = 800;
    gl::ShaderMain mainShader{};
    gl::ShaderBasic shader_basic{};
    gl::ShaderBlur shader_blur{};
    gl::ShaderCombine shader_combine{};

    gl::VertexArray sphere{};
    gl::VertexArray grid{};
    gl::VertexArray quad{};

    gl::Texture default_tex;
    gl::Texture board_tex;

    gl::Texture b_pawn;
    gl::Texture b_rook;
    gl::Texture b_queen;
    gl::Texture b_knight;
    gl::Texture b_king;
    gl::Texture b_bishop;

    gl::Texture w_pawn;
    gl::Texture w_rook;
    gl::Texture w_queen;
    gl::Texture w_knight;
    gl::Texture w_king;
    gl::Texture w_bishop;

    gl::Texture move;
    GlResources();
};

class AppContext
{
  public:
    AppResources resources_app;
    GlResources resources_gl;
};

} // namespace phys
