/*
 * Webcam.hpp
 *
 *  Created on: 04.10.2017
 *      Author: Christoph Neuhauser
 */

#ifndef WEBCAM_HPP_
#define WEBCAM_HPP_

#include <boost/shared_ptr.hpp>
#include "FrameData.hpp"

namespace cv {
class VideoCapture;
}

class Webcam
{
public:
	Webcam();
	~Webcam();
	//! \param id is the number of the camera
	bool open(int id = 0);
	//! \return Returns false if no frame is available
	bool readFrame(FrameDataPtr frameImage, FrameDataPtr downscaledImage);

private:
	cv::VideoCapture *stream;
};

#endif /* WEBCAM_HPP_ */
