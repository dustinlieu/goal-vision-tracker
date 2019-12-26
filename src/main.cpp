#include <iostream>
#include <pthread.h>

#include "Iolite.hpp"
#include "KinectDevice.hpp"

int main(int argc, char** argv) {
	Freenect::Freenect freenect;
	KinectDevice &device = freenect.createDevice<KinectDevice>(0);

	Iolite* iolite = new Iolite("roborio-968-frc.local", 5800);

	device.startVideo();

	while (cvWaitKey(10) < 0) {
		device.GetGoalData();
		iolite->Update();
	}

	cvDestroyWindow("Output");
	device.stopVideo();

	return 0;
}
