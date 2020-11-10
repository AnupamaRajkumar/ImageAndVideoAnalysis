/*
Detection of circular object by edge detection and Hough transform for circles
*/

#include "Hough.h"


int main(int argc, char** argv)
{
	if (argc < 2) {
		cerr << "Image not found" << endl;
		return EXIT_FAILURE;
	}
	Mat img = imread(argv[1]);
	int rmin, rmax;
	rmin = rmax = 0;
	cout << "Enter the range of radius ie rmin and rmax" << endl;
	cin >> rmin >> rmax;
	houghTrm hough(img, rmin, rmax);
	hough.DetectCircles();
	waitKey(0);
	return 1;
}


