#include "BViewPort.h"

BViewPort::BViewPort() {
  // initialize ILI9341
  mWorldX = mWorldY = 0;
  mOffsetX = mOffsetY = 0;
}

BViewPort::~BViewPort() {
  // release any resources
}
