#include "Hough.h"

/*Constructor*/
houghTrm::houghTrm(Mat& img, int rmin, int rmax) {
	this->img  = img;
	this->rmin = rmin;
	this->rmax = rmax;
}

/*Function for edge detection*/
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

/*Function for Hough Circle Detection operations*/
void houghTrm::DetectCircles() {
	int choice = 1;
	Mat edgeImg;
	map<list<int>, int> accumulator;
	cout << "Hough Circle transform menu:" << endl;
	cout << "1. Our Hough Implementation" << endl;
	cout << "2. OpenCV Hough Implementation" << endl;
	cout << "Please enter your choice (1/2)" << endl;
	cin >> choice;
	switch (choice) {
	case 1:
		edgeImg = this->EdgeDetector();
		this->HoughTransform(edgeImg, accumulator);
		break;
	case 2:
		edgeImg = this->EdgeDetector();
		this->HoughTransformOpenCV(edgeImg);
		break;
	default:
		cout << "Enter valid choice" << endl;
		break;
	}	
}

/*Perform Hough Transform*/
void houghTrm::HoughTransform(Mat& edgeImg, map<list<int>, int>& accumulator) {
	vector<Point2i> points;
	Mat HoughSpace = Mat::zeros(img.size(), CV_8UC1);
	int stepSize = 1;
	/*detect edge points from the edge image*/
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

				int a = points[p].x - int(r * cos(theta * CV_PI / 180));
				int b = points[p].y - int(r * sin(theta * CV_PI / 180));
				
				list<int> triple;
				triple.push_back(a);	//a --> x coordinate but a row index
				triple.push_back(b);	//b --> y coordinate but a column index
				triple.push_back(r);	//r --> radius
				
				map <list<int>, int>::iterator it = accumulator.find(triple);
				/*accumulator is incremented if (a,b) is at distance r from x ie cast vote*/
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
	
	/*Hough space*/
	for (auto& it : accumulator) {
		list<int> val = it.first;
		int a = val.front();
		val.pop_front();
		int b = val.front();
		//cout << a << " " << b << endl;
		if(a >= 0 && a < img.rows && b >= 0 && b < img.cols)
			HoughSpace.at<uchar>(a, b) = it.second;
	}

	imshow("Hough Space", HoughSpace);
	imwrite("HoughSpace.png", HoughSpace);

	/*Draw the detected circles*/
	this->DrawCircles(accumulator);
}

/*Draw the detected circles*/
void houghTrm::DrawCircles(map<list<int>, int>& accumulator) {

	int step = 360;
	float threshold = 0.35;				//decide how many min points needed to decide if a circle is valid or not
	map<list<int>, int>::iterator it;
	vector<Vec3i> circles;
	/*if a center pair (a, b) with radius r is very frequenty found and beyond the set threshold value
	there is a probability that circle with radius r at center (a, b) exists*/
	for (it = accumulator.begin(); it != accumulator.end(); it++) {

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
		/*the x and y coordinate is flipped here because x in cartesian forms column in 
		image matrix and y in cartesian coordinate forms row in image matrix */
		Point center(circles[i][1], circles[i][0]);
		int radius = circles[i][2];
		/*draw circle center*/
		circle(img, center, 1, Scalar(0, 255, 0), -1, 4, 0);
		/*draw circle*/
		circle(img, center, radius, Scalar(0, 0, 255), 1, 8, 0);
	}
	imshow("Detected Circles", img);
	imwrite("DetectedCircles.png", img);
}

/*Hough transform opencv*/
void houghTrm::HoughTransformOpenCV(Mat& edgeImg) {
	vector<Vec3f> circles;
	HoughCircles(edgeImg, circles, HOUGH_GRADIENT, 1,
		edgeImg.rows/8,							// change this value to detect circles with different distances to each other
		255, 3, rmin, rmax);		// change the last two parameters
									// (min_radius & max_radius) to detect larger circles


	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		Point center = Point(c[0], c[1]);
		// circle center
		circle(img, center, 1, Scalar(0, 100, 100), 1, LINE_AA);
		// circle outline
		int radius = c[2];
		circle(img, center, radius, Scalar(255, 0, 255), 1, LINE_AA);
	}
	imshow("Dectected Circles OpenCV", img);
	imwrite("DetectedCirclesOpenCV.png", img);
}