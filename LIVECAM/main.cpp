#include <array>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <spawn.h>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include "stdlib.h"
#include "helpers.hh"

#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <algorithm> 

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <opencv2/core.hpp>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include <sw/redis++/redis++.h>
#include <experimental/filesystem>

using namespace std;
using namespace cv;
using namespace sw::redis;
using namespace std::chrono_literals;

vector<string> capture_destination;
Redis *_redis = new Redis("tcp://STREAM_IP:6379");

int main(int argc, char* argv[])
{
    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());
    std::string CAMERA_URL = capture_data[0];

    auto sub = _redis->subscriber();
    //sub.unsubscribe(capture_data[1]+"-mp");

    //GATHER INDIVIDUAL LIVE CAM METRICS
    VideoCapture *capture = new VideoCapture( CAMERA_URL , cv::CAP_FFMPEG);
    //Width
    int frame_width = capture->get(cv::CAP_PROP_FRAME_WIDTH); 
    //Height
    int frame_height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);
    //Frame Rate 
    int frame_rate = capture->get(cv::CAP_PROP_FPS);

    //frame count
    int frame_count = capture->get(cv::CAP_PROP_FRAME_COUNT);

    cv::Mat frame;

    while(true){
        try{
            (*capture) >> frame;
            int frame_time = capture->get(cv::CAP_PROP_POS_MSEC);
            int delay_time = 1000;
            float frame_time_seconds = ((float)frame_time / delay_time);

            long frame_position = capture->get(cv::CAP_PROP_POS_FRAMES);
            
            std::ostringstream oss;
            oss << frame_time_seconds;
            auto _frame_time = oss.str();
            std::cout << " frame time  " << _frame_time << std::endl;

            std::ostringstream _oss;
            _oss << frame_position;
            auto _frame_position = _oss.str();
            //concanate video frame + metrics
            std::vector<uint8_t> frame_buffer;
            std::stringstream frame_super_string;
            cv::imencode(".png", frame, frame_buffer);
            for (auto c : frame_buffer) frame_super_string << c;
            std::string frame_buffer_as_string = frame_super_string.str();
            //std::string notification_payload = _frame_time +":"+frame_buffer_as_string;
            std::string notification_payload = _frame_position +":"+frame_buffer_as_string;
            std::string detection_payload = notification_payload;
            std::string recognition_payload = detection_payload;
            std::string frame_position_det = _frame_position+":"+std::to_string(frame_rate);
            //push frames
            _redis->publish(capture_data[1]+"-nt", notification_payload);
            _redis->publish(capture_data[1]+"-fd", detection_payload);
            _redis->publish(capture_data[1]+"-rec", recognition_payload);
            //push metris
            std::string frame_position_rec =  frame_position_det;
            _redis->publish(capture_data[1]+"-fpd", frame_position_det);
            // _redis->publish(capture_data[1]+"-fpr", frame_position_rec);
       
            std::cout << " WIDTHs  " << frame_width << " HEIGHTs " << frame_height << " position " << _frame_position << " FPSs " << frame_rate  <<  " FNOs " << frame_count << " Timestamp "<< _frame_time << std::endl;

        }catch( cv::Exception& e ){
            const char* err_msg = e.what();
            std::cout << "exception  " << err_msg << " caught on camera  " << capture_data[1]  << std::endl;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
return 0;
}
