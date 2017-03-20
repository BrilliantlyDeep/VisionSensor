#include <opencv/highgui.h>
#include <opencv/cv.h>

#define  FRAME_WIDTH  640
#define FRAME_HEIGHT  480

#define MAX_HUE  179
#define MAX_SAT  255
#define MAX_VAL  255 


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
