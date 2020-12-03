/******************************************************************
Task 1 : Points tracked in a video sequence
Task 2 : Optical flow of the sequence
*******************************************************************/

#include "MotionDetection.h"

int main(int argc, char** argv)
{
	if (argc < 2){
		cerr << "Enter the video file name" << endl;
		exit(0);
	}

	MotionDetection detection(argv[1]);
	waitKey(0);
	return 0;
}


