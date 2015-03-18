#pragma once

#include "main.hpp"
#include <queue>
#include "Area.hpp"
#include <vector>
#include <mutex>

typedef uint32_t coord_t; // type of the coordinate-system
typedef uint32_t index_t; // type of the index to represent the coordinates

class Screen {
private:
  coord_t width;
  coord_t height;
  coord_t depth;
  uint32_t oversampling;
  iter_t *screen;
  std::queue<uint32_t> jobs;
  std::mutex lock;
  virtual uint32_t Parent(uint32_t job) const;
  virtual uint32_t Path(uint32_t job) const;
  virtual uint32_t Trace(uint32_t job, std::vector<uint32_t> &path) const;
  virtual void Cell(uint32_t job, Area<coord_t> &area) const;

public:
  Screen(coord_t x, coord_t y, coord_t overs);
  virtual ~Screen();
  virtual void Reset();
  virtual void SplitJob(uint32_t job);
  virtual uint32_t NextJob();
  virtual void AreaForJob(uint32_t job, Area<coord_t> &area) const;
  virtual iter_t& Iteration(iter_t iter);
  virtual iter_t& Iteration(uint32_t x, uint32_t y);
  virtual void PrepareVector(std::vector<iter_t> &iterations) const;
  virtual bool IterationForPixel(coord_t x, coord_t y, std::vector<iter_t> &iterations) const;
  virtual index_t TransformCoord(coord_t x, coord_t y) const;
  virtual void TransformIndex(index_t index, coord_t &x, coord_t &y) const;
  virtual index_t TransformCoord(coord_t x, coord_t y, coord_t z) const;
  virtual void TransformIndex(index_t index, coord_t &x, coord_t &y, coord_t &z) const;
  virtual coord_t Width() const;
  virtual coord_t Height() const;
  virtual uint32_t Oversampling() const;
  virtual iter_t* FullScreen();
};
