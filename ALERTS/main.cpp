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

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using namespace std;
using namespace cv;
using namespace sw::redis;
using namespace std::chrono_literals;

using bsoncxx::stdx::string_view;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

namespace fs = std::experimental::filesystem;

mongocxx::instance instance{};
mongocxx::uri uri("mongodb://DATABASE_IP:27017");
mongocxx::client client(uri);
mongocxx::database db = client["photo_bomb"];

int face_count = 0;
std::string captured_face;
std::string recognised_face;
std::string recognised_face_id;
std::string recognised_face_score;
vector<string> capture_destination;
Redis *_redis = new Redis("tcp://STREAM_IP:6379");

int main(int argc, char* argv[])
{
    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());
    std::string camera_id = capture_data[1];

    std::vector<uint8_t>* pop_up = new std::vector<uint8_t>();
    if(capture_data.size() > 0){
        // Create a Subscriber.
        auto sub = _redis->subscriber();
        sub.unsubscribe(capture_data[1]+"-cf");
        sub.unsubscribe(capture_data[1]+"-tf");
        // Set callback functions.
        sub.on_message([camera_id,&capture_data,pop_up](std::string channel, std::string msg) {
            face_count++;
            if(channel==capture_data[1]+"-cf"){

                int _pos = msg.find(":");
                captured_face = msg.substr(_pos + 1);
                recognised_face_score = msg.substr(0 , _pos);
            }

            if(channel==capture_data[1]+"-tf"){

                int pos = msg.find(":");
                recognised_face = msg.substr(pos + 1);
                recognised_face_id= msg.substr(0 , pos);
                //cv::Rect(0, 273, 1504, 300)
                
                cv::Mat color(cv::Size(1504, 300), CV_8UC3, cv::Scalar(0, 125, 125));
                cv::Mat _dFace ,_pFace;

                std::vector<unsigned char> dFace(captured_face.begin(), captured_face.end());
                cv::Mat dmat(dFace, true);
                _dFace = cv::imdecode(dmat, 1);

                std::vector<unsigned char> pFace(recognised_face.begin(), recognised_face.end());
                cv::Mat pmat(pFace, true);
                _pFace = cv::imdecode(pmat, 1);

                cv::Mat poi_right(cv::Size(200, 200),CV_8UC3 ,Scalar(42, 32, 23));
                cv::Mat poi_left(cv::Size(200, 200),CV_8UC3 ,Scalar(254, 254, 253));
                cv::resize(_dFace, _dFace, Size(200, 180), 0, 0, INTER_CUBIC); 
                cv::resize(_pFace, _pFace, Size(200, 180), 0, 0, INTER_CUBIC); 

                (&_dFace)->copyTo(poi_left(cv::Rect(0,10,(&_dFace)->cols, (&_dFace)->rows)));
                (&_pFace)->copyTo(poi_right(cv::Rect(0,10,(&_pFace)->cols, (&_pFace)->rows)));
                (&poi_left)->copyTo(color(cv::Rect(407,50,(&poi_left)->cols, (&poi_left)->rows)));
                (&poi_right)->copyTo(color(cv::Rect(957,50,(&poi_right)->cols, (&poi_right)->rows)));

                //query db for details
                
                using bsoncxx::stdx::string_view;

                auto _cursor = db["person_of_interest"].find(document{} 
                    << "_id"
                    << bsoncxx::oid{recognised_face_id}
                    << finalize
                );
                
                std::string userID;
                std::string userName;
                std::string arrestDetails;
                for(auto doc : _cursor) {
                    bsoncxx::document::element id = doc["_id"];
                    bsoncxx::document::element name = doc["name"];
                    bsoncxx::document::element arrest_details = doc["arrest_details"];
                    userID = id.get_oid().value.to_string();
                    userName = name.get_utf8().value.to_string();
                    arrestDetails = arrest_details.get_utf8().value.to_string();
                }

                std::cout <<" userID "<< userID <<" arrestDetails " << arrestDetails << " userName " << userName << endl;

                std::string match = recognised_face_score.substr(0, 3) + recognised_face_score.substr(0, 3);
                putText(color,"Captured" , cv::Point(50,140), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(254, 254, 253), 2.0, CV_MSA);
                putText(color,"Target" , cv::Point(1320,140), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(254, 254, 253), 2.0, CV_MSA);
                putText(color,"Similarity" , cv::Point(695,100), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(254, 254, 253), 2.0, LINE_AA);
                putText(color, recognised_face_score, cv::Point(685,180), FONT_HERSHEY_PLAIN, 3.5, Scalar(254, 254, 253), 2.0, CV_MSA);
                putText(color,userName , cv::Point(1170,200), FONT_HERSHEY_PLAIN, 2.5, Scalar(22, 30, 100), 2.5, CV_MSA);

                std::stringstream  _Data_payload;
                cv::imencode(".png", color, *pop_up);
                for (auto c : *pop_up)  _Data_payload << c;
                std::string Data_payload = recognised_face_score +":"+ _Data_payload.str();
                _redis->publish(camera_id+"-alert", Data_payload);
                std::cout << " face " << recognised_face_score << " id " << recognised_face_id  << " width " << (&color)->cols << " height " << (&color)->rows << std::endl;
         

                auto folder =  std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
                string folder_name = "/var/www/html/personas/"+ std::to_string(folder);
    
                int directory_status;
                const char* dirname = folder_name.c_str();
                directory_status = mkdir(dirname,0777);

                string _dirname = folder_name+"/%ld.jpg";
                const char* Dirname = _dirname.c_str();

                char filename[100];
                auto nanos =  std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
                sprintf(filename, Dirname , nanos);
                cv::imwrite(filename, _dFace.clone());

                char _filename[100];
                auto _nanos =  std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
                sprintf(_filename, Dirname, _nanos);
                cv::imwrite(_filename, _pFace.clone());

                auto _cam = db["live_cams"].find(document{} 
                    << "_id"
                    << bsoncxx::oid{camera_id}
                    << finalize
                );

                auto start = std::chrono::system_clock::now();

                std::string latitude;
                std::string longitude;
                std::string location_id;
                std::string location_name;

                std::string _camera_gps;
                std::string camera_name;

                for(auto _doc : _cam) {
                    bsoncxx::document::element _id = _doc["_id"];
                    bsoncxx::document::element camera_gps = _doc["camera_gps"];
                    bsoncxx::document::element cameraname = _doc["displayname"];

                    location_id = _id.get_oid().value.to_string();
                    _camera_gps = camera_gps.get_utf8().value.to_string();
                    camera_name = cameraname.get_utf8().value.to_string();
                }

                int cam_pos = _camera_gps.find(",");
                latitude = _camera_gps.substr(0 , cam_pos);
                longitude = _camera_gps.substr(cam_pos + 1);
                
                auto _location = db["live_cams_locations"].find(document{} 
                    << "_id"
                    << bsoncxx::oid{location_id}
                    << finalize
                );

                for(auto _Doc : _location) {
                    bsoncxx::document::element locationname = _Doc["locationname"];
                    location_name = locationname.get_utf8().value.to_string();
                }

                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                std::time_t end_time = std::chrono::system_clock::to_time_t(end);

                std::ostringstream oss;
                oss << std::ctime(&end_time);
                auto timestamp = oss.str();
                
                std::string original_image_url = filename;
                std::string detected_image_url = _filename;

                 db["profiles"].insert_one(document{} 
                    << "name" << userName
                    << "original_image_url" <<  "/personas/"+ original_image_url.erase(0,23)
                    << "detected_image_url" << "/personas/"+ detected_image_url.erase(0,23)
                    << "status" << arrestDetails
                    << "location_name" << location_name
                    << "camera_name" << camera_name
                    << "latitude" << latitude
                    << "longitude" << longitude
                    << "userId" << userID
                    << "timestamp" << timestamp
                    << "confidence" << match 
                    << finalize
                );

                std::cout <<" poi id "<< recognised_face_id << endl;
            }
        });

        sub.subscribe({capture_data[1]+"-cf",capture_data[1]+"-tf"});

        // Consume messages in a loop.
        while (true) {
            try {
                sub.consume();
            } catch (...) {
                // Handle exceptions.
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
return 0;
}
