#pragma once

class Timer {
private:
  //The clock time when the timer started
  int startTicks;

  //The ticks stored when the timer was paused
  int pausedTicks;

  //The timer status
  bool paused;
  bool started;

public:
  //Initializes variables
  Timer();
  virtual ~Timer();

  //The various clock actions
  virtual void Start();
  virtual void Stop();
  virtual void Pause();
  virtual void Unpause();

  //Gets the timer's time
  virtual int Get_Ticks();
  virtual double Get_Delta();

  //Checks the status of the timer
  virtual bool Is_Started();
  virtual bool Is_Paused();
};

