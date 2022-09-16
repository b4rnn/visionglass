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

#include <opencv2/core.hpp>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include <sw/redis++/redis++.h>
#include <experimental/filesystem>
#include <boost/thread/thread.hpp>


using namespace std;
using namespace cv;
using namespace sw::redis;
using namespace std::chrono_literals;

namespace fs = std::experimental::filesystem;

//class MTCNN;

/*REDIS*/
int face_count = 0;
//Redis *_redis = new Redis("tcp://127.0.0.1:6379");

int main(int argc, char* argv[])
{
    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());
    std::string camera_id = capture_data[1];

    //GATHER INDIVIDUAL LIVE CAM METRICS
    
    VideoCapture *capture = new VideoCapture(capture_data[0] , cv::CAP_FFMPEG);
    //Width
    int frame_width = capture->get(cv::CAP_PROP_FRAME_WIDTH); 
    //Height
    int frame_height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);
    //Frame Rate 
    int frame_rate = capture->get(cv::CAP_PROP_FPS);

    //capture->release();

    std::stringstream* frame_super_string = new std::stringstream();
    std::vector<uint8_t>* poi_alert_buffer = new std::vector<uint8_t>();
    std::vector<uint8_t>* recog_face_buffer = new std::vector<uint8_t>();
    std::vector<uint8_t>* detected_face_buffer = new std::vector<uint8_t>();
    std::vector<uint8_t>* frame_buffer = new std::vector<uint8_t>(frame_width * frame_height * 3 + 16);
    cv::Mat* displayWindow = new cv::Mat(cv::Size(frame_width, frame_height),CV_8UC3 ,Scalar(0, 0, 0));
    //displayWindow->setTo(cv::Scalar(0,0,0,0));

    if(capture_data.size() > 0){
        auto  CHANNEL = [camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate](const std::string& key){
            Redis *_redis = new Redis("tcp://127.0.0.1:6379?keep_alive=true&socket_timeout=100ms&connect_timeout=100ms");
            auto sub = _redis->subscriber();
            sub.unsubscribe(capture_data[1]+"-nt");
            sub.unsubscribe(capture_data[1]+"-det");
            // Set callback functions.
            //sub.on_message([camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate](std::string  channel, std::string msg) {
                if(key == capture_data[1]+"-nt"){
                    sub.on_message([camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate,_redis](std::string  channel, std::string msg) {
                        int pos = msg.find(":");
                        std::string frame_number = msg.substr(0 , pos);
                        std::string frame_raw_data = msg.substr(pos + 1);

                        std::vector<unsigned char> pic_data(frame_raw_data.begin(), frame_raw_data.end());
                        cv::Mat vmat(pic_data, true);
                        cv::Mat video_frame = cv::imdecode(vmat, 1);
                        //cv::resize(video_frame, video_frame, Size(640, 480), 0, 0, INTER_CUBIC);
                        cv::resize(video_frame, video_frame, Size(1504, 846), 0, 0, INTER_CUBIC);

                        cv::Mat roi = (*displayWindow)(cv::Rect(0, 0, 1504, 846));
                        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 125, 125));
                        double alpha = 0.2;
                        cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);


                        (&video_frame)->copyTo((*displayWindow)(cv::Rect(0, 0,(&video_frame)->cols, (&video_frame)->rows)));

                        try{
                            cv::imencode(".png", *displayWindow, *frame_buffer);
                            std::string _data_payload((*frame_buffer).begin(), (*frame_buffer).end());
                            std::string data_payload = frame_number+":"+ _data_payload;
                            //_redis->publish(camera_id+"-mp", data_payload);
                            _redis->publish(camera_id+"-cnc", data_payload);
                            //frame_buffer->clear();
                        }catch( cv::Exception& e ){
                            const char* err_msg = e.what();
                            std::cout << "exception caught: corrrupted image on camera " << std::endl;
                        }
                    });
                }
                    
                if(key==capture_data[1]+"-det"){
                    sub.on_message([camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate,_redis](std::string  channel, std::string msg) {
                        int _pos = msg.find(":");
                        std::string margin_left = msg.substr(0 , _pos);
                        std::string detected_frame_raw_data = msg.substr(_pos + 1);

                        std::vector<unsigned char> _pic_data(detected_frame_raw_data.begin(), 
                        detected_frame_raw_data.end());
                        cv::Mat _vmat(_pic_data, true);
                        cv::Mat dFace = cv::imdecode(_vmat, 1);
                        cv::Mat dect(cv::Size(124, 124),CV_8UC3 ,Scalar(254, 254, 253));
                        (&dFace)->copyTo(dect(cv::Rect(6,6,(&dFace)->cols, (&dFace)->rows)));
                        (&dect)->copyTo((*displayWindow)(cv::Rect( stoi(margin_left) 
                        ,906,(&dect)->cols, (&dect)->rows)));

                        try{
                            cv::imencode(".png", *displayWindow, *detected_face_buffer);
                            std::string _Data_payload((*detected_face_buffer).begin(), 
                            (*detected_face_buffer).end());
                            std::string Data_payload = margin_left+":"+ _Data_payload;
                            //_redis->publish(camera_id+"-mp", Data_payload);
                            _redis->publish(camera_id+"-cnc", Data_payload);
                            //detected_face_buffer->clear();
                        }catch( cv::Exception& e ){
                            const char* err_msg = e.what();
                            std::cout << "exception caught: corrrupted image on camera " << std::endl;
                        }
                        std::cout << "margin left " << margin_left << " face height " << dFace.rows << " face width " << dFace.cols << std::endl;
                    });
                }
                
                if(key==capture_data[1]+"-recog"){
                    sub.on_message([camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate,_redis](std::string  channel, std::string msg) {
                        int rec_pos = msg.find(":");
                        std::string margin_top = msg.substr(0 , rec_pos);
                        std::string _detected_frame_raw_data = msg.substr(rec_pos + 1);

                        std::vector<unsigned char> rec_pic_data(_detected_frame_raw_data.begin(),_detected_frame_raw_data.end());
                        cv::Mat rec_vmat(rec_pic_data, true);
                        cv::Mat rec = cv::imdecode(rec_vmat, 1);
                        (&rec)->copyTo((*displayWindow)(cv::Rect(1524,stoi(margin_top) ,(&rec)->cols, (&rec)->rows)));
                        try{
                            cv::imencode(".png", *displayWindow, *recog_face_buffer);
                            std::string _Rec_Data_payload((*recog_face_buffer).begin(),(*recog_face_buffer).end());
                            std::string Rec_Data_payload = margin_top+":"+ _Rec_Data_payload;
                            //_redis->publish(camera_id+"-mp", Rec_Data_payload);
                            _redis->publish(capture_data[1]+"-cnc", Rec_Data_payload);
                            //recog_face_buffer->clear();
                        }catch( cv::Exception& e ){
                            const char* err_msg = e.what();
                            std::cout << "exception caught: corrrupted image on camera " << std::endl;
                        }
                        std::cout << "margin top " << margin_top << " face height " << rec.rows << " face width " << rec.cols << std::endl;
                    });
                }
            
                if(key == capture_data[1]+"-alert"){
                    sub.on_message([camera_id,displayWindow,frame_buffer,frame_super_string,&capture_data,detected_face_buffer,poi_alert_buffer,recog_face_buffer,frame_rate,_redis](std::string  channel, std::string msg) {
                        try{
                            int vpos = msg.find(":");
                            std::string timestamp = msg.substr(0 , vpos);
                            std::string pop_up_overlay = msg.substr(vpos + 1);
                            std::vector<unsigned char> pop_up_data(pop_up_overlay.begin(),
                            pop_up_overlay.end());

                            cv::Mat _pmat(pop_up_data, true);
                            cv::Mat pmat = cv::imdecode(_pmat, 1);

                            cv::Mat roi = (*displayWindow)(cv::Rect(0, 273, 1504, 300));
                            for (int i = 1; i < frame_rate; i++ ){
                                cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 125, 125));
                                (&pmat)->copyTo(color(cv::Rect(0, 0,(&pmat)->cols, (&pmat)->rows)));

                                double alpha = 0.8;
                                cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
                                cv::imencode(".png", *displayWindow, *poi_alert_buffer);
                                std::string _Data_payload((*poi_alert_buffer).begin(), 
                                (*poi_alert_buffer).end());
                                std::string Data_payload = timestamp+":"+ _Data_payload;
                                std::string _payload = " alert";
                                _redis->publish(camera_id+"-cnc",  Data_payload);
                                usleep(10);
                            }
                            }catch( cv::Exception& e ){
                            const char* err_msg = e.what();
                            std::cout <<" ALERT "<< err_msg << std::endl;
                        }
                    });
                }

            sub.subscribe(key);
            sub.consume();

            //sub.subscribe({capture_data[1]+"-nt",capture_data[1]+"-det",capture_data[1]+"-alert",capture_data[1]+"-recog"});

            // Consume messages in a loop.
            while (true) {
                try {
                    sub.consume();
                } catch (...) {
                    // Handle exceptions.
                }
            }
            sub.unsubscribe(key);
            sub.consume();
            
        };

        thread t1(std::thread(CHANNEL, capture_data[1]+"-nt"));
        thread t2(std::thread(CHANNEL, capture_data[1]+"-det"));
        thread t3(std::thread(CHANNEL, capture_data[1]+"-recog"));
        thread t4(std::thread(CHANNEL, capture_data[1]+"-alert"));
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        
    }
    return 0;
}
