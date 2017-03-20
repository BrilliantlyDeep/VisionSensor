/*

   The following program detects the rectangles from the video stream, returning its position and
   orientation. It's possible to detect up to two different colours by selecting the wanted ranges
   via a set trackbars.

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

*/

#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "myLib.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
   string s1 = "-DEBUG";
   string s2 = "-SENSING";

   int HowManyColours;

   bool   DEBUG_MODE = false;
   bool SENSING_MODE = false;

   // check what mode the user is adopting.
   if (s1.compare(argv[1]) == 0)
      DEBUG_MODE = true;
   if (s2.compare(argv[1]) == 0)
      SENSING_MODE = true;


   if( argc < 2 || !DEBUG_MODE || !SENSING_MODE)
   {
      cout << "You have to call the program either in -DEBUG mode or -SENSING mode\n";
      cout << "Exiting.\n" ;
      return -1;
   }  

   if (DEBUG_MODE)
      DebugMode();   // argv[1] = -DEBUG => we enter debug mode
   else if (SENSING_MODE)
   {
      // when -SENSING mode gets called we have to make sure that there are 3 arguments
      // and that the 3rd one (argv[2]) is a number
      if ( argc != 3 || atoi(argv[2]) < 0 || atoi(argv[2]) > 9 )
      {
         cout << "Invalid argument when calling -SENSING" << endl;
         cout << "Exiting" << endl;
         return -1;
      }
      
      HowManyColours = atoi(argv[2]);
      SensingMode(HowManyColours);  
   }

   return 0;
}
