#include "Screen.hpp"
#include <cmath>

const uint32_t treeSize = 4;

//special rounding functions
uint32_t roundup(uint32_t z, uint32_t n)
{
  if(z==0 || n == 0) {
    return 0;
  }
  return z/n + 1;
}

uint32_t rounddown(uint32_t z, uint32_t n)
{
  if(n == 0) {
    return 0;
  }
  return z/n;
}

Screen::Screen(coord_t w, coord_t h, uint32_t overs)
{
  oversampling = overs;
  width = w*oversampling;
  height = h*oversampling;
  //TODO not needed here, but for the algorithm
  depth = 2;
  screen = new iter_t[width*height];
  Reset();
}

//initialize all memory
void Screen::Reset()
{
  std::fill(screen, screen + width*height, IS_BLANK);
  while(!jobs.empty()) {
    jobs.pop();
  }
  SplitJob(0);
}

void Screen::SplitJob(uint32_t job)
{
  std::lock_guard<std::mutex> guard(lock);
  for(uint32_t i=1; i<=treeSize; i++) {
    jobs.push(job*treeSize+i);
//    jobs.push(job*treeSize+2);
//    jobs.push(job*treeSize+3);
//    jobs.push(job*treeSize+4);
  }
}

uint32_t Screen::NextJob()
{
  std::lock_guard<std::mutex> guard(lock);
  uint32_t job = 0;
  if(!jobs.empty()) {
    job = jobs.front();
    jobs.pop();
  }
//  lock.unlock();
  return job;
}

uint32_t Screen::Parent(uint32_t job) const
{
  float c = job;
  return std::ceil(c/((float)treeSize) - 1);
}

uint32_t Screen::Path(uint32_t job) const
{
  float p = job;
  return p - Parent(job)*((float)treeSize);
}

uint32_t Screen::Trace(uint32_t job, std::vector<uint32_t> &path) const
{
  uint32_t e = 0;
  while(job > 0) {
    path.insert(path.begin(),Path(job));
    job = (job - 1) / treeSize;
    e++;
  }
  return e;
}

//calculate the area for the job
void Screen::Cell(uint32_t job, Area<coord_t> &area) const
{
  uint32_t sizeX = width-1;
  uint32_t sizeY = height-1;
  uint32_t sizeZ = depth-1;
  std::vector<uint32_t> path;
  /*uint32_t level = */Trace(job,path);

  //set the center to the center of the screen
  uint32_t xzM = sizeX;
  uint32_t xnM = 2;
  uint32_t yzM = sizeY;
  uint32_t ynM = 2;
  uint32_t zzM = sizeZ;
  uint32_t znM = 2;

  //these values hold the offset to the area
  //we only render the borders of the area
  //so do not render a pixel twice
  uint32_t right, left, top, bottom;
  right = left = top = bottom = 0;

  //for every path entry move the center to the given
  //value
  for(uint32_t i=0; i<path.size(); i++) {
    uint32_t p = path[i];
    // ignore the first value
    uint32_t offsetM = (i != 0);

//1|2//
//4|3//
    switch(p) {
        // move to the top left area
      case 1:
        xzM = (xzM * 2) - sizeX;
        xnM *= 2;
        yzM = (yzM * 2) - sizeY;
        ynM *= 2;
        zzM = (zzM * 2) - sizeZ;
        znM *= 2;
        right=0;
        bottom=0;
        top+=offsetM;
        left+=offsetM;
        break;
        // move to the top right area
      case 2:
        xzM = (xzM * 2) + sizeX;
        xnM *= 2;
        yzM = (yzM * 2) - sizeY;
        ynM *= 2;
        zzM = (zzM * 2) - sizeZ;
        znM *= 2;
        bottom=0;
        left=0;
        top+=offsetM;
        right+=offsetM;
        break;
        //move to the bottom right area
      case 3:
        xzM = (xzM * 2) + sizeX;
        xnM *= 2;
        yzM = (yzM * 2) + sizeY;
        ynM *= 2;
        zzM = (zzM * 2) - sizeZ;
        znM *= 2;
        top=0;
        left=0;
        bottom+=offsetM;
        right+=offsetM;
        break;
        //move to the bottom left area
      case 4:
        xzM = (xzM * 2) - sizeX;
        xnM *= 2;
        yzM = (yzM * 2) + sizeY;
        ynM *= 2;
        zzM = (zzM * 2) - sizeZ;
        znM *= 2;
        top=0;
        right=0;
        bottom+=offsetM;
        left+=offsetM;
        break;
        // TODO test this
        // move to the top left area
      case 5:
        xzM = (xzM * 2) - sizeX;
        xnM *= 2;
        yzM = (yzM * 2) - sizeY;
        ynM *= 2;
        zzM = (zzM * 2) + sizeZ;
        znM *= 2;
        right=0;
        bottom=0;
        top+=offsetM;
        left+=offsetM;
        break;
        // move to the top right area
      case 6:
        xzM = (xzM * 2) + sizeX;
        xnM *= 2;
        yzM = (yzM * 2) - sizeY;
        ynM *= 2;
        zzM = (zzM * 2) + sizeZ;
        znM *= 2;
        bottom=0;
        left=0;
        top+=offsetM;
        right+=offsetM;
        break;
        //move to the bottom right area
      case 7:
        xzM = (xzM * 2) + sizeX;
        xnM *= 2;
        yzM = (yzM * 2) + sizeY;
        ynM *= 2;
        zzM = (zzM * 2) + sizeZ;
        znM *= 2;
        top=0;
        left=0;
        bottom+=offsetM;
        right+=offsetM;
        break;
        //move to the bottom left area
      case 8:
        xzM = (xzM * 2) - sizeX;
        xnM *= 2;
        yzM = (yzM * 2) + sizeY;
        ynM *= 2;
        zzM = (zzM * 2) + sizeZ;
        znM *= 2;
        top=0;
        right=0;
        bottom+=offsetM;
        left+=offsetM;
        break;
    }
  }

  //calculate the area by rounding around the center
  area.X1 = roundup(xzM - sizeX, xnM) + left;
  area.Y1 = roundup(yzM - sizeY, ynM) + top;
  area.Z1 = roundup(zzM - sizeZ, znM);

  area.X2 = rounddown(xzM + sizeX, xnM) - right;
  area.Y2 = rounddown(yzM + sizeY, ynM) - bottom;
  area.Z2 = rounddown(zzM + sizeZ, znM);
}

void Screen::AreaForJob(uint32_t job, Area<coord_t> &area) const
{
  Cell(job,area);
}

iter_t& Screen::Iteration(iter_t iter)
{
  return screen[iter];
}

iter_t& Screen::Iteration(coord_t x, coord_t y)
{
  uint32_t idx = TransformCoord(x,y);
  return screen[idx];
}

void Screen::PrepareVector(std::vector<iter_t> &iterations) const
{
//  iterations.reserve(oversampling*iversampling);
//  std::fill(iterations.begin(), iterations.end(), IS_BLANK);
  for(uint32_t i=0; i<oversampling*oversampling; i++) {
    iterations.push_back(IS_BLANK);
  }
}

//gives all the Iterations for a pixel (oversampling)
bool Screen::IterationForPixel(coord_t x, coord_t y, std::vector<iter_t> &iterations) const
{
  x *= oversampling;
  y *= oversampling;
  uint32_t idx=0;
  bool quit = false;
  for (uint32_t i=0; i<oversampling; i++) {
    for (uint32_t j=0; j<oversampling; j++) {
      iter_t iter = screen[TransformCoord(x+i,y+j)];
      if(iter == IS_BLANK) {
        quit = true;
        break;
      }
      iterations[idx] = iter;
      idx++;
    }
    if(quit) {
      break;
    }
  }
  return !quit;
}

index_t Screen::TransformCoord(coord_t x, coord_t y) const
{
  return x+y*width;
}

void Screen::TransformIndex(index_t index, coord_t &x, coord_t &y) const
{
  x = index%width;
  y = index/width;
}

index_t Screen::TransformCoord(coord_t x, coord_t y, coord_t z) const
{
  index_t ix = x;
  index_t iy = y;
  index_t iz = z;
  return ix + iy*width*depth + iz*width;
}

void Screen::TransformIndex(index_t index, coord_t &x, coord_t &y, coord_t &z) const
{
  x = index % width;
  y = index / (width*depth);
  z = (index / width) % depth;
}

coord_t Screen::Width() const
{
  return width;
}

coord_t Screen::Height() const
{
  return height;
}

uint32_t Screen::Oversampling() const
{
  return oversampling;
}

Screen::~Screen()
{
  delete[] screen;
}

iter_t* Screen::FullScreen()
{
  return screen;
}
