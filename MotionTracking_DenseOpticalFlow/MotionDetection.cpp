#include "MotionDetection.h"

MotionDetection::MotionDetection(char* fileName) {
	this->fileName = fileName;
	this->MotionTracker();
	this->DenseOpticalFlow();
}

/*********************************
Motion tracking menu
**********************************/
void MotionDetection::MotionDetectionMenu() {
	int choice = 1;
	cout << "Motion Detection Menu" << endl;
	cout << "1. Motion Tracker" << endl;
	cout << "2. Dense optical flow" << endl;
	cout << "Enter your choice (1/2)" << endl;
	cin >> choice;
	switch (choice)
	{
		case 1:
			this->MotionTracker();
			break;
		case 2:
			this->DenseOpticalFlow();
			break;
		default:
			break;
	}
}

/*************************************************************
OpenCV Motion Tracker
OpenCV sparse optical flow using calcOpticalFlowPyrLK()
Lukas Kanade algorithm, good points are detected using
goodFeaturesToTrack()
**************************************************************/

void MotionDetection::MotionTracker() {
	/*Reducing frame rate of the input video*/
	VideoCapture capture(this->fileName);
	int frame_width = capture.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CAP_PROP_FRAME_HEIGHT);
	bool ret = true;
	vector<Mat> frames;
	cout << "Reducing frame rate of the original video" << endl;
	while (true) {
		Mat frame;
		ret = capture.read(frame);
		if (ret) {
			frames.push_back(frame);
		}
		else
			break;
	}
	VideoWriter slowFr("slowVid.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 2, Size(frame_width, frame_height));
	for (int f = 0; f < frames.size(); f++) {
		slowFr.write(frames[f]);
	}
	slowFr.release();

	/*Reading from the slowed down video*/
	VideoCapture slowCap("slowVid.avi");

	/*Store the motion tracked video*/
	VideoWriter output("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 2, Size(frame_width, frame_height));
	if (!slowCap.isOpened()) {
		//Error in opening the video input
		cerr << "Unable to open the video file!" << endl;
		exit(0);
	}
	// Create some colors
	vector<Scalar> colors;
	RNG rng;
	for (int i = 0; i < 100; i++)
	{
		int r = 0;		
		int g = 255;	
		int b = 0;		
		colors.push_back(Scalar(r, g, b));
	}
	Mat old_frame, old_gray;
	vector<Point2f> p0, p1;
	// Take first frame and find corners in it
	slowCap >> old_frame;
	cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
	goodFeaturesToTrack(old_gray, p0, 10000, 0.2, 7, Mat(), 7, false, 0.04);
	// Create a mask image for drawing purposes
	Mat mask = Mat::zeros(old_frame.size(), old_frame.type());
	cout << "**************Motion tracking started!!****************" << endl;
	while (true) {
		Mat frame, frame_gray;
		slowCap >> frame;
		if (frame.empty())
			break;
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		// calculate optical flow
		vector<uchar> status;
		vector<float> err;
		TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 100, 0.03);
		calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15, 15), 2, criteria);
		vector<Point2f> good_new;
		for (uint i = 0; i < p0.size(); i++)
		{
			// Select good points
			if (status[i] == 1) {
				good_new.push_back(p1[i]);
				// draw the tracks
				line(mask, p1[i], p0[i], colors[i], 2);
				circle(frame, p0[i], 5, colors[i], -1);
				circle(frame, p1[i], 5, colors[i], -1);
			}
		}
		Mat img;
		add(frame, mask, img);
		output.write(img);
		// Now update the previous frame and previous points
		old_gray = frame_gray.clone();
		p0 = good_new;
	}
	output.release();
	cout << "**************Motion tracking completed!!****************" << endl;
}

/********************************************************************
OpenCV Dense Optical Flow using Farneback algorothm
calcOpticalFlowFarneback()
********************************************************************/

void MotionDetection::DenseOpticalFlow() {
	/*Reducing frame rate of the input video*/
	VideoCapture capture(this->fileName);
	int frame_width = capture.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CAP_PROP_FRAME_HEIGHT);
	bool ret = true;
	vector<Mat> frames;
	cout << "Reducing frame rate of the original video" << endl;
	while (true) {
		Mat frame;
		ret = capture.read(frame);
		if (ret) {
			frames.push_back(frame);
		}
		else
			break;
	}
	VideoWriter slowFr("slowVid.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 2, Size(frame_width, frame_height));
	for (int f = 0; f < frames.size(); f++) {
		slowFr.write(frames[f]);
	}
	slowFr.release();

	/*Reading from the slowed down video*/
	VideoCapture slowCap("slowVid.avi");

	/*Store the motion tracked video*/
	VideoWriter output("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 2, Size(frame_width, frame_height));
	if (!slowCap.isOpened()) {
		//Error in opening the video input
		cerr << "Unable to open the video file!" << endl;
		exit(0);
	}
	// Create some random colors
	vector<Scalar> colors;
	RNG rng;
	for (int i = 0; i < 100; i++)
	{
		int r = 0;
		int g = 255;
		int b = 0;
		colors.push_back(Scalar(r, g, b));
	}
	Mat frame1, prvs;
	slowCap >> frame1;
	cvtColor(frame1, prvs, COLOR_BGR2GRAY);
	
	// Create a mask image for drawing purposes
	cout << "**************Optical Flow started!!****************" << endl;
	while (true) {
		Mat frame2, next, diff;
		Mat mask = Mat::zeros(frame1.size(), frame1.type());
		slowCap >> frame2;
		if (frame2.empty())
			break;
		cvtColor(frame2, next, COLOR_BGR2GRAY);
		Mat flow(prvs.size(), CV_32FC2);
		calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 25, 5, 5, 1.2, 0);
		// visualization
		Mat flow_parts[2];
		split(flow, flow_parts);
		Mat magnitude, angle, magn_norm;
		cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
		normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
		int scale = 20;
		for (int r = 0; r < magn_norm.rows; r+=20) {
			for (int c = 0; c < magn_norm.cols; c+=20) {
				Point2f p1, p2;
				p1.x = c + magn_norm.at<float>(r, c)*20;
				p1.y = r + magn_norm.at<float>(r, c)*20;
				p2.x = c + magn_norm.at<float>(r, c) * sin(angle.at<float>(r, c)*M_PI / 180.)*scale;
				p2.y = r + magn_norm.at<float>(r, c) * cos(angle.at<float>(r, c)*M_PI / 180.)*scale;
				arrowedLine(mask, p2, p1, Scalar(0, 255, 0), 2);
			}
		}
		Mat img;
		add(frame2, mask, img);
		//build hsv image
		/*Mat _hsv[3], hsv, hsv8, bgr;
		_hsv[0] = angle;
		_hsv[1] = Mat::ones(angle.size(), CV_32F);
		_hsv[2] = magn_norm;
		merge(_hsv, 3, hsv);
		hsv.convertTo(hsv8, CV_8U, 255.0);
		cvtColor(hsv8, bgr, COLOR_HSV2BGR);*/
		output.write(img);
		prvs = next;
	}
	output.release();
	cout << "**************Optical Flow completed!!****************" << endl;
}
