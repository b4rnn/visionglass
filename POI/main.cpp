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

#include "tbb/concurrent_hash_map.h"

#include <opencv2/features2d.hpp>

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

/*REDIS*/
int face_count = 0;
Redis *_redis = new Redis("tcp://STREAM_IP:6379");

void rootSift(Mat &descriptors, const float eps = 1e-7)
{
	// Compute sums for L1 Norm
	Mat sums_vec;
	descriptors = abs(descriptors); //otherwise we draw sqrt of negative vals
	reduce(descriptors, sums_vec, 1 /*sum over columns*/, REDUCE_SUM, CV_32FC1);
	for (int row = 0; row < descriptors.rows; row++) {
		int offset = row * descriptors.cols;
		for (int col = 0; col < descriptors.cols; col++) {
			descriptors.at<float>(offset + col) = sqrt(descriptors.at<float>(offset + col) /
				(sums_vec.at<float>(row) + eps) /*L1-Normalize*/);
		}
		// L2 distance
		normalize(descriptors.row(row), descriptors.row(row), 1.0, 0.0, NORM_L2);
	}
	return;
}

struct trainingData{
    cv::Mat Tdesc;
    cv::Mat Tdata;
    float Tbrightness;
    std::string userId;
    };


struct rankingData{
    cv::Mat Rdata;
    float score;
    std::string userId;
};

int counter = 0;

int live_face_count = 0;

Ptr<cv::Feature2D> detector = SIFT::create(128);

cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");

tbb::concurrent_hash_map<int, std::vector< trainingData> > hash_map;
typedef tbb::concurrent_hash_map<int, std::vector<trainingData> >::accessor hash_map_accessor; 

std::vector<trainingData> data;

 std::vector<uint8_t>* recognised_face_buffer = new std::vector<uint8_t>();

int main(int argc, char* argv[])
{
    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());
    std::string camera_id = capture_data[1];

    //find facial threshold
    auto _cursor = db["live_cams"].find(document{} 
        << "_id"
        << bsoncxx::oid{capture_data[1]}
        << finalize
    );

    std::string poiThreshold;
    for(auto doc : _cursor) {
        bsoncxx::document::element Threshold = doc["poi_threshold"];
        poiThreshold = Threshold.get_utf8().value.to_string();
    }
                
    if(capture_data.size() > 0){
        // Create a Subscriber.
        auto sub = _redis->subscriber();
        sub.unsubscribe(capture_data[1]+"-poi");
        // Set callback functions.
        sub.on_message([camera_id,&capture_data,poiThreshold](std::string channel, std::string msg) {
            if(channel==capture_data[1]+"-poi"){
                //int DB_SIZE = db["person_of_interest"].estimated_document_count( {} );
                auto cursor = db["person_of_interest"].find({});
                for (auto&& doc : cursor){
                    try{
                        bsoncxx::document::element id = doc["_id"];
                        bsoncxx::document::element pic = doc["facial_structured"];
                        bsoncxx::document::element pic_luminannce = doc["facial_luminannce"];
                        bsoncxx::document::element facial_unstructured = doc["facial_unstructured"];
                        std::string original_pic = facial_unstructured.get_utf8().value.to_string();

                        int upos = original_pic.find(":");

                        std::string profile_id = id.get_oid().value.to_string();
                        std::string profile_pic = pic.get_utf8().value.to_string();
                        std::string profile_luminannce = pic_luminannce.get_utf8().value.to_string();
                        std::string original_profile_pic = original_pic.substr(upos + 1);;
                        
                        std::vector<unsigned char> profile_pic_data(profile_pic.begin(), profile_pic.end());
                        cv::Mat pmat(profile_pic_data, true);
                        cv::Mat dbFace= cv::imdecode(pmat, 1);

                        std::vector<unsigned char> original_profile_pic_data(original_profile_pic.begin(), original_profile_pic.end());
                        cv::Mat original_pmat(original_profile_pic_data, true);
                        cv::Mat original_trainDescriptors = cv::imdecode(original_pmat, 1);

                        float original_luminance = std::stof(profile_luminannce);

                        cv::Mat trainDescriptors;
                        std::vector<cv::KeyPoint> trainKeypoints;
                        //cv::resize(original_trainDescriptors, original_trainDescriptors, Size(48, 56), 0, 0, INTER_LINEAR);
                        detector->detectAndCompute(original_trainDescriptors, cv::Mat(), trainKeypoints, trainDescriptors);
                        rootSift(trainDescriptors);

                        trainingData tData = {trainDescriptors,original_trainDescriptors,original_luminance,profile_id};
                        data.push_back(tData);
                        hash_map.insert({++counter,data});

                    }catch( cv::Exception& e ){
                        const char* err_msg = e.what();
                        std::cout << "exception db " << err_msg << " caught on camera  " << capture_data[1]  << std::endl;
                    }
                    
                }

                int pos = msg.find(":");
                std::string frame_number = msg.substr(0 , pos);
                std::string frame_raw_data = msg.substr(pos + 1);

                std::vector<unsigned char> pic_data(frame_raw_data.begin(), frame_raw_data.end());
                cv::Mat vmat(pic_data, true);
                cv::Mat face= cv::imdecode(vmat, 1);
                
                tbb::concurrent_hash_map<int,  std::vector<trainingData> >::const_iterator it = hash_map.begin();
                while (it != hash_map.end()){
                    int index = it->first;
                    std::unique_ptr<int> ptr (new int(index));
                    hash_map_accessor accessor;
                    const auto isFound = hash_map.find(accessor, *ptr);
                    if (isFound ==true){ 
                        int idx = *ptr;
                        //perform search
                        std::vector<trainingData> TrainDescriptors = accessor->second;
                        std::vector<cv::DMatch> matches;
                        cv::Mat queryDescriptors;
                        std::vector<cv::KeyPoint> queryKeypoints;
                        cv::resize(face, face, Size(48, 56), 0, 0, INTER_LINEAR);
                        detector->detectAndCompute(face, cv::Mat(), queryKeypoints, queryDescriptors);
                        rootSift(queryDescriptors); 
                        matcher->match(queryDescriptors, TrainDescriptors[idx-1].Tdesc, matches);
                        
                        float eudist ,dist = 0.0;
                        for(int j = 0; j < TrainDescriptors[idx-1].Tdesc.cols; j++) {
                            dist += (TrainDescriptors[idx-1].Tdesc.at<float>(matches[0].trainIdx, j) - queryDescriptors.at<float>(matches[0].queryIdx, j))
                                * (TrainDescriptors[idx-1].Tdesc.at<float>(matches[0].trainIdx, j) - queryDescriptors.at<float>(matches[0].queryIdx, j));
                        }
                    
                        eudist = std::sqrt(dist);
                        float distance = (1.0 - eudist)*100;
                    
                        if(distance >= stoi(poiThreshold)){
                            //score
                            std::ostringstream oss;
                            oss << distance;
                            auto score = oss.str();

                            cv::Mat live_dFace = face.clone();
                            cv::Mat live_pFace = TrainDescriptors[idx-1].Tdata.clone();
                            cv::Mat live_dect(cv::Size(124, 124),CV_8UC3 ,Scalar(254, 254, 253));
                            cv::resize(live_dFace, live_dFace, Size(112, 112), 0, 0, INTER_CUBIC);
                            (&live_dFace)->copyTo(live_dect(cv::Rect(6,6,(&live_dFace)->cols, 
                            (&live_dFace)->rows)));
                            cv::resize(live_dect, live_dect, Size(112, 112), 0, 0, INTER_CUBIC);  
                            cv::resize(live_pFace, live_pFace, Size(112, 112), 0, 0, INTER_CUBIC);

                            //score
                            std::string _live_score  = score;
                            int live_pos = _live_score.find(".");
                            std::string _live_score_left = _live_score.substr(0 , live_pos);
                            _live_score_left = _live_score_left + ".";
                            std::string _live_score_right= _live_score.substr(live_pos + 1);
                            _live_score_right = _live_score_right.substr(0, 3) + "%";

                            cv::Mat live_rec(cv::Size(380, 170),CV_8UC3 ,Scalar(0, 0, 0));
                            cv::Mat live_rec_left(cv::Size(125, 145),CV_8UC3 ,Scalar(42, 32, 23));
                            cv::Mat live_rec_right(cv::Size(123, 145),CV_8UC3 ,Scalar(22, 30, 100));

                            putText(live_rec_left,_live_score_left , Point(72,140), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(254, 254, 253), 2.0, CV_MSA);
                            putText(live_rec_right,_live_score_right, Point(5,140), FONT_HERSHEY_COMPLEX_SMALL, 1.5, Scalar(254, 254, 253), 2.0, CV_MSA);
                
                            (&live_dect)->copyTo(live_rec_left(cv::Rect(5,5,(&live_dect)->cols, (&live_dect)->rows)));
                            (&live_pFace)->copyTo(live_rec_right(cv::Rect(5,5,(&live_pFace)->cols, (&live_pFace)->rows)));

                            (&live_rec_left)->copyTo(live_rec(cv::Rect(60,5,(&live_rec_left)->cols, (&live_rec_left)->rows)));
                            (&live_rec_right)->copyTo(live_rec(cv::Rect(180,5,(&live_rec_right)->cols, (&live_rec_right)->rows)));

                            live_face_count++;
                            
                            if (live_face_count==1){
                                int face_pos = 0;
                                cv::imencode(".png", live_rec, *recognised_face_buffer);
                                std::string _one_face_payload((*recognised_face_buffer).begin(), (*recognised_face_buffer).end());
                                std::string one_face_payload = std::to_string(face_pos) +":"+ _one_face_payload;
                                _redis->publish(camera_id+"-recog", one_face_payload);
                            }

                            int live_margin_top = live_face_count + ((&live_rec)->rows * (live_face_count-1));
                            if(live_face_count>1){
                                cv::imencode(".png", live_rec, *recognised_face_buffer);
                                std::string _face_payload((*recognised_face_buffer).begin(), (*recognised_face_buffer).end());
                                std::string face_payload = std::to_string(live_margin_top) +":"+ _face_payload;
                                _redis->publish(camera_id+"-recog",face_payload);
                            }

                            if(live_face_count==5){
                                live_face_count = 0;
                            }
                            sleep(0.5);
                            //captured
                            std::vector<uint8_t> captured_face_buffer;
                            std::stringstream captured_face_super_string;
                            cv::imencode(".png", face, captured_face_buffer);
                            for (auto c : captured_face_buffer) captured_face_super_string << c;
                            std::string  captured_face_payload = score +":"+ captured_face_super_string.str();
                            
                            //target
                            std::vector<uint8_t> target_face_buffer;
                            std::stringstream target_face_super_string;
                            cv::imencode(".png", TrainDescriptors[idx-1].Tdata, target_face_buffer);
                            for (auto _c : target_face_buffer) target_face_super_string << _c;
                            std::string  target_face_payload = TrainDescriptors[idx-1].userId +":"+ target_face_super_string.str();
                            _redis->publish(camera_id+"-cf", captured_face_payload);
                            _redis->publish(camera_id+"-tf", target_face_payload);
                            std::cout << " hash map container size " << hash_map.size() << " query " <<queryDescriptors.cols << " target " << TrainDescriptors[idx-1].Tdesc.cols << " score " << distance <<std::endl;
                            
                        }
                    }
                    it++;
                }
                //data.clear();
                hash_map.clear();
                counter=0;
            }
        });

        sub.subscribe({capture_data[1]+"-poi"});

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
