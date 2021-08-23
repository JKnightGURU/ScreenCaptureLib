## ScreenCaptureLib
### Cross-platform C++ screen capture and recording library

ScreenCaptureLib was created somewhere around 2019 as an internal tool, due to me being unable to find a decent library for screen capturing and recording. Now we've decided to opensource it.

### Supported OS: 
- Windows (vc120, vc141)
- Linux (pretty much any modern compiler)
- Probably, MacOS (but I have no way to check that out)

### Features

- MPEG4 writer for any continuous `cv::Mat` images based on FFMPEG (yeah, there's `cv::VideoWriter`, but it's worse in terms of performance and it doesn't finalize files correctly);
- Screen recorder based on `XShm` extension of `X11` for Linux and [screen_capture_lite] for Windows (thanks, **@smasherprog**!);
- Multiple screens supported;
- Buffered capturing enables a backward offset of an actual start in seconds.

### Dependencies

- FFMPEG <4.0
- OpenCV
- [screen_capture_lite] - *Windows*

### Developers

- @[jknightmmcs]
- @[TheDZhon]

### How to build
> To be added later
> This is a stub

### Usage

Screen recording:
```cpp
#include "screenrecorder.h"

#include <thread>
#include <iostream>

#include <opencv2/imgproc.hpp>

int main()
{
    sc_api::ScreenRecorder recorder;

	// (buffer_size_in_seconds, desired_FPS)
    recorder.init(5, 10);

	// Let recorder thread work for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

	// Initialize recording with the following parameters:
	// Recording directory: "."
	// Prefix (will be concatenated with a screen number): "out_mon_t1_"
	// API type (simply use wbFFMPEG)
    recorder.startRecording(".", "out_mon_t1_", sc_api::wbFFMPEG);
	
	// Sleep for 20 seconds more
    std::this_thread::sleep_for(std::chrono::seconds(20));
    
	// In the end, get a file named "out_mon_t1_0.mp4" 
	// in a desired directory, with the length of 25 seconds.
	// 5: buffered, from the first sleep 
	// 20: during the last sleep
    recorder.stopRecording();
	
	return 0;
}
```

Video file writer:

```cpp
#include "basevideowriter.h"

#include <thread>
#include <iostream>

#include <opencv2/imgproc.hpp>

int main()
{
    // initialize writer: using FFMPEG, { filename, FPS, width, height, bitrate }
	
    auto writer = sc_api::createVideoWriter(sc_api::wbFFMPEG, { "out.mp4", 25, 1024, 768, 800000 });

	// let's generate some frames
	
    for (int i = 0; i < 500; i++)
    {
        cv::Mat   mat = cv::Mat::zeros(768, 1024, CV_8UC3);
        cv::Point pt1(rand() % 1024, rand() % 768);
        cv::Point pt2(rand() % 1024, rand() % 768);
        cv::rectangle(mat, pt1, pt2, cv::Scalar::all(255), 8);

		// and simply write them into a writer object
        writer->write(mat);
    }
	
	return 0;
}

```


## License

MIT

[screen_capture_lite]: <https://github.com/smasherprog/screen_capture_lite>
[jknightmmcs]: <https://github.com/jknightmmcs>
[TheDZhon]: <https://github.com/thedzhon>
