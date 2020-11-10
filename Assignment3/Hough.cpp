#include "Hough.h"
#include <unordered_map>
#include <list>
#include <iterator>

houghTrm::houghTrm(Mat& img, int rmin, int rmax) {
	this->img  = img;
	this->rmin = rmin;
	this->rmax = rmax;
}

Mat houghTrm::EdgeDetector() {
	Mat edgeImg = Mat::zeros(img.size(), CV_8UC1);
	Mat binImg = edgeImg.clone();

	/*step 1: Filtering to smooth out noise*/
	int kSize = 3;
	Mat kernel = Mat(kSize, kSize, img.type());
	Mat filtImg = Mat::zeros(img.size(), img.type());
	/*converting image to grayscale*/
	if (filtImg.channels() > 1) {
		cvtColor(img, filtImg, COLOR_BGR2GRAY);
	}
	
	GaussianBlur(filtImg, filtImg, kernel.size(), 0, 0);

	/*step 2 : Thresholding*/
	double maxVal = 255.;
	double thresh = 0.;
	threshold(filtImg, binImg, thresh, maxVal, THRESH_BINARY + THRESH_OTSU);

	/*Step 3 : Edge filtering*/
	Canny(binImg, edgeImg, 0, 2, 3);

	imwrite("Edge.png", edgeImg);
	return edgeImg;
}

void houghTrm::DetectCircles() {
	Mat edgeImg = this->EdgeDetector();
	//imshow("EdgeImg", edgeImg);
	map<list<int>, int> accumulator;
	this->HoughTransform(edgeImg, accumulator);
}

void houghTrm::HoughTransform(Mat& edgeImg, map<list<int>, int>& accumulator) {
	vector<Point2i> points;
	Mat HoughSpace = Mat::zeros(edgeImg.size(), CV_8UC1);
	int stepSize = 1;
	for (int row = 0; row < edgeImg.rows; row++) {
		for (int col = 0; col < edgeImg.cols; col++) {
			if (edgeImg.at<uchar>(row, col) > 0) {
				Point2i point = Point2i(row, col);
				points.push_back(point);
			}
		}
	}
	cout << points.size() << endl;
	for (int p = 0; p < points.size(); p++) {
		for (int r = rmin; r <= rmax; r++) {
			for (int theta = 0; theta <= 360; theta = theta + stepSize) {
				//cout << "points:" << points[p].x << " " << points[p].y << endl;
				//cout << theta << endl;
				int a = points[p].x - int(r * cos(theta * CV_PI / 180));
				int b = points[p].y - int(r * sin(theta * CV_PI / 180));
				//accumulator is incremented if (a,b) is at distance r from x ie cast vote
				list<int> triple;
				triple.push_back(a);
				triple.push_back(b);
				triple.push_back(r);
				
				map <list<int>, int>::iterator it = accumulator.find(triple);
				/*if key is already present*/
				if (it != accumulator.end()) {
					it->second++;
				}
				else {
					accumulator.insert(make_pair(triple, 1));
				}
			}
		}
	}
	/*if a center pair (a, b) with radius r is very frequenty found,
	there is a probability that circle with radius r at center (a, b) exists*/
	
	/*Hough space*/
	for (auto& it : accumulator) {
		list<int> val = it.first;
		int a = val.front();
		val.pop_front();
		int b = val.front();
		//cout << a << " " << b << endl;
		if(a >= 0 && b >= 0)
			HoughSpace.at<uchar>(a, b) = it.second;
	}
	Mat EqualiseHough;
	equalizeHist(HoughSpace, EqualiseHough);

	this->DrawCircles(accumulator);

	imshow("Hough Space", EqualiseHough);
	imwrite("HoughSpace.png", EqualiseHough);
}

void houghTrm::DrawCircles(map<list<int>, int>& accumulator) {

	int step = 360;
	float threshold = 0.2;
	map<list<int>, int>::iterator it;
	vector<Vec3i> circles;
	for (it = accumulator.begin(); it != accumulator.end(); it++) {
		//float val = static_cast<float>(it->second / step);
		//cout << val << endl;
		if ((static_cast<float>(it->second) / static_cast<float>(step)) > threshold) {
			Vec3i circParams;
			list<int> val = it->first;
			/*x coord - center*/
			circParams[0] = val.front();
			val.pop_front();
			/*y coord - center*/
			circParams[1] = val.front();
			/*radius*/
			val.pop_front();
			circParams[2] = val.front();
			circles.push_back(circParams);
		}
	}
	cout << circles.size() << endl;
	/*draw circles*/
	for (int i = 0; i < circles.size(); i++) {
		Point center(circles[i][0], circles[i][1]);
		int radius = circles[i][2];
		/*draw circle center*/
		circle(img, center, 1, Scalar(0, 255, 0), -1, 4, 0);
		/*draw circle*/
		//cout << radius << endl;
		circle(img, center, radius, Scalar(0, 0, 255), 1, 8, 0);
	}
	imshow("Detected Circles", img);
	imwrite("DetectedCircles.png", img);
}