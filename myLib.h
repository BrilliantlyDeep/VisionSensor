#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "object.h"

#define  FRAME_WIDTH  640
#define FRAME_HEIGHT  480

#define MAX_HUE  179
#define MAX_SAT  255
#define MAX_VAL  255 

#define MAX_NUM_OBJECTS 50
#define MIN_OBJECT_AREA 20*20


// Type definitions and functions prototypes
struct HSV
{
   int hue, sat, val;
};

void morphOps(cv::Mat &thresh);
HSV** InitialSetup(int N);
void DebugMode();
void SensingMode(int HowManyColours);
void createTrackbarsForHSVSel(HSV* min, HSV* max);
void findAndDrawRect(std::vector<std::vector<cv::Point> >, cv::Size);
vector<Object> analyzeContours(Mat image);
void DrawObecjtCenter(Mat &image, Object object);