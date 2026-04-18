
#pragma once
#include "App.hpp"
#include "app/AppResources.hpp"
#include "slides/Game.hpp"

namespace phys::app
{

enum class SlideType
{
    Menu,
    Game
};

class GameApp : public App
{
  public:
    GameApp(sf::ContextSettings settings);

  protected:
    AppContext appContext;
    void tick() override;

  private:
    SlideType selected_slide{SlideType::Game};
    GameSlide game_slide{appContext};

    void buildDock(int dock_id);
};
} // namespace phys::app
