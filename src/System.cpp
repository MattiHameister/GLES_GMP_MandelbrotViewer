#include "System.hpp"
#include "Timer.hpp"

System::System()
{
  done = false;
  logic = new Logic();
  renderer = new Renderer(logic);
}

System::~System()
{
  Cleanup();
}

// Initialization functions
void System::InitApp(void)
{
  InitializeSDL();
}

void System::ResizeView(uint32_t w, uint32_t h)
{
  logic->Resize(w,h);
  renderer->InitRenderer(w,h);
}

void System::InitializeSDL(void)
{
  SDL_Init(SDL_INIT_VIDEO);

  // Create OpenGL ES2 Context
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  int w = 1024;
  int h = 768;
  SDL_CreateWindowAndRenderer(w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer);

  ResizeView(w, h);
}

// Cleanup functions
void System::Cleanup(void)
{
  delete renderer;
  delete logic;
  SDL_Quit();
}

void System::HandleEvent(SDL_Event &event)
{
  switch(event.type) {

      //    case SDL_KEYDOWN:
      //      break;

    case SDL_QUIT:
      logic->Stop();
      done = true;
      return;
      break;

    case SDL_WINDOWEVENT:
      if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
        ResizeView(event.window.data1, event.window.data2);
      }
      break;

      //###################################################
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
        case SDLK_UP:
          logic->KeyDownEvent(1);
          break;
        case SDLK_RIGHT:
          logic->KeyDownEvent(2);
          break;
        case SDLK_DOWN:
          logic->KeyDownEvent(3);
          break;
        case SDLK_LEFT:
//          renderer->MakeScreenshot();
          break;
        default:
          break;
      }
      break;
      //###################################################

    case SDL_MOUSEMOTION:
      break;

    case SDL_MOUSEBUTTONDOWN:
      logic->MouseDownEvent(event.button.button, event.button.x, event.button.y);
      break;

    case SDL_MOUSEBUTTONUP:
      logic->MouseUpEvent(event.button.button, event.button.x, event.button.y);
      break;

    default:
      break;
  }   // End switch
}

// Event-related functions
void System::EventLoop(void)
{
  SDL_Event event;
  Timer fps;
  timeSinceLastFrame = 0;
  fps.Start();
  while(!done) {
    while (SDL_PollEvent(&event)) {
      HandleEvent(event);
    }

    GameLoop();

    //Cap the frame rate
    if( fps.Get_Ticks() < 1000 / FRAMES_PER_SECOND ) {
      SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.Get_Ticks() );
    }
    timeSinceLastFrame = fps.Get_Delta();
    fps.Start();
  }
}
float timef = 0.0f;
// Game related functions
void System::GameLoop(void)
{
  GameLogic();
  timef+=timeSinceLastFrame;
  if(timef>0.25f) {
    RenderFrame();
    timef=0.0f;
  }
}

void System::GameLogic(void)
{
  logic->DoLogic(timeSinceLastFrame);
  if(logic->RedrawDisplay(false)) {
    renderer->Reset();
  }
}

void System::RenderFrame(void)
{
  renderer->Render();
  SDL_RenderPresent(sdlRenderer);
}
