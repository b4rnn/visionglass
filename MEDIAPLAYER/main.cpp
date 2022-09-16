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

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <opencv2/core.hpp>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include <sw/redis++/redis++.h>
#include <experimental/filesystem>

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/hwcontext.h>
}

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
mongocxx::uri uri("mongodb://192.168.0.51:27017");
mongocxx::client client(uri);
mongocxx::database db = client["photo_bomb"];


int main(int argc, char* argv[])
{
    av_register_all();
    avcodec_register_all();
    avfilter_register_all();
    avformat_network_init();

    vector<string> capture_data(argv, argv+argc);
    capture_data.erase(capture_data.begin());

    //find facial threshold
    auto _cursor = db["live_cams"].find(document{} 
        << "_id"
        << bsoncxx::oid{capture_data[1]}
        << finalize
    );

    std::string frame_rate;
    for(auto doc : _cursor) {
        bsoncxx::document::element FPS = doc["frame_rate"];
        frame_rate = FPS.get_utf8().value.to_string();
    }
       
    std::string id = capture_data[1];

    //GATHER INDIVIDUAL LIVE CAM METRICS
    VideoCapture *capture = new VideoCapture(capture_data[0] , cv::CAP_FFMPEG);
    //Width
    int frame_width = capture->get(cv::CAP_PROP_FRAME_WIDTH); 
    //Height
    int frame_height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);

    //capture->release();

    //ENCODER
    auto start = std::chrono::system_clock::now();

    AVFormatContext *format_ctx = avformat_alloc_context();
    std::string videoUrl  = capture_data[0];
    const char *outFname  = videoUrl.c_str();

    int directory_status,_directory_status;
    string folder_name = "/var/www/html/live/"+capture_data[1];
    const char* dirname = folder_name.c_str();
    directory_status = mkdir(dirname,0777);

    auto Timestamps =  std::chrono::duration_cast<std::chrono::nanoseconds>
    (std::chrono::system_clock::now().time_since_epoch()).count();
    string sub_folder_name = folder_name+"/"+std::to_string(Timestamps);
    const char* _dirname = sub_folder_name.c_str();
    _directory_status =mkdir(_dirname,0777);
        
    if(capture_data.size() > 0){

        auto  CHANNEL = [frame_rate,frame_width,frame_height,&capture_data,outFname,sub_folder_name,_directory_status,Timestamps](const std::string& key){
            //ENCODER
            std::string segment_list_type = sub_folder_name + "/stream.m3u8";
            auto start = std::chrono::system_clock::now();
            AVFormatContext *format_ctx = avformat_alloc_context();

            avformat_alloc_output_context2(&format_ctx, nullptr, "hls", segment_list_type.c_str());
            if(!format_ctx) {
                return 1;
            }

            if (!(format_ctx->oformat->flags & AVFMT_NOFILE)) {
                int avopen_ret  = avio_open2(&format_ctx->pb, outFname,
                                            AVIO_FLAG_WRITE, nullptr, nullptr);
                if (avopen_ret < 0)  {
                    std::cout << "failed to open stream output context, stream will not work! " <<  outFname << std::endl;
                    return 1;
                }
            }

            AVCodec *out_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
            AVStream *out_stream = avformat_new_stream(format_ctx, out_codec);
            AVCodecContext *out_codec_ctx = avcodec_alloc_context3(out_codec);
            //set codec params
            const AVRational dst_fps = {stoi(frame_rate), 1};
            out_codec_ctx->codec_tag = 0;
            out_codec_ctx->codec_id = AV_CODEC_ID_H264;
            out_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
            out_codec_ctx->width = frame_width;
            out_codec_ctx->height = frame_height;
            //out_codec_ctx->bit_rate = 50000;
            out_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
            out_codec_ctx->framerate = dst_fps;
            //out_codec_ctx->skip_frame = AVDISCARD_BIDIR;
            out_codec_ctx->time_base.num = 1;
            out_codec_ctx->gop_size = stoi(frame_rate);
            out_codec_ctx->time_base.den = stoi(frame_rate);
            /*
            out_codec_ctx->thread_count = 1;
            out_codec_ctx->thread_type = FF_THREAD_SLICE;
            out_codec_ctx->gop_size = stoi(frame_rate);
            out_codec_ctx->time_base.den = stoi(frame_rate); // fps
            */
            
            out_codec_ctx->time_base = av_inv_q(dst_fps);
            if (format_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                out_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }

            int ret_avp = avcodec_parameters_from_context(out_stream->codecpar, out_codec_ctx);
            if (ret_avp < 0)
            {
                std::cout << "Could not initialize stream codec parameters!" << std::endl;
                exit(1);
            }


            int av_ret;
            int frames_validator;
            int frames_counter = 0;
            AVFrame *frame = av_frame_alloc();
            std::vector<uint8_t>* imgbuf = new std::vector<uint8_t>(frame_height * frame_width * 3 + 16);
            std::vector<uint8_t>* framebuf = new std::vector<uint8_t>(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, frame_width, frame_height, 1));
            cv::Mat* image = new cv::Mat(frame_height, frame_width, CV_8UC3, imgbuf->data(), frame_width * 3);
                        
            if (!_directory_status){
                //append last charcter from key
                std::string pktPos = key.c_str();
                std::string hls_segment_filename = sub_folder_name + "/"+pktPos.back()+"file%03d.ts";
                std::cout << "HLS SEGMENT " <<  hls_segment_filename << std::endl;
                //update dp
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                std::time_t end_time = std::chrono::system_clock::to_time_t(end);

                std::ostringstream oss;
                oss << std::ctime(&end_time);
                auto timestamp = oss.str();

                std::istringstream iss(timestamp);

                std::vector<std::string> captured_time{istream_iterator<std::string>{iss},istream_iterator<std::string>{}};

                //Get Time
                std::time_t Dtime;
                std::tm* Tinfo;
                char Tbuffer [80];

                std::time(&Dtime);
                Tinfo = std::localtime(&Dtime);

                std::strftime(Tbuffer,80,"%Y-%m-%d:%H:%M:%S",Tinfo);
                std::puts(Tbuffer);
                string _time(Tbuffer);

                db["recordings"].insert_one(document{} 
                    << "camera_id"      <<  capture_data[1]
                    << "recording_id"   <<  std::to_string(Timestamps)
                    << "time"           <<  _time
                    << "recording_time" <<  timestamp
                    << "time_day"       <<  captured_time[0]
                    << "time_month"     <<  captured_time[1]
                    << "time_date"      <<  captured_time[2]
                    << "time_year"      <<  captured_time[4]
                    << "time_score"     <<  std::to_string(Timestamps)
                    << "recording_url"  <<  segment_list_type.erase(0,13)
                    << "utc_time"       <<  bsoncxx::types::b_date(std::chrono::system_clock::now())
                    << finalize
                );

                db["live_cams"].update_one(document{} << "_id" << bsoncxx::oid{capture_data[1]}<< finalize,
                    document{}<<"$set" << bsoncxx::builder::stream::open_document <<"camera_player"
                    << segment_list_type << bsoncxx::builder::stream::close_document
                    <<finalize
                );

                AVDictionary *hlsOptions = NULL;
                av_dict_set(&hlsOptions, "profile", "baseline", 0);
                av_dict_set(&hlsOptions, "c", "copy", 0);
                av_dict_set(&hlsOptions, "preset", "ultrafast", 0);
                av_dict_set(&hlsOptions, "tune", "zerolatency", 0);
                av_dict_set(&hlsOptions, "x264encopts", "slow_firstpass:ratetol=100", 0);
                av_dict_set(&hlsOptions, "pass", "2", 0);
                av_dict_set(&hlsOptions, "crf", "28", 0); 
                av_dict_set(&hlsOptions, "strict", "-2", 0);
                av_dict_set(&hlsOptions, "hls_segment_type",   "mpegts", 0);
                av_dict_set(&hlsOptions, "segment_list_type",  "m3u8",   0);
                av_dict_set(&hlsOptions, "hls_segment_filename", hls_segment_filename.c_str(),   0);
                av_dict_set(&hlsOptions, "g",frame_rate.c_str(), 0);
                av_dict_set(&hlsOptions, "hls_time",   "1", 0);
                av_dict_set(&hlsOptions, "hls_init_time",   "1", 0);
                av_dict_set(&hlsOptions, "hls_list_size",   "0", 0);
                av_dict_set(&hlsOptions, "live_start_index", "0", 0);
                av_dict_set(&hlsOptions, "segment_time_delta", "1.0", 0);
                av_dict_set_int(&hlsOptions, "reference_stream",   out_stream->index, 0);
                av_dict_set(&hlsOptions, "force_key_frames", "expr:gte(t,n_forced*10)", AV_DICT_DONT_OVERWRITE);
                
                /*
                av_dict_set(&hlsOptav_dict_set(&hlsOptions, "setpts=0.75*PTS", "0", 0);ions,     "hls_playlist_type", "vod", 0);
                av_dict_set(&hlsOptions,     "segment_time_delta", "1.0", 0);
                av_dict_set(&hlsOptions,     "hls_flags", "append_list", 0);
                av_dict_set(&hlsOptions, "live_start_index", "-10", 0);
                av_dict_set(&hlsOptions, "hls_playlist_type", "vod", 0);
                av_dict_set_int(&hlsOptions, "reference_stream",   out_stream->index, 0);
                av_dict_set(&hlsOptions, "segment_list_flags", "cache+live", 0);
                av_dict_set(&hlsOptions, "force_key_frames", "expr:gte(t,n_forced*3)", AV_DICT_DONT_OVERWRITE);
                */

                int ret_avo = avcodec_open2(out_codec_ctx, out_codec, &hlsOptions);
                
                if (ret_avo < 0)
                {
                    std::cout << "Could not open video encoder!" << std::endl;
                    exit(1);
                }
                out_stream->codecpar->extradata = out_codec_ctx->extradata;
                out_stream->codecpar->extradata_size = out_codec_ctx->extradata_size;

                av_dump_format(format_ctx, 0, outFname, 1);

                SwsContext * pSwsCtx = sws_getContext(frame_width,frame_height,AV_PIX_FMT_BGR24, frame_width, frame_height , AV_PIX_FMT_YUV420P , SWS_FAST_BILINEAR, NULL, NULL, NULL);
                        
                if (pSwsCtx == NULL) {
                    fprintf(stderr, "Cannot initialize the sws context\n");
                    exit(1);
                    return -1;
                }
                

                int ret_avfw = avformat_write_header(format_ctx, &hlsOptions);
                if (ret_avfw < 0)
                {
                    std::cout << "Could not write header!" << std::endl;
                    exit(1);
                }
                
                Redis *redis = new Redis("tcp://127.0.0.1:6379?keep_alive=true&socket_timeout=100ms&connect_timeout=100ms");
                auto sub = redis->subscriber();
                sub.on_message([frame,framebuf,frame_width,frame_height,image,out_codec_ctx,out_stream,format_ctx,pSwsCtx,segment_list_type,&capture_data](std::string channel, std::string msg) {
                    std::cout <<" CHANNELS " << channel <<  std::endl;
                    
                    int pos = msg.find(":");
                    
                    std::string frame_number = msg.substr(0 , pos);
                    std::string frame_raw_data = msg.substr(pos + 1);

                    std::vector<unsigned char> pic_data(frame_raw_data.begin(), frame_raw_data.end());
                    cv::Mat vmat(pic_data, true);
                    cv::Mat video_frame = cv::imdecode(vmat, 1);
                    
                    av_image_fill_arrays(frame->data, frame->linesize, framebuf->data(), AV_PIX_FMT_YUV420P, frame_width, frame_height, 1);
                    frame->width = frame_width;
                    frame->height = frame_height;
                    frame->format = static_cast<int>(AV_PIX_FMT_YUV420P);
                    
                    *image = video_frame;

                    const int stride[] = {static_cast<int>(image->step[0])};
                    sws_scale(pSwsCtx, &image->data, stride , 0 , frame_height , frame->data, frame->linesize); 
                    frame->pts += av_rescale_q(1, out_codec_ctx->time_base, out_stream->time_base);

                    //std::cout <<frame->pts << " WIDTH " <<  frame->width <<" HEIGHT " << frame->height << " LENGTH "<< frame_raw_data.length() << " FOLDER " << segment_list_type << std::endl;
                    
                    AVPacket pkt = {0};
                    av_init_packet(&pkt);

                    if(out_stream != NULL){
                        
                        int ret_frame = avcodec_send_frame(out_codec_ctx, frame);
                        if (ret_frame < 0)
                        {
                            std::cout << "Error sending frame to codec context!" << std::endl;
                            exit(1);
                        }
                        int ret_pkt = avcodec_receive_packet(out_codec_ctx, &pkt);
                        if (ret_pkt < 0)
                        {
                            std::cout << "Error receiving packet from codec context!" << " WIDTH " <<  frame_width <<" HEIGHT " << frame_height << std::endl;
                            exit(1);
                        }

                        if (pkt.pts == AV_NOPTS_VALUE || pkt.dts == AV_NOPTS_VALUE) {
                            av_log (format_ctx, AV_LOG_ERROR,
                                "Timestamps are unset in a packet for stream% d\n", out_stream-> index);
                            return AVERROR (EINVAL);
                            exit(1);
                        }

                        if (pkt.pts < pkt.dts) {
                            av_log (format_ctx, AV_LOG_ERROR, "pts%" PRId64 "<dts%" PRId64 "in stream% d\n",
                            pkt.pts, pkt.dts,out_stream-> index);
                            return AVERROR (EINVAL);
                            exit(1);
                        }
                        if (pkt.stream_index == out_stream->index){
                            av_interleaved_write_frame(format_ctx, &pkt);
                        }
                    }
                    // sub.subscribe({capture_data[0]+"-ONE"});
                    
                });

                // Subscribe to channels and patterns.
                sub.subscribe(key);
                sub.consume();
                //sub.subscribe({capture_data[0]+"-0",capture_data[0]+"-1",capture_data[0]+"-2",capture_data[0]+"-3",capture_data[0]+"-4",capture_data[0]+"-5"});

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
            }
        };

        thread t1(std::thread(CHANNEL, capture_data[1]+"-cnc"));
        //thread t2(std::thread(CHANNEL, capture_data[1]+"-notify"));

        t1.join();
        //t2.join();
    }
    
    return 0;
}
