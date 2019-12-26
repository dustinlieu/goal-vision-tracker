#include "KinectDevice.hpp"

#include <vector>
#include <cmath>

const double FOCAL_LENGTH_MM = 6.1;
const double GOAL_HEIGHT_MM = 304.8;
const double IMAGE_HEIGHT_PX = 480;
const double SENSOR_HEIGHT_MM = 5.3248;

bool sortX(cv::Point p1, cv::Point p2) {
	return p1.x > p2.x;
}

void sortCorners(std::vector<cv::Point> &approx, std::vector<cv::Point2f> &output) {
	std::vector<cv::Point2f> corners;

	std::sort(approx.begin(), approx.end(), sortX);
	approx.erase(approx.begin() + 2, approx.end() - 2);

	for (size_t i = 0; i < approx.size(); ++i) {
		corners.push_back(cv::Point2f(approx[i].x, approx[i].y));
	}

	cv::Point2f center(0, 0);

	for (size_t i = 0; i < corners.size(); ++i) {
		center += corners[i];
	}

	center *= (1.0 / corners.size());

	std::vector<cv::Point2f> top, bot;

	for (size_t i = 0; i < corners.size(); i++) {
		if (corners[i].y < center.y) {
			top.push_back(corners[i]);
		} else {
			bot.push_back(corners[i]);
		}
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	output.push_back(tl);
	output.push_back(tr);
	output.push_back(br);
	output.push_back(bl);
}

double normalDist(cv::Point2f a, cv::Point2f b) {
	cv::Point2f diff = a - b;
	return std::sqrt((diff.x * diff.x) + (diff.y * diff.y));
}

double distToSideHeight(double height) {
	double calculatedDistance = (((FOCAL_LENGTH_MM * GOAL_HEIGHT_MM * IMAGE_HEIGHT_PX) / (height * SENSOR_HEIGHT_MM)) *
								 0.039370);
	return calculatedDistance * 1.0442349529;
}

KinectDevice::KinectDevice(freenect_context* context, int index)
		: Freenect::FreenectDevice(context, index), m_IRNewFrame(false),
		  m_IRMat(cv::Size(640, 480), CV_8UC3, cv::Scalar(0)) {
	setVideoFormat(FREENECT_VIDEO_IR_8BIT);
	pthread_mutex_init(&m_IRMutex, NULL);
}

void KinectDevice::VideoCallback(void* data, uint32_t timestamp) {
	pthread_mutex_lock(&m_IRMutex);
	m_IRMat = cv::Mat(cv::Size(640, 480), CV_8UC1, data, 640);
	m_IRNewFrame = true;
	pthread_mutex_unlock(&m_IRMutex);
};

bool KinectDevice::GetVideo(cv::Mat &output) {
	if (!m_IRNewFrame) {
		return false;
	}

	pthread_mutex_lock(&m_IRMutex);
	m_IRNewFrame = false;
	m_IRMat.copyTo(output);
	pthread_mutex_unlock(&m_IRMutex);

	return true;
}

std::shared_ptr<KinectDevice::GoalData> KinectDevice::GetGoalData() {
	std::shared_ptr<KinectDevice::GoalData> data(new KinectDevice::GoalData());

	cv::Mat src;
	cv::Mat src_bw;
	cv::Mat output;

	if (!GetVideo(src)) {
		return data;
	}

	cv::cvtColor(src, output, CV_GRAY2RGB);

	cv::GaussianBlur(src, src, cv::Size(3, 3), 0);
	cv::threshold(src, src, 100, 255, CV_THRESH_BINARY);

	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(src.clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1);

	std::vector<cv::Point> approx;
	std::vector<cv::Point2f> corners;

	for (size_t x = 0; x < contours.size(); ++x) {
		approx.clear();
		corners.clear();

		cv::Rect boundingBox = cv::boundingRect(contours[x]);
		cv::approxPolyDP(cv::Mat(contours[x]), approx, cv::arcLength(cv::Mat(contours[x]), true) * 0.025, true);

		if (boundingBox.width > 25 && boundingBox.height > 25 && approx.size() >= 4) {
			sortCorners(approx, corners);

			cv::Rect rect_top = cv::Rect(cv::Point(boundingBox.x + (boundingBox.width / 4), boundingBox.y),
										 cv::Point(boundingBox.x + boundingBox.width - (boundingBox.width / 4),
												   boundingBox.y + (boundingBox.height * 0.65)));
			cv::Mat sub_top = src(rect_top);

			cv::Rect goal = cv::Rect(
					cv::Point(boundingBox.x + (boundingBox.width * 0.2), boundingBox.y - (boundingBox.height * 0.3)),
					cv::Point(boundingBox.x + boundingBox.width - (boundingBox.width * 0.2),
							  boundingBox.y + (boundingBox.height * 0.4)));

			double topDarkness = (double) cv::countNonZero(sub_top) / rect_top.area();

			if (topDarkness < 0.05) {
				double leftHeight = normalDist(corners[0], corners[3]);
				double rightHeight = normalDist(corners[1], corners[2]);

				double leftDist = distToSideHeight(leftHeight);
				double rightDist = distToSideHeight(rightHeight);

				data->x = (src.cols / 2) - (boundingBox.x + (boundingBox.width / 2));
				data->y = (src.rows / 2) - (boundingBox.y + (boundingBox.height / 2));
				data->leftDist = leftDist;
				data->rightDist = rightDist;

				cv::rectangle(output, boundingBox, CV_RGB(0, 255, 0), 1);
				cv::rectangle(output, goal, CV_RGB(255, 0, 0), 2);
				cv::rectangle(output, rect_top, CV_RGB(0, 255, 255), 1);

				for (int i = 0; i < corners.size(); ++i) {
					cv::circle(output, corners[i], 1, CV_RGB(255, 0, 0), 3);
				}
			}
		}
	}

	cv::imshow("Output", output);

	src.release();
	output.release();

	return data;
}