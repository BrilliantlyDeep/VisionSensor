/*

   The following program detects the rectangles from the video stream, returning its position and
   orientation. The user can decide how many colours he wants to detect.

   It's possible to enter in DEBUG mode by calling './CnRDetect -DEBUG'. This will show the result
   of the filtering thanks to 3 replica of the camera feed containing respectively the binary
   thresholded image (where it's possible to see the effect of the colour filtering), the edges of the
   colour filtered object done via Canny edge algorithm and the rectangle that approximates such obj.

   If the DEBUG mode is not called, the program must be launched with the -SENSING flag by calling
   './CnRDetect -SENSING N' where 'N' is the number of filters the user wants to setup.
   This mode will allow an initial setup to adjust the colour filters and, once the said operation is terminated
   with the aid of the screen, clicking 's' or 'S' will start the computing.
   The sensor will send the position of two vertices and orientation of the found objects. Clicking 'q' or 'Q'
   will terminate the program completely.

   Date: 16th Mar 2017
   Author: Mattia Iurich
   Version: 1.2

   In ver 1.1 some adjustments have been made in order to make the code look better.

   In ver 1.2 the possibility to select more then 2 desidered colours is added. Version 1.1 still isn't 100% working
   because of some "variables issues". 
   18th Mar 2017: when the function is called by terminal and no argument is passed, a segmentation
   fault error appears. Still trying to figure out why. FIXED (on 20th March).
   Still changes and adjustaments are needed.
   24th Mar 2017: software works pretty well more or less. There's still room for improvement in the object.cpp file. 
   A function that returns the averege colour of the pointed object should be implemented asap.

*/

#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "myLib.h"

using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{
   int HowManyColours;

   // check what mode the user is adopting.
   if (argc < 2 || ( strcmp(argv[1], "-DEBUG") != 0 && strcmp(argv[1], "-SENSING") != 0 ) )
   {
      cout << "You have to call the program either in -DEBUG or -SENSING mode\n";
      cout << "Exiting.\n";
      return -1;
   }
   if ( strcmp(argv[1], "-DEBUG") == 0 )
   {   
      DebugMode();   // argv[1] = -DEBUG => we enter debug mode
   }

   else if ( strcmp(argv[1],"-SENSING") == 0 )
   {
      if (argc == 2)
      {
         cout << "Declare the number of filters you want to set up\n";
         cout << "Exiting.\n";
         return -1;
      }
      if (atoi( argv[2]) <= 0)
      {
         cout << "Something's wrong! Oops\n";
         return -1;
      }
      
      HowManyColours = atoi(argv[2]);
      SensingMode(HowManyColours);
   }


   return 0;
}
