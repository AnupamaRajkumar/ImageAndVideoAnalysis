#pragma once
#ifndef __MOTIONDET__
#define __MOTIONDET__

#define _USE_MATH_DEFINES
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <cmath>

using namespace std;
using namespace cv;

class MotionDetection {
public:
	MotionDetection(char* fileName);
private:
	void MotionDetectionMenu();
	void MotionTracker();
	void DenseOpticalFlow();
	char* fileName;

};
#endif // !__MOTIONDET__

