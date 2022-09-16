#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <spawn.h>
#include <iostream>
#include <algorithm>
#include "helpers.hh"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::stdx::string_view;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

mongocxx::instance instance{};
mongocxx::uri uri("mongodb://DATABASE_IP:27017");
mongocxx::client client(uri);
mongocxx::database db = client["photo_bomb"];

using namespace std;
vector<string> capture_destination;

int main(int argc, char* argv[])
{
   
    vector<string> _capture_source(argv, argv+argc);
    _capture_source.erase(_capture_source.begin());
    std::cout << " POST DATA  " << _capture_source[0] << " VIDEOS FINISHED " << std::endl;

    vector<string> capture_source;
    vector<string> capture_data;
 
    std::stringstream superVideo(_capture_source[0]);
    string video;
    while (superVideo >> video)
    {
        capture_data.push_back(video);
        if (superVideo.peek() == ',')
            superVideo.ignore();
    }

    char chars[] = "[-{,\'}_]";

    for (int j=0;j<capture_data.size();j++)
    {
        std::string data = capture_data.at(j);
        for (unsigned int i = 0; i < strlen(chars); ++i)
        {
            data.erase(std::remove(data.begin(), data.end(), chars[i]), data.end());
        }
        std::string _video = data.substr(data.find(":")+1);
        capture_source.push_back(_video);
        std::string _channels = data.substr(0 ,data.find(":"));
        capture_destination.push_back(_channels);
    }

    fprintf(stderr, "About to spawn ALERTS process from pid %d\n", getpid());

    pid_t p;

    const char* args[] = {
        "./ALERTS",
        capture_source[0].c_str(),
        capture_destination[0].c_str(),
        "ALERTSPID",
        nullptr,
        nullptr
    };

    int r = posix_spawn(&p, "./ALERTS", nullptr, nullptr,
                       (char**) args, nullptr);

    fprintf(stderr, "Child pid %d should run my ALERTS\n", p);

    char * ALERTSPID =(char*) malloc(6);
    sprintf(ALERTSPID, "%d", p);

    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = db["live_cams"].find_one(document{} << "_id" << bsoncxx::oid{capture_destination[0]} << finalize);
    if(maybe_result) {
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";

        db["live_cams"].update_one(document{} << "_id" << bsoncxx::oid{capture_destination[0]} << finalize,
            document{}<<"$set" << bsoncxx::builder::stream::open_document <<"alerts_cam_pid"
            << ALERTSPID << bsoncxx::builder::stream::close_document
            <<finalize
        );
    }

    std::cout << capture_destination[0] << " " << p << " " << ALERTSPID << " ID " << capture_destination[0] <<std::endl;
    
return 0;
}
