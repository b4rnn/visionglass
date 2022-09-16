# DATABASE

Load balancing , storage and broadcast.
*DATABASE CONTAINER CONSISTS OF*

   *`NGINX`
    `FLASK`
    `UWSGI`
    `MONGO DB`

# SETUP
# Dependencies
```
sudo apt-get install libssl-dev
sudo apt-get install qtbase5-dev
sudo apt-get install qtdeclarative5-dev
sudo apt-get install libhdf5-serial-dev
sudo apt-get install build-essential git unzip pkg-config
sudo apt-get install libjpeg-dev libpng-dev libtiff-dev
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install libgtk2.0-dev
sudo apt-get install python3-dev python3-numpy python3-pip
sudo apt-get install libxvidcore-dev libx264-dev libgtk-3-dev
sudo apt-get install libtbb2 libtbb-dev libdc1394-22-dev
sudo apt-get install libv4l-dev v4l-utils
sudo apt-get install liblapacke-dev
sudo apt install default-jre
sudo apt install -y mongodb
sudo apt install pkg-config
sudo apt install libcurl3-dev
sudo apt-get install redis-tools
sudo apt-get install libboost-all-dev
sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
sudo apt-get install libavresample-dev libvorbis-dev libxine2-dev
sudo apt-get install libfaac-dev libmp3lame-dev libtheora-dev
sudo apt-get install libopencore-amrnb-dev libopencore-amrwb-dev
sudo apt-get install libopenblas-dev libatlas-base-dev libblas-dev
sudo apt-get install liblapack-dev libeigen3-dev gfortran
sudo apt-get install libprotobuf-dev libgoogle-glog-dev libgflags-dev
sudo apt install libpcre3-dev libssl-dev zlib1g-dev
sudo apt install libavcodec-dev libavformat-dev libswscale-dev libavresample-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev libvorbis-dev python3-dev libatlas-base-dev gfortran libgtk-3-dev libv4l-dev
sudo apt install libcanberra-gtk0 libcanberra-gtk-common-dev libcanberra-gtk3-0 libcanberra-gtk-dev libcanberra-gtk3-dev  libcanberra-gtk-module libcanberra-gtk3-module 
```
# CMAKE >=3.19

```
cd /tmp && mkdir CMAKETMP && cd CMAKETMP
See [Install cmake from source](https://github.com/Kitware/CMake.git)
git clone https://github.com/Kitware/CMake.git
cd cmake directory then ..
./bootstrap && make && sudo make install

```
# OPENCV 4.5.0 
```
cd /tmp && mkdir OPENCVTMP && cd OPENCVTMP
wget -O opencv.zip https://github.com/opencv/opencv/archive/master.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/master.zip
unzip opencv.zip
unzip opencv_contrib.zip
# Create build directory and switch into it
mkdir -p build && cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE  \
-D CMAKE_C_COMPILER=/usr/bin/gcc-7 \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib-master/modules ../opencv-master \
-D BUILD_TIFF=ON \
-D WITH_FFMPEG=ON \
-D WITH_GSTREAMER=ON \
-D WITH_TBB=ON \
-D BUILD_TBB=ON \
-D WITH_EIGEN=ON \
-D OPENCV_ENABLE_NONFREE=ON \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D PYTHON_DEFAULT_EXECUTABLE=$(which python3) \
-D BUILD_SHARED_LIBS=OFF \
-D BUILD_NEW_PYTHON_SUPPORT=ON \
-D OPENCV_GENERATE_PKGCONFIG=ON \
-D OPENCV_PC_FILE_NAME=opencv4.pc \
-D OPENCV_ENABLE_NONFREE=ON \
-D BUILD_EXAMPLES=OFF ..
```
```
make -j$(nproc)
sudo make install
sudo ldconfig
pkg-config --modversion opencv4
```
# libmongoc and libbson
```

sudo apt-get install libbson-1.0-0
sudo apt-get install libmongoc-1.0-0
```
# mongo-c driver

```
cd /tmp && mkdir MONGOCDRIVERTMP && cd MONGOCDRIVERTMP
git clone https://github.com/mongodb/mongo-c-driver.git
cd mongo-c-driver
git checkout 1.17.0
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
cmake --build .
sudo cmake --build . --target install
sudo cmake --build . --target uninstall [--]
```
# mongocxx-3.6.x

```
cd /tmp && mkdir MONGOCXXTMP && cd MONGOCXXTMP
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.5/mongo-cxx-driver-r3.6.5.tar.gz
tar -xzf mongo-cxx-driver-r3.6.5.tar.gz
cd mongo-cxx-driver-r3.6.5/build
cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX=/usr/local
sudo cmake --build . --target EP_mnmlstc_core
cmake --build .
sudo cmake --build . --target install
sudo cmake --build . --target uninstall [--]
```

#  hiredis
```
cd /tmp && mkdir HIREREDISTMP && cd HIREREDISTMP
git clone https://github.com/redis/hiredis.git
cd hiredis
make
sudo make install
```

#  redis-plus-plus
```
cd /tmp && mkdir REDISCPPTMP && cd REDISCPPTMP
git clone https://github.com/sewenew/redis-plus-plus.git
cd redis-plus-plus
mkdir build
cd build
cmake ..
make
sudo make install
```

# NGINX SERVER  + RTMP 
```
sudo apt install nginx

#Nginx Directories
    #File Directories
     cd /var/www/html/ && mkdir uploads && mkdir images && mkdir quotas && mkdir cbi && mkdir recordings && mkdir recordings/videos && mkdir recordings/cams && cd ..
```
# python 3.5 +
```
#update & upgrade
    sudo apt update
    sudo apt install software-properties-common
#repo
    sudo add-apt-repository ppa:deadsnakes/ppa
    Press [ENTER] to continue or Ctrl-c to cancel adding it.
#install
    sudo apt install python3.7
#check
    python3.7 --version
```
# pip 
```
#install
    python3 -m pip install --upgrade pip
#upgrade
    pip3 install --upgrade setuptools
```

# CONDITIONS 
```
#[ENABLE] redis-server service
   sudo systemctl enable mongodb
#[RESTART] redis-server service
   sudo systemctl restart mongodb
#[STOP] redis-server service
   sudo systemctl stop mongodb
#[STATUS] redis-server service
   sudo systemctl status mongodb
```
# CONFIG 
```
sudo lsof -i | grep mongo
sudo vi /etc/mongodb.conf
#REMOTE CONNECTION
   [BEFORE] bind 127.0.0.1
   [AFTER]  bind 127.0.0.1,[MONGO_SERVER_IP]
#RESTART
   sudo systemctl restart mongodb
#STATUS
   sudo systemctl status mongodb
```
# PORTS 
```
flask --5005 
mongodb --27107
uwsgi --5007:5018
sudo ufw allow 5007/tcp
sudo ufw allow 5007/udp
sudo ufw allow 27107/tcp
sudo ufw allow 27107/udP
sudo ufw enable
```
# CONNECTION TESTING ON TERMINAL
``` 
[REDIS]
    [QUERY] redis-cli -h [REDIS_SERVER_IP] ping
    [RESPONSE] PONG
[MONGODB]
    sudo ufw allow from [LIVESTREAM_IP] to any port 27017
    sudo ufw allow from [PERSON OF INTEREST_IP] to any port 27017
    sudo ufw allow from [MEDIAPLAYER_IP] to any port 27017
    sudo ufw allow from [NOTIFICATIONS_IP] to any port 27017
    sudo ufw allow from [FACIAL DETECTION_IP] to any port 27017
    sudo ufw allow from [ALERTS_IP] to any port 27017
    sudo ufw allow from [FACIAL RECOGNITION_IP] to any port 27017
```
# DB
```
    #DATABASE
        photo_bomb
    #COLLECTIONS
        live_cams
        live_cams_locations
        person_of_interest
        pids
        profiles
        recordings
``` 
# 
``` 
#EXPORT DB FROM MOMGO FOR TESTING ON TERMINAL
    #LOCATIONS
        sudo mongoexport --db photo_bomb -c live_cams --out dblive_cams_locations.json
    #LIVECAMS
        sudo mongoexport --db photo_bomb -c live_cams_locations --out dblive_cams_locations.json
``` 
# 
```
#IMPORT DB INTO MONGO FOR TESTING ON TERMINAL
    #LOCATIONS
        mongorestore -h [MONGO_DB_IP]:27017 -d photo_bomb --nsInclude dblive_cams_locations.json
    #LIVECAMS
        mongorestore -h [MONGO_DB_IP]:27017 -d photo_bomb --nsInclude dblive_cams.json
``` 
# BROADCAST
```
cd DATABASE 
pip3 install -r requirements.txt
sudo vi /etc/nging/sites-enabled/default

#paste text[from default]
    
#test nginx server
    sudo nginx -t
    sudo systemctl restart nginx
    sudo systemctl status nginx

cd back to   projcet FOLDER 
    python3 broadcast.py

#test using postman (127.0.0.1:5007)
```
# default
```
server {
        listen 80;
        listen [::]:80;
        #server_name  str.appb.casa;
        #ssl_certificate      /etc/letsencrypt/live/str.appb.casa/fullchain.pem;
        #ssl_certificate_key  /etc/letsencrypt/live/str.appb.casa/privkey.pem;

        root /var/www/html;
        index index.html;

      location /videos/ {
	        #add_header 'Access-Control-Allow-Origin' '*' always;
    	    proxy_pass http://localhost:5005;
	    }
        
      location /live/ {
          add_header 'Access-Control-Allow-Origin' '*' always;
	    }

      location /recordings/ {
          add_header 'Access-Control-Allow-Origin' '*' always;
	    }
	
      location /profiles/ {
         #proxy_pass http://localhost:5005;
      }

      location /controls/ {
	      proxy_pass http://localhost:5007;
      }
    
      location /api/ {
	      proxy_pass http://localhost:5007;
      }
}

#BROADCAST
geo $limit {
    default 1;
}
	 
map $limit $limit_key {
	    0 "";
	    1 $binary_remote_addr;
}

limit_req_zone $limit_key zone=req_zone:10m rate=8r/s;
#offline

#START[ALL SINGLE CAMERA NODES]
server {
	listen 5008;
	listen [::]:5008;

	server_name _;

	#root /var/www/html;
	root /usr/share/nginx/html;
	index index.html;

	location / {
			# First attempt to serve request as file, then
			# as directory, then fall back to displaying a 404.
			limit_req zone=req_zone burst=1 nodelay;
			proxy_pass http://127.0.0.1:5009;
			proxy_pass_request_headers on;
			proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
			proxy_set_header Host $http_host;
			proxy_set_header X-Forwarded-Proto $scheme;
			proxy_redirect off;
			proxy_connect_timeout       300;
			proxy_send_timeout          300;
			proxy_read_timeout          300;
			send_timeout                300;
			proxy_method POST;
			mirror /ALERTS;
            mirror /LIVECAM;
            mirror /MEDIAPLAYER;
            mirror /NOTIFICATIONS;
            mirror /FACIALDETECTION; 
            mirror /PERSONOFINTEREST;
            mirror /FACIALRECOGNITION;

            mirror_request_body on;
            try_files $uri $uri/ =404;
	}

    #local server
    location /LIVECAM                    	    { internal;proxy_pass http://LIVECAM_IP:5010$request_uri; }
    location /MEDIAPLAYER                	    { internal;proxy_pass http://MEDIAPLAYER_IP:5011$request_uri; }
    location /NOTIFICATIONS              	    { internal;proxy_pass http://NOTIFICATIONS_IP:5015$request_uri; }
    location /FACIALDETECTION            	    { internal;proxy_pass http://FACIALDETECTION_IP:5012$request_uri; }
    location /FACIALRECOGNITION                 { internal;proxy_pass http://FACIALRECOGNITION_IP:5014$request_uri; }
    location /PERSONOFINTEREST      	        { internal;proxy_pass http://PERSONOFINTEREST_IP:5017$request_uri; }
    location /ALERTS                            { internal;proxy_pass http://ALERTS_IP:5018$request_uri; }
}
```
