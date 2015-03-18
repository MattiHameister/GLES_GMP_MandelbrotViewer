#pragma once

#include "main.hpp"
#include "SDL.h"
#include "Renderer.hpp"
#include "Logic.hpp"
#include "Screen.hpp"

class System {
public:
  System();
  virtual ~System();

  virtual void InitApp(void);
  virtual void InitializeSDL(void);
  virtual void ResizeView(uint32_t w, uint32_t h);

  // Cleanup functions
  virtual void Cleanup(void);

  virtual void HandleEvent(SDL_Event &event);
  // Event-related functions
  virtual void EventLoop(void);

  // Game related functions
  virtual void GameLoop(void);
  virtual void GameLogic(void);
  virtual void RenderFrame(void);

private:
  double timeSinceLastFrame;
  Renderer *renderer;
  Logic *logic;
  bool done;

  SDL_Window *sdlWindow;
  SDL_Renderer *sdlRenderer;
};

const int RUN_GAME_LOOP = 1;
