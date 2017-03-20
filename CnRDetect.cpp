/*

   The following program detects the rectangles from the video stream, returning its position and
   orientation. It's possible to detect up to two different colours by selecting the wanted ranges
   via a set trackbars.

   It's possible to enter in DEBUG mode by calling './CnRDetect -DEBUG'. This will show the result
   of the filtering thanks to 3 replica of the camera feed containing respectively the binary
   thresholded image (where it's possible to see the effect of the colour filtering), the edges of the
   colour filtered object done via Canny edge algorithm and the rectangle that approximates such obj.

   If the DEBUG mode is not called, the program must be launched with the -SENSING flag by calling
   './CnRDetect -SENSING'. This mode will allow an initial setup to adjust the two colour filters and,
   once the said operation is terminated with the aid of the screen, clicking 's' or 'S' will start the
   computing. The sensor will send the position of two vertices and orientation of the found objects. 
   Clicking 'q' or 'Q' will terminate the program completely.

   Date: 12th Mar 2017
   Author: Mattia Iurich
   Version: 1.0

*/

#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>

using namespace std;
using namespace cv;

#define  FRAME_WIDTH  640
#define FRAME_HEIGHT  480

#define MAX_HUE  255
#define MAX_SAT  255
#define MAX_VAL  255

// Type definitions and functions prototypes
struct HSV
{
   int hue, sat, val;
};

void morphOps(Mat &thresh);
HSV** InitialSetup();
void DebugMode();
void SensingMode();
void createTrackbarsForHSVSel();
void findAndDrawRect(vector<vector<Point> >, Size);


int main(int argc, char** argv)
{
   string s1 = "-DEBUG";
   string s2 = "-SENSING";

   bool DEBUG_MODE, SENSING_MODE;

   // Check what mode the user is adopting.
   if( argc != 2 || (s1.compare(argv[1]) != 0) && (s2.compare(argv[1]) != 0 ) )
   {
      cout << "You have to call the program either in -DEBUG mode or -SENSING mode\n";
      cout << "Exiting.\n" ;
      return -1;
   }  

   if (s1.compare(argv[1]) == 0)
      DebugMode();
   else
      SensingMode();  

   return 0;
}

// =====================================================================================
// Function declarations

HSV** InitialSetup()
{
   VideoCapture capture;

   Mat camera, cameraHSV, cameraThreshold1, cameraThreshold2;

   HSV min1, min2, max1, max2;

   min1.hue = min2.hue = 0;
   min1.sat = min2.sat = 0;
   min1.val = min2.val = 0;
   max1.hue = max2.hue = 255;
   max1.sat = max2.sat = 255;
   max1.val = max2.val = 255;

   // Allocate a 2x2 HSV matrix
   HSV** bars = (HSV **)malloc(2*sizeof(HSV*));
   for (int i = 0; i < 2; i++)
      bars[i] = (HSV *)malloc(2*sizeof(HSV));

   namedWindow("Trackbar for filter 1", CV_WINDOW_AUTOSIZE);
   namedWindow("Trackbar for filter 2", CV_WINDOW_AUTOSIZE);

   createTrackbar("Low hue 1" , "Trackbar for filter 1", &min1.hue, MAX_HUE);
   createTrackbar("High hue 1", "Trackbar for filter 1", &max1.hue, MAX_HUE);
   createTrackbar("Low sat 1" , "Trackbar for filter 1", &min1.sat, MAX_SAT);
   createTrackbar("High sat 1", "Trackbar for filter 1", &max1.sat, MAX_SAT);
   createTrackbar("Low val 1" , "Trackbar for filter 1", &min1.val, MAX_VAL);
   createTrackbar("High val 1", "Trackbar for filter 1", &max1.val, MAX_VAL);

   createTrackbar("Low hue 2" , "Trackbar for filter 2", &min2.hue, MAX_HUE);
   createTrackbar("High hue 2", "Trackbar for filter 2", &max2.hue, MAX_HUE);
   createTrackbar("Low sat 2" , "Trackbar for filter 2", &min2.sat, MAX_SAT);
   createTrackbar("High sat 2", "Trackbar for filter 2", &max2.sat, MAX_SAT);
   createTrackbar("Low val 2" , "Trackbar for filter 2", &min2.val, MAX_VAL);
   createTrackbar("High val 2", "Trackbar for filter 2", &max2.val, MAX_VAL);

   capture.open(0);

   // Show the thresholded images
   while( (char)waitKey(30) != 's' )
   {
      if(!capture.isOpened())
      {
         cout << "Could not find an available camera. Quitting." << endl;
         return NULL;
      }
      
      capture.read(camera);
      cvtColor(camera, cameraHSV, CV_BGR2HSV);   // Transformation from RGB to HSV colour space
      inRange(cameraHSV, Scalar(min1.hue, min1.sat, min1.val), Scalar(max1.hue, max1.sat, max1.val), cameraThreshold1);
      inRange(cameraHSV, Scalar(min2.hue, min2.sat, min2.val), Scalar(max2.hue, max2.sat, max2.val), cameraThreshold2);
      morphOps(cameraThreshold1);
      morphOps(cameraThreshold2);
      
      imshow("Filter 1", cameraThreshold1);
      imshow("Filter 2", cameraThreshold2);
   }

   capture.release();

   bars[0][0] = min1; bars[0][1] = min2;
   bars[1][0] = max1; bars[1][1] = max2;

   return bars;

}

void DebugMode()
{
   Mat src, hsvSpace, threshold, edges;

   int LOW_THRESHOLD  = 0;
   int HIGH_THRESHOLD = 100;

   vector<vector<Point> > contours;   // Vectors for the contours storage
   vector<Vec4i> hierarchy;
   vector<Rect> boundRect;

   createTrackbarsForHSVSel();   // create trackbars for the HSV palette
   createTrackbar("Min Threshold", "Trackbars", &LOW_THRESHOLD , HIGH_THRESHOLD);
   createTrackbar("Max Threshold", "Trackbars", &HIGH_THRESHOLD, HIGH_THRESHOLD);
   
   VideoCapture capture;
   capture.open(0);

   capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
   capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

   while(true)   // loop exectues as long as the user doesn't press ESC, q or Q
   {
      capture.read(src);   // read from camera
      cvtColor(src, hsvSpace, CV_BGR2HSV);   // RGB to HSV color space transformation
      // create a binary such that 1s are between Scalar(min_, min_, min_) and Scalar(max_, max_, max_)
      inRange(hsvSpace, Scalar(LOW_H, LOW_S, LOW_V), Scalar(HIGH_H, HIGH_S, HIGH_V), threshold);
      morphOps(threshold);   // morphological operations: they allow to close the 'hole' and delete the 'dots'

      // threshold now contains the binary that only displays one colour (if the trackbars are set correctly)

      // Apply Gaussian blurring and Canny edge algorithm for the edge detection
      GaussianBlur(threshold, threshold, Size(3,3), 0, 0);   // Kernel = 3x3, Sigmas are calculated automatically (see 'getGaussianKernel()')
      Canny(threshold, edges, LOW_THRESHOLD, HIGH_THRESHOLD);

      // Transfer the edges from Canny to findContours (so that I have a vector<vector<Point> > type of variable)
      findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

      /*
         Algorithm that approximates the edges of the figure to a rectangle.
         After that it needs to be able to calculate the rectangle position and orientation
         (will something like RotatedRect be useful?)
      */

      // Few tries with that algorithm
      findAndDrawRect(contours, edges.size());

      // Show images
      imshow("Camera feed", src);
      imshow("Thresholded", threshold);
      imshow("Edges", edges);

      if((char)waitKey(30) == 'q')
         return;
   }

   return ;
}

void SensingMode()
{
   Mat src, filter1, filter2;
   HSV FiltersParameters[2][2];

   InitialSetup();

   return;
}

void createTrackbarsForHSVSel()
{
   namedWindow("Trackbars", CV_WINDOW_AUTOSIZE);

   createTrackbar("Low  hue", "Trackbars", &LOW_H , HIGH_H );
   createTrackbar("High hue", "Trackbars", &HIGH_H, HIGH_H );
   createTrackbar("Low  sat", "Trackbars", &LOW_S , HIGH_S );
   createTrackbar("High sat", "Trackbars", &HIGH_S, HIGH_S );
   createTrackbar("Low  val", "Trackbars", &LOW_V , HIGH_V );
   createTrackbar("High val", "Trackbars", &HIGH_V, HIGH_V );

   return;
}

void findAndDrawRect(vector<vector<Point> > contours, Size drawingSize )
{
   vector<RotatedRect> minRect(contours.size());
   Mat drawing = Mat::zeros(drawingSize, CV_8UC3);
   Point2f vertices[4];

   for (int k = 0; k < contours.size(); k++)
      minRect[k] = minAreaRect( Mat (contours[k]) );

   // Drawing
   for(int k = 0; k < contours.size(); k++)
   {
      minRect[k].points(vertices);
      for (int j = 0; j < 4; j++)
         line(drawing, vertices[j], vertices[(j + 1)%4], Scalar(0, 0, 255), 1, 8);
   }

   namedWindow("Min rect", CV_WINDOW_AUTOSIZE);
   imshow("Min rect", drawing);

   return;
}

void morphOps(Mat &thresh)
{
	// create structuring element that will be used to "dilate" and "erode" image.
	// the element chosen here is a 3px by 3px rectangle.
        // As a rule of thumb you want to dilate with larger element to make sure the object is nicely visible

	erode (thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));
	erode (thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));

	dilate(thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));
        dilate(thresh,thresh,getStructuringElement( MORPH_RECT, Size(3,3)));

	return ;	
}

