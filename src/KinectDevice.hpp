#ifndef KinectDevice_hpp
#define KinectDevice_hpp

#include <pthread.h>

#include <libfreenect.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class KinectDevice : public Freenect::FreenectDevice {
	public:
		struct GoalData {
			double x;
			double y;
			double leftDist;
			double rightDist;
		};

		KinectDevice(freenect_context* context, int index);

		void VideoCallback(void* data, uint32_t timestamp);
		bool GetVideo(cv::Mat &output);
		std::shared_ptr<GoalData> GetGoalData();
	private:
		cv::Mat m_IRMat;
		pthread_mutex_t m_IRMutex;
		bool m_IRNewFrame;
};

#endif
