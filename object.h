/*
   Yet to be completed!
*/

#ifndef OBJECT_H
#define OBJECT_H


#include <string>
#include <opencv/highgui.h>
#include <opencv/cv.h>

using namespace std;
using namespace cv;

class Object
{
   public: 
      Object(void);
      ~Object(void);

      int getXCenter();
      int getYCenter();

      void setXCenter(int x);
      void setYCenter(int y);

      Scalar getAvgColour();
      void setAvgColour(Scalar min, Scalar max);

   private:
      int corners;
      int xCenter, yCenter;
      Scalar AvgColour;
};

#endif
