#include <array>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <time.h>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include "stdlib.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm> 

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "FacePreprocess.h"
#include <opencv2/core.hpp>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "MTCNN/mtcnn_opencv.hpp"

#include <sw/redis++/redis++.h>
#include <experimental/filesystem>

using namespace std;
using namespace cv;
using namespace sw::redis;
using namespace std::chrono_literals;

class MTCNN;

/*REDIS*/
Redis *_redis = new Redis("tcp://STREAM_IP:6379");
const char arcface_model[30] = "y1-arcface-emore_109";
const string prefix = "./models/linux";

int face_count = 0;
int frames_validator;

//std::string frame_rate;
std::string frame_number;

std::string faceTime;
std::string frame_position;
std::string frame_position_time;

int main(int argc, char* argv[])
{
    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());

    char buff[10];
    char string[10];
    const int stage = 4;
    float factor = 0.709f;
    const int minSize = 20;
    const int avg_face = 15;
    vector<double> angle_list;

    MTCNN detector(prefix);
    MTCNN * _detector = &detector;
    float threshold[3] = {0.7f, 0.6f, 0.6f};
    std::string camera_id = capture_data[1];
    std::string dataSource = capture_data[0];
    double fps, current, score, angle, padding;
    std::vector<uint8_t>* recognised_face_buffer = new std::vector<uint8_t>();
        
    if(capture_data.size() > 0){
        // Create a Subscriber.
        auto sub = _redis->subscriber();
        sub.unsubscribe(capture_data[1]+"-rec");
        sub.unsubscribe(capture_data[1]+"-fpr");


        //GATHER INDIVIDUAL LIVE CAM METRICS
        VideoCapture *capture = new VideoCapture( capture_data[0] , cv::CAP_FFMPEG);
        //Frame Rate 
        int frame_rate = capture->get(cv::CAP_PROP_FPS);
        capture->release();

        // Set callback functions.
        sub.on_message([&capture_data,buff,string,factor,angle_list,_detector,avg_face,minSize,stage,threshold,camera_id,dataSource,recognised_face_buffer,frame_rate](std::string channel, std::string msg) {
            int pos = msg.find(":");
            frame_position= msg.substr(0 , pos);
            std::string frame_raw_data = msg.substr(pos + 1);
            
            std::vector<unsigned char> pic_data(frame_raw_data.begin(), frame_raw_data.end());
            cv::Mat vmat(pic_data, true);
            cv::Mat video_frame = cv::imdecode(vmat, 1);
            //grab video metrics from queue
            try{
                
                if(stoi(frame_position)%frame_rate  == 0){
                    vector<FaceInfo> faceInfo = _detector->Detect_mtcnn(video_frame, minSize, threshold, factor, stage);
                    int faceSize = faceInfo.size();
                    
                    if(faceSize > 0){
                        //some face detected
                        for (int i = 0; i < faceInfo.size(); i++) {
                            int x = (int) faceInfo[i].bbox.xmin;
                            int y = (int) faceInfo[i].bbox.ymin;
                            int w = (int) (faceInfo[i].bbox.xmax - faceInfo[i].bbox.xmin + 1);
                            int h = (int) (faceInfo[i].bbox.ymax - faceInfo[i].bbox.ymin + 1);
                            //extract recognised face 
                            if(faceInfo[i].bbox.score >=  0.9995){
                                face_count++;
                                cv::Rect roi(x, y, w, h);
                                cv::Mat aligned = video_frame(roi);
                                cv::resize(aligned, aligned, Size(112, 112), 0, 0, INTER_LINEAR);
                                //recognised face
                                cv::imencode(".png", aligned, *recognised_face_buffer);
                                std::string _rec_face_payload((*recognised_face_buffer).begin(), (*recognised_face_buffer).end());
                                std::string rec_face_payload = std::to_string(faceInfo[i].bbox.score) +":"+ _rec_face_payload;
                                _redis->publish(camera_id+"-poi",rec_face_payload);
                                std::cout << "recognized face " << faceInfo[i].bbox.score <<" face number " << face_count << " frame rate " << frame_rate <<std::endl;
                            }

                        }
                    }
                }
            
            }catch( cv::Exception& e ){
                const char* err_msg = e.what();
                std::cout << err_msg << std::endl;
            }
        });

        // Subscribe to multiple channels
        sub.subscribe(capture_data[1]+"-rec");

        // Consume messages in a loop.
        while (true) {
            try {
                sub.consume();
            } catch (...) {
                // Handle exceptions.
            }
        }
    }

    return 0;
}
