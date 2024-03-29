/*
author: Forec
last edit date: 2016/10/06
email: forec@bupt.edu.cn
LICENSE
Copyright (c) 2015-2017, Forec <forec@bupt.edu.cn>

Permission to use, copy, modify, and/or distribute this code for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef __MONITOR_H_
#define __MONITOR_H_

//opencv


#include <opencv2/core/core.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//C++
#include <ctime>
#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;


extern int keyboard;

const float h_ranges[] = { 0, 256 };
const float s_ranges[] = { 0, 180 };
const float* ranges[] = { h_ranges, s_ranges };

const int h_bins = 50, s_bins = 60;
const int histSize[] = { h_bins, s_bins };

extern char recordName[128];

void processCamera(bool showWindow,
	unsigned int method,
	unsigned int unnormal = 10,
	unsigned int fps = 24) {
	VideoCapture capture(0);
	if (!capture.isOpened()) {
		cerr << "Unable to open camera " << endl;
		exit(EXIT_FAILURE);
	}

	bool backGroundFlag = true, recordFlag = false;
	Mat frame, fgMask;					// current frame, fg mask
	Mat HSV;							// HSV format
	MatND base, cur;					// histogram
	unsigned int UnnormalFrames = 0;
	int channels[] = { 0, 1 };



	

	// ESC or 'q' for quitting
	while ((char)keyboard != 'q' && (char)keyboard != 27) {
		if (!capture.isOpened()) {
			cerr << "Unable to read camera" << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}


		Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 Background subtractor
		Ptr<BackgroundSubtractor> pMOG = createBackgroundSubtractorMOG2();; //MOG Background subtractor

		capture>>frame;
		if (method == 0)
			pMOG2->apply(frame, fgMask);
		else if (method == 1)
			pMOG->apply(frame, fgMask);
		else if (method == 2)
			fgMask = frame;
		imshow("Monitor", frame);
		imshow("MonitorMask", fgMask);

		// set background
		if (backGroundFlag) {
			cv::cvtColor(frame, HSV, COLOR_BGR2HSV);
			cv::calcHist(&HSV, 1, channels, Mat(), base, 2, histSize, ranges, true, false);
			cv::normalize(base, base, 0, 1, NORM_MINMAX, -1, Mat());
			backGroundFlag = false;
		}

		cv::cvtColor(frame, HSV, COLOR_BGR2HSV);
		cv::calcHist(&HSV, 1, channels, Mat(), cur, 2, histSize, ranges, true, false);
		cv::normalize(cur, cur, 0, 1, NORM_MINMAX, -1, Mat());

		double comp = compareHist(base, cur, 0);
		if (comp < 0.65)
			UnnormalFrames += 1;
		else if (UnnormalFrames > 0)
			UnnormalFrames--;
		if (UnnormalFrames > unnormal)
			recordFlag = true;
		else if (UnnormalFrames <= 0) {
			UnnormalFrames = 0;
			recordFlag = false;
		}
		// DO SOMETHING WARNING
		// Here We Starting Recoding
		if (recordFlag) {
			VideoWriter recorder("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 1, cv::Size(frame.cols, frame.rows));
		
		}

		if (showWindow && !frame.empty()) {
			imshow("Monitor", frame);
		}
		keyboard = waitKey(30);
	}

}

#endif __MONITOR_H_
