#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace cv;

int main(){
	VideoCapture cap(0);
	if (!cap.isOpened()) {//摄像头未被正确调用
		std::cout << "!!!";
		return -1;
	}
	Mat frame;
	bool judge = true;
	namedWindow("call");
	int i = 0;
	while (judge) {
		cap >> frame;
		if (frame.empty()) break;
		
		String s = "call" ;
		imshow(s, frame);
		if (27 == waitKey(30)) {
			break;
		}

		
	}
	destroyWindow("call");
	return 0;
}