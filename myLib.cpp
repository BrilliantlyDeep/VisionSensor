/*
   UdBOT library for the vision sensor. 

   Date: 20th March 2017
   Author: Mattia Iurich
   Version: 1.2

   Version 1.2 is suitable for UNIX based PCs (on which openCV must be installed). The final version will run on a RaspberryPi 3B.
   Changes might be needed (especially for the camera administration).
   Might also work on other Operating systems (I'm not sure).
*/

#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "myLib.h"

using namespace std;
using namespace cv;

HSV** InitialSetup(int N)
{
   cv::VideoCapture capture;
   cv::Mat camera, cameraHSV, FilteredImage;
   HSV min[N], max[N];   // create a vector of paramters for the filters.

   // Allocate a 2xN HSV Matrix
   HSV** bars = (HSV **)malloc(2*sizeof(HSV*));   // 2 rows allocation
   for (int i = 0; i < N; i++)                    // N columns allocation
      bars[i] = (HSV *)malloc(N*sizeof(HSV));

   for(int n = 0; n < N; n++)   // Fill the parameters with initial condition (0, 0, 0) - (179, 255, 255)
   {   
      min[n].hue = 0;
      min[n].sat = 0;
      min[n].val = 0;
      max[n].hue = MAX_HUE;
      max[n].sat = MAX_SAT;
      max[n].val = MAX_VAL;
   }

   // Open the camera process and set the width/height
   capture.open(0);

   capture.set(CV_CAP_PROP_FRAME_WIDTH,   FRAME_WIDTH);
   capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);


   for (int i = 0; i < N; i++)
   {
      // Create trackbars for the HSV filtering
      createTrackbarsForHSVSel( &(min[i]), &(max[i]) );

      cout << "Press n to skip to the next filter.\n";

      while ( (char)cv::waitKey(30) != 'n' )   // execute the filtering untill the user presses 'n'
      {   
         capture.read(camera);   // store the image on camera
         cvtColor(camera, cameraHSV, CV_BGR2HSV);   // cameraHSV = HSV_trasformation(camera)
         // Create binary of pixels such that: minHSV < pixel < maxHSV. Save it in "FilteredImage"
         inRange(cameraHSV, Scalar(min[i].hue, min[i].sat, min[i].val), Scalar(max[i].hue, max[i].sat, max[i].val), FilteredImage);
         morphOps(FilteredImage);   // morphological operations: they allow to close the 'holes' and delete the 'dots'

         // Show the results
         imshow("Original", camera);
         imshow("Filtered", FilteredImage);
      }

      bars[0][i] = min[i]; bars[1][i] = max[i];
      
   }

   return bars;
}


void DebugMode()
{
   // Matrices for images
   cv::Mat src, hsvSpace, threshold, edges;
   
   // HSV classes for the trackbars
   HSV min, max;

   min.hue = min.sat = min.val = 0;
   max.hue = MAX_HUE;  max.sat = MAX_SAT;  max.val = MAX_VAL;

   int LOW_THRESHOLD = 0;
   int HIGH_THRESHOLD = 255;

   std::vector<std::vector<cv::Point> > contours;   // Vectors for the contours storage
   std::vector<cv::Vec4i> hierarchy;
   std::vector<cv::Rect> boundRect;

   createTrackbarsForHSVSel(&min, &max);   // create trackbars for the HSV palette
   cv::createTrackbar("Min Threshold", "Trackbars", &LOW_THRESHOLD , HIGH_THRESHOLD);
   cv::createTrackbar("Max Threshold", "Trackbars", &HIGH_THRESHOLD, HIGH_THRESHOLD);
   
   cv::VideoCapture capture;
   capture.open(0);

   capture.set(CV_CAP_PROP_FRAME_WIDTH , FRAME_WIDTH);
   capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

   while(true)   // loop exectues as long as the user doesn't press q
   {
      capture.read(src);   // read from camera
      cvtColor(src, hsvSpace, CV_BGR2HSV);   // RGB to HSV color space transforcv::Mation
      // create a binary such that 1s are between cv::Scalar(min_, min_, min_) and cv::Scalar(max_, max_, max_)
      inRange(hsvSpace, cv::Scalar(min.hue, min.sat, min.val), cv::Scalar(max.hue, max.sat, max.val), threshold);
      morphOps(threshold);   // morphological operations: they allow to close the 'hole' and delete the 'dots'

      // threshold now contains the binary that only displays one colour (if the trackbars are set correctly)

      // Apply Gaussian blurring and Canny edge algorithm for the edge detection
      // Kernel = 3x3, Sigmas are calculated autocv::Matically (see 'getGaussianKernel()')
      GaussianBlur(threshold, threshold, cv::Size(3,3), 0, 0);
      Canny(threshold, edges, LOW_THRESHOLD, HIGH_THRESHOLD);

      // Transfer the edges from Canny to findContours (so that I have a std::vector<std::vector<cv::Point> > type of variable)
      findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));

      /*
         Algorithm that approxicv::Mates the edges of the figure to a rectangle.
         After that it needs to be able to calculate the rectangle position and orientation
         (will something like cv::RotatedRect be useful?)
      */

      // Few tries with that algorithm
      findAndDrawRect(contours, edges.size());

      // Show images
      cv::imshow("Camera feed", src);
      cv::imshow("Thresholded", threshold);
      cv::imshow("Edges", edges);

      if((char)cv::waitKey(30) == 'q')
         return;
   }

   return ;
}


void SensingMode(int HowManyColours)
{
   cv::Mat src, filter[HowManyColours];   // source image and filtered ones
   // vector <Mat > filter;  <== this might be better
   VideoCapture capture;                  // VideoCapture object to get frames from camera
   
   char input;
   bool CORRECT_SETUP = false;

   // Filter[i] will be the i-th filtered (binary) image. All the objects of such colour will
   // be visible here.
   
   // Allocate a 2xN HSV Matrix
   HSV** FiltersParams = (HSV **)malloc(2*sizeof(HSV*));       // 2 rows allocation
   for (int i = 0; i < HowManyColours; i++)                    // N columns allocation
      FiltersParams[i] = (HSV *)malloc(HowManyColours*sizeof(HSV));

   do
   {
      FiltersParams = InitialSetup(HowManyColours);   // FilterParams[0][i] contains the i-th minimum
                                                      // FilterParams[1][i] contains the i-th maximum
      do
      {
         cout << "Are the filters setup correctly? [y/n] " ;
         cin >> input;

         if (input != 'y' && input != 'Y' && input != 'n' && input != 'N')
         {
            cout << "Reply with y/Y or n/N." << endl;
            CORRECT_SETUP = false;
         }
         else if (input == 'y' || input == 'Y')
            CORRECT_SETUP = true;
         else if (input == 'n' || input == 'N')
            CORRECT_SETUP = false;
      } while ( input != 'y' && input != 'Y' && input != 'n' && input != 'N' );

   } while( !CORRECT_SETUP );

   // Camera feed setup
   capture.open(0);

   if ( !capture.isOpened() )
   {
      cout << "Not able to detect a camera or the object is not working correctly." << endl;
      cout << "Exiting." << endl;

      return;
   }

   for( int i = 0; i < HowManyColours; i++ )
   {
      // Detect everything we can in the i-th filtered image
      
   }

   return;
}


void createTrackbarsForHSVSel(HSV* min, HSV* max)
{
   cv::namedWindow("Trackbars", CV_WINDOW_AUTOSIZE);

   cv::createTrackbar("Low  hue", "Trackbars", &(min->hue), MAX_HUE );
   cv::createTrackbar("High hue", "Trackbars", &(max->hue), MAX_HUE );
   cv::createTrackbar("Low  sat", "Trackbars", &(min->sat), MAX_SAT );
   cv::createTrackbar("High sat", "Trackbars", &(max->sat), MAX_SAT );
   cv::createTrackbar("Low  val", "Trackbars", &(min->val), MAX_VAL );
   cv::createTrackbar("High val", "Trackbars", &(max->val), MAX_VAL );

   return;
}


void findAndDrawRect(std::vector<std::vector<cv::Point> > contours, cv::Size drawingSize )
{
   std::vector<cv::RotatedRect> minRect(contours.size());
   cv::Mat drawing = cv::Mat::zeros(drawingSize, CV_8UC3);
   cv::Point2f vertices[4];

   for (int k = 0; k < contours.size(); k++)
      minRect[k] = minAreaRect( cv::Mat (contours[k]) );

   // Drawing
   for(int k = 0; k < contours.size(); k++)
   {
      minRect[k].points(vertices);
      for (int j = 0; j < 4; j++)
         line(drawing, vertices[j], vertices[(j + 1)%4], cv::Scalar(0, 0, 255), 1, 8);
   }

   cv::namedWindow("Min rect", CV_WINDOW_AUTOSIZE);
   cv::imshow("Min rect", drawing);

   return;
}

void morphOps(Mat &thresh)
{
   // create structuring element that will be used to "dilate" and "erode" image.
   // the element chosen here is a 3px by 3px rectangle.
   // As a rule of thumb you want to dilate with larger element to make sure the object is nicely visible
   // but I actually found that often time this is not the case.

   erode (thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));
   dilate(thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));

   dilate(thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));
   erode (thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));

   return ; 
}