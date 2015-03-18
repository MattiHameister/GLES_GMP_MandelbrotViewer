#include "Timer.hpp"
#include "SDL.h"

Timer::Timer()
{
  //Initialize the variables
  startTicks = 0;
  pausedTicks = 0;
  paused = false;
  started = false;
}

Timer::~Timer()
{
}

void Timer::Start()
{
  //Start the timer
  started = true;

  //Unpause the timer
  paused = false;

  //Get the current clock time
  startTicks = SDL_GetTicks();
}

void Timer::Stop()
{
  //Stop the timer
  started = false;

  //Unpause the timer
  paused = false;
}

void Timer::Pause()
{
  //If the timer is running and isn't already paused
  if( ( started == true ) && ( paused == false ) ) {
    //Pause the timer
    paused = true;

    //Calculate the paused ticks
    pausedTicks = SDL_GetTicks() - startTicks;
  }
}

void Timer::Unpause()
{
  //If the timer is paused
  if( paused == true ) {
    //Unpause the timer
    paused = false;

    //Reset the starting ticks
    startTicks = SDL_GetTicks() - pausedTicks;

    //Reset the paused ticks
    pausedTicks = 0;
  }
}

int Timer::Get_Ticks()
{
  //If the timer is running
  if( started == true ) {
    //If the timer is paused
    if( paused == true ) {
      //Return the number of ticks when the timer was paused
      return pausedTicks;
    } else {
      //Return the current time minus the start time
      return SDL_GetTicks() - startTicks;
    }
  }

  //If the timer isn't running
  return 0;
}

double Timer::Get_Delta()
{
  return Get_Ticks() / 1000.0;
}

bool Timer::Is_Started()
{
  return started;
}

bool Timer::Is_Paused()
{
  return paused;
}

