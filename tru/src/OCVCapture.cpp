//
// OCVCapture - Code for capturing video frames using
// v4l2 into OpenCV Mat wrappers
// 
// Written in 2012 by Martin Fox
//
// To the extent possible under law, the author(s) have dedicated all
// copyright and related and neighboring rights to this software to
// the public domain worldwide. This software is distributed without
// any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication
// along with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

// Modificado por Kleber Lima da Silva em outubro de 2014 (linha 384)


#include "OCVCapture.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <iomanip>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <errno.h>
#include <sys/mman.h>

using namespace cv;
using namespace std;

OCVCapture::OCVCapture()
{
	m_desired_width = 320;
	m_desired_height = 240;
	m_desired_frame_rate = 30;

	m_verbose = false;

	m_camera_handle = -1;
	m_first_grab = true;
	
	m_final_width = 0;
	m_final_height = 0;
	m_final_frame_rate = 0;
	
	m_yuv_image_size = 0;
	m_yuv_image_data = NULL;
	m_yuv_bytes_per_line = 0;
}

OCVCapture::~OCVCapture()
{
	close();
}

void OCVCapture::setVerbose(bool verboseOn)
{
	m_verbose = verboseOn;
}

bool OCVCapture::verbose() const
{
	return m_verbose;
}

void OCVCapture::setDesiredSize(uint32_t width, uint32_t height)
{
	if (isOpen())
	{
		reportError("cannot change image size while open");
		return;
	}
	
	if (width != m_desired_width || height != m_desired_height)
	{
		m_desired_width = width;
		m_desired_height = height;
	}
}

static const char* messageHeader = "Capture: ";

void OCVCapture::reportError(const char *error)
{
	cerr << messageHeader << error << endl;
}

void OCVCapture::reportError(const char* error, int64_t value)
{
	cerr << messageHeader << error << " " << value << endl;
}
	
int OCVCapture::retry_ioctl(int request, void* argument)
{
	int result;
	
	do
	{
		result = v4l2_ioctl(m_camera_handle, request, argument);
	}
	while (result == -1 && errno == EINTR);
	
	return result;
}

bool OCVCapture::isOpen() const
{
	return (m_camera_handle > 0);
}

uint32_t OCVCapture::width() const
{
	return m_final_width;
}

uint32_t OCVCapture::height() const
{
	return m_final_height;
}

uint32_t OCVCapture::frameRate() const
{
	return m_final_frame_rate;
}

bool OCVCapture::open()
{
	if (isOpen())
		return true;
	
	m_first_grab = true;

	const char* deviceName = "/dev/video0";
	
	m_camera_handle = v4l2_open(deviceName, O_RDWR | O_NONBLOCK, 0);
	if (m_camera_handle <= 0)
	{
		reportError("failed to open /dev/video0");
		close();
		return false;
	}
	
	// Query for capabilities
	struct v4l2_capability cap;
	bzero(&cap, sizeof(cap));
	
	if (retry_ioctl(VIDIOC_QUERYCAP, &cap) == -1)
	{
		reportError("failed to read capabilities");
		close();
		return false;
	}
	else
	{
		if (m_verbose)
		{
			cout << messageHeader << "capabilities " << hex << cap.capabilities << dec << endl;
		}
		
		if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
		{
			reportError("does not have capture capability");
			close();
			return false;
		}
	}
	
	// Query for channels
	int channel = -1;
	if (retry_ioctl(VIDIOC_G_INPUT, &channel) == -1)
	{
		reportError("failed to read channel");
		close();
		return false;
	}
	else if (m_verbose)
	{
		cout << messageHeader << "channel " << channel << endl;
	}
	
	if (m_verbose)
	{
		// Enumerate the inputs.
		struct v4l2_input input = {0};
		while (retry_ioctl(VIDIOC_ENUMINPUT, &input) != -1)
		{
			cout << messageHeader << "input " << input.index << " " << input.name << " " << hex << input.std << dec << endl;
			input.index += 1;
		}
	}
	
	if (m_verbose)
	{
		// Enumerate formats
		struct v4l2_fmtdesc format_description = {0};
		format_description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		while (retry_ioctl(VIDIOC_ENUM_FMT, &format_description) != -1)
		{
			cout << messageHeader << "format " << format_description.description << " ";
			uint32_t pixelFormat = format_description.pixelformat;
			while (pixelFormat)
			{
				char byte = (pixelFormat & 0xFF);
				cout << byte;
				pixelFormat >>= 8;
			}
			cout << endl;

			format_description.index += 1;
		}
	}

	static const uint32_t desiredBuffers = 4;
	static const uint32_t desiredFormat = V4L2_PIX_FMT_YUYV;
	int matrixType = CV_8UC2;
    
	struct v4l2_format format;
	bzero(&format, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (retry_ioctl(VIDIOC_G_FMT, &format) == -1)
	{
		reportError("could not read initial format");
		close();
		return false;
	}
	
	format.fmt.pix.pixelformat =	desiredFormat;
	format.fmt.pix.field =			V4L2_FIELD_ANY;
	format.fmt.pix.width =			m_desired_width;
	format.fmt.pix.height =			m_desired_height;
	format.fmt.pix.bytesperline =	0;

	if (retry_ioctl(VIDIOC_S_FMT, &format) == -1)
	{
		reportError("could not set format");
	}

	if (format.fmt.pix.pixelformat != desiredFormat)
	{
		reportError("driver could not provide requested format");
		close();
		return false;
	}
	
	m_final_width = format.fmt.pix.width;
	m_final_height = format.fmt.pix.height;
	m_yuv_bytes_per_line = format.fmt.pix.bytesperline;
	
	if (m_verbose)
	{
		cout << messageHeader << "dimensions " << m_final_width << " x " << m_final_height << endl;
		cout << messageHeader << "bytes per line " << m_yuv_bytes_per_line << endl;
	}

	// Set the frame rate
	if ((cap.capabilities & V4L2_CAP_TIMEPERFRAME) != 0)
	{
		struct v4l2_streamparm setfps;
		bzero(&setfps, sizeof(setfps));
		setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		setfps.parm.capture.timeperframe.numerator = 1;
		setfps.parm.capture.timeperframe.denominator = m_desired_frame_rate;
		retry_ioctl(VIDIOC_S_PARM, &setfps);
	}
	
	// Query the frame rate
	struct v4l2_streamparm getfps;
	bzero(&getfps, sizeof(getfps));
	getfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (retry_ioctl(VIDIOC_G_PARM, &getfps) == -1)
		reportError("could not query frame rate");
	else
	{
		float frameInterval = (float) getfps.parm.capture.timeperframe.numerator /
						      (float) getfps.parm.capture.timeperframe.denominator;
		m_final_frame_rate = round(1.0 / frameInterval);
		
		if (m_verbose)
		{
			cout << messageHeader << "frame rate " << m_final_frame_rate << " fps" << endl;
		}
	}
	
	// Request buffers.
	struct v4l2_requestbuffers request;
	bzero(&request, sizeof(request));
	
	uint32_t bufferCount = desiredBuffers;
	
	request.count = bufferCount;
	request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request.memory = V4L2_MEMORY_MMAP;
   
	while (true)
	{
		if (retry_ioctl(VIDIOC_REQBUFS, &request) == -1)
		{
			reportError("could not map buffers");
			close();
			return false;
		}
		
		// If the number of buffers returned is 0 try asking for a
		// smaller number.
		if (request.count == 0)
		{
			if (bufferCount == 1)
			{
				reportError("ran out of memory for buffers");
				close();
				return false;
			}
			
			bufferCount--;
			request.count = bufferCount;
		}
		else
			break;
	}
	
	if (m_verbose)
	{
		cout << messageHeader << bufferCount << " buffers allocated" << endl;
	}
	
	bufferCount = request.count;
	m_mapped_buffer_ptrs.reserve(bufferCount);
	m_mapped_buffer_lens.reserve(bufferCount);
	
	// Now map the buffers.
	for (uint32_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex)
	{
		struct v4l2_buffer buffer;

		bzero(&buffer, sizeof(buffer));
		
		buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = bufferIndex;
		
		if (retry_ioctl(VIDIOC_QUERYBUF, &buffer) == -1)
		{
			reportError("failed to query buffer", bufferIndex);
			close();
			return false;
		}
		
		if (m_verbose)
		{
			cout << messageHeader << "buffer length " << buffer.length << endl;
		}
		
		void* mapped = v4l2_mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
								 MAP_SHARED, m_camera_handle, buffer.m.offset);
		
		if (mapped == MAP_FAILED)
		{
			reportError("failed to map buffer", bufferIndex);
			close();
			return false;
		}
		
		m_mapped_buffer_ptrs.push_back(mapped);
		m_mapped_buffer_lens.push_back(buffer.length);
	}
	
	// Now allocate the image data.
	m_yuv_image_size = m_final_height * m_yuv_bytes_per_line;
	m_yuv_image_data = new (nothrow) uint8_t[m_yuv_image_size];
	
	if (m_yuv_image_data == NULL)
	{
		reportError("Could not allocate YUV image data");
		close();
		return false;
	}
	

	// Adicionado por Kleber Lima da Silva em outubro de 2014
	// Altera o valor do "exposure"
	struct v4l2_control control1;
	control1.id = V4L2_CID_EXPOSURE_AUTO;
	control1.value = V4L2_EXPOSURE_MANUAL;
	retry_ioctl (VIDIOC_S_CTRL, &control1);

	v4l2_set_control(m_camera_handle, V4L2_CID_EXPOSURE_AUTO_PRIORITY, 0);
	v4l2_set_control(m_camera_handle, V4L2_CID_EXPOSURE_ABSOLUTE, 1081);


	return true;
}

bool OCVCapture::firstGrabSetup()
{
	if (m_first_grab)
	{		
		// Enqueue all the buffers.
		size_t numBuffers = m_mapped_buffer_ptrs.size();
		for (size_t i = 0; i < numBuffers; ++i)
		{
			struct v4l2_buffer buffer;
			bzero(&buffer, sizeof(buffer));
			
			buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buffer.memory = V4L2_MEMORY_MMAP;
			buffer.index = i;
			
			if (retry_ioctl(VIDIOC_QBUF, &buffer) == -1)
			{
				reportError("failed to enqueue buffer", i);
				return false;
			}
		}

		// Enable streaming
		int captureType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (retry_ioctl(VIDIOC_STREAMON, &captureType) == -1)
		{
			reportError("failed to start streaming");
			return false;
		}
	}

	m_first_grab = false;
	return true;
}
	
bool OCVCapture::grab()
{	
	if (!isOpen())
		return false;

	if (!firstGrabSetup())
		return false;
		
	enum status_type { kTrying, kFailure, kSuccess };

	status_type status = kTrying;
	void* dataPtr = NULL;
	size_t dataLen = 0;
	
	while (status == kTrying)
	{
		fd_set readset;
		struct timeval timeout;
		
		FD_ZERO(&readset);
		FD_SET(m_camera_handle, &readset);
		
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		
		int numReady = select(m_camera_handle + 1, &readset, NULL, NULL, &timeout);
		
		if (numReady == -1)
		{
			// Wait for it...
			if (errno != EINTR)
			{
				reportError("error on select", errno);
				status = kFailure;
			}
		}
		else if (numReady == 0)
		{
			reportError("select timeout");
			status = kFailure;
		}
		else
		{
			struct v4l2_buffer buffer;
			bzero(&buffer, sizeof(buffer));
			
			buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buffer.memory = V4L2_MEMORY_MMAP;
			
			if (retry_ioctl(VIDIOC_DQBUF, &buffer) == -1)
			{
				if (errno != EAGAIN)
				{
					reportError("Failure to dequeue buffer");
					status = kFailure;
				}
			}
			else
			{
				uint32_t bufferIndex = buffer.index;
				if (bufferIndex >= m_mapped_buffer_ptrs.size())
				{
					reportError("dequeued buffer index out of range");
					status = kFailure;
				}
				else
				{
					status = kSuccess;
					// Grab the data.
					dataPtr = m_mapped_buffer_ptrs[bufferIndex];
					memcpy(m_yuv_image_data, dataPtr, m_yuv_image_size);
					
					// Put this buffer back on the queue
					if (retry_ioctl(VIDIOC_QBUF, &buffer) == -1)
					{
						reportError("error putting buffer back in queue");
						status = kFailure;
					}
				} 
			}
		}
	}
	
	return (status == kSuccess);
}

void OCVCapture::resizeMat(Mat& mat, int matType)
{
	if (mat.empty() ||
		mat.rows != m_final_height ||
		mat.cols != m_final_width ||
		mat.type() != matType)
	{
		mat = Mat(m_final_height, m_final_width, matType);
	}
}

bool OCVCapture::gray(Mat& grayMat)
{
	if (!isOpen())
		return false;
	
	if (m_yuv_image_data == NULL)
		return false;
		
	resizeMat(grayMat, CV_8UC1);
	
	for (size_t rowIndex = 0; rowIndex < m_final_height; ++rowIndex)
	{
		const uint8_t *getIt = m_yuv_image_data + rowIndex * m_yuv_bytes_per_line;
		uint8_t *putIt = grayMat.ptr(rowIndex);

		for (size_t colIndex = 0; colIndex < m_final_width; ++colIndex)
		{
			*putIt++ = *getIt;
			getIt += 2;
		}
	}
	
	return true;
}

#define Fixed(n) ((int)((n) * 255.0 + 0.5))
#define clamp(v) ((v) < 0 ? 0 : ((v) > 255 ? 255 : (v)))
#define prepare(comp) clamp((comp + 128) >> 8)

bool OCVCapture::rgb(Mat& rgbMat)
{
	if (!isOpen())
		return false;
		
	if (m_yuv_image_data == NULL)
		return false;
		
	resizeMat(rgbMat, CV_8UC3);
	
	for (size_t rowIndex = 0; rowIndex < m_final_height; ++rowIndex)
	{
		const uint8_t *getIt = m_yuv_image_data + rowIndex * m_yuv_bytes_per_line;
		uint8_t *putIt = rgbMat.ptr(rowIndex);
		
		for (size_t colIndex = 0; colIndex < m_final_width; colIndex += 2)
		{
			int y1 = *getIt++;
			int cb = *getIt++;
			int y2 = *getIt++;
			int cr = *getIt++;
			
			// There seems to be multiple way so of doing YCbCr to
			// RGB conversions due to variations in the standards.
			
#if 1
			// This conversion uses the standards defined for JFIF
			// which allow all components to span the entire range
			// of 0 to 255. I'm not sure that is what my webcam is
			// doing or whether it conforms to a spec where Y ranges
			// from 16 to 235 and Cb and Cr range from 16 to 239.
			//
			// For details see http://www.fourcc.org/fccyvrgb.php
			
			// First we adjust Cb and Cr so they lie in the range
			// -0.5 to 0.5
			cb -= 128;
			cr -= 128;
			
			// For speed we use fixed point with 8 bits of fractional
			// precision.
			int rAdd = Fixed(1.403) * cr;
			int gAdd = Fixed(-0.344) * cb + Fixed(-.714) * cr;
			int bAdd = Fixed(1.770) * cb;
			
			y1 *= 256;
			y2 *= 256;
#else
			// This is the solution provided by Microsoft, which
			// seems to assume a standard where the component range
			// is restricted so the components require some scaling
			// to bring them into range before the coefficients are
			// applied.
			y1 -= 16;
			y2 -= 16;
			y1 *= 298;
			y2 *= 298;
			
			cb -= 128;
			cr -= 128;
			
			int rAdd = 409 * cr;
			int gAdd = -100 * cb - 208 * cr;
			int bAdd = 516 * cb;
#endif
			
			*putIt++ = prepare(y1 + bAdd);
			*putIt++ = prepare(y1 + gAdd);
			*putIt++ = prepare(y1 + rAdd);

			*putIt++ = prepare(y2 + bAdd);
			*putIt++ = prepare(y2 + gAdd);
			*putIt++ = prepare(y2 + rAdd);
		}
	}
	
	return true;
}

bool OCVCapture::yuv(Mat& yuvMat)
{
	if (!isOpen())
		return false;
		
	if (m_yuv_image_data == NULL)
		return false;
		
	resizeMat(yuvMat, CV_8UC3);
	
	for (size_t rowIndex = 0; rowIndex < m_final_height; ++rowIndex)
	{
		const uint8_t *getIt = m_yuv_image_data + rowIndex * m_yuv_bytes_per_line;
		uint8_t *putIt = yuvMat.ptr(rowIndex);
		
		for (size_t colIndex = 0; colIndex < m_final_width; colIndex += 2)
		{
			int y1 = *getIt++;
			int cb = *getIt++;
			int y2 = *getIt++;
			int cr = *getIt++;
			
			*putIt++ = y1;
			*putIt++ = cb;
			*putIt++ = cr;
			*putIt++ = y2;
			*putIt++ = cb;
			*putIt++ = cr;
		}
	}
	
	return true;
}

void OCVCapture::close()
{	
	if (m_camera_handle >= 0)
	{
		// Turn off the stream.
		int captureType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (retry_ioctl(VIDIOC_STREAMOFF, &captureType) == -1)
			reportError("unable to stop stream");
	}
	
	if (m_yuv_image_data)
		delete [] m_yuv_image_data;
	m_yuv_image_size = 0;
	m_yuv_image_data = NULL;
	m_yuv_bytes_per_line = 0;
	
	for (size_t i = 0; i < m_mapped_buffer_ptrs.size(); ++i)
	{
		if (v4l2_munmap(m_mapped_buffer_ptrs[i], m_mapped_buffer_lens[i]) == -1)
			reportError("could not unmap buffer", i);
	}
	
	m_final_height = 0;
	m_final_width = 0;
	m_final_frame_rate = 0;

	m_mapped_buffer_ptrs.resize(0);
	m_mapped_buffer_lens.resize(0);

	if (m_camera_handle >= 0)
		v4l2_close(m_camera_handle);
		
	m_camera_handle = -1;
}
