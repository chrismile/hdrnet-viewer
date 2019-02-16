/*
 * FrameData.hpp
 *
 *  Created on: 05.10.2017
 *      Author: Christoph Neuhauser
 */

#ifndef FRAMEDATA_HPP_
#define FRAMEDATA_HPP_

#include <cstdint>
#include <boost/shared_ptr.hpp>

struct FrameData {
public:
    FrameData();
    ~FrameData();
    uint8_t *pixels;
    int w, h;
};

typedef boost::shared_ptr<FrameData> FrameDataPtr;

#endif /* FRAMEDATA_HPP_ */
