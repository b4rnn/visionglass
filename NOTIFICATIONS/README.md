# NOTIFICATIONS

Aligns raw data from various nodes and publish them for encoding.

# SETUP

*NOTIFICATIONS has been optimised to run on cpu . No need for GPU.*
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
# PORTS 
``` 
redis --6379
uwsgi --5015
sudo ufw allow 5015/tcp
sudo ufw allow 5015/udp
```
# CONNECTION TESTING ON TERMINAL
``` 
[REDIS]
    [QUERY] redis-cli -h [REDIS_SERVER_IP] ping
    [RESPONSE] PONG
[MONGODB]
    [QUERY] nc -zv [MONGODB_SERVER_IP] 27017
    [RESPONSE] Connection to [MONGODB_SERVER_IP] 27017 port [tcp/*] succeeded!
```
# DEPLOYMENT

```
REPLACE [STREAM_IP] WITH ACTUAL REDIS SERVER IP I.E 192.168.9.208 [LOCATE STREAM_IP FROM main.cpp ]
REPLACE [DATABASE_IP] WITH ACTUAL MONGO DB SERVER IP I.E 192.168.9.209 [LOCATE DATABASE_IP FROM engine.cpp ]
REPLACE [DATABASE_IP] WITH ACTUAL MONGO DB SERVER IP I.E 192.168.9.209 [LOCATE DATABASE_IP FROM build/app.py ]

cd to  NOTIFICATIONS FOLDER
    pip3 install -r requirements.txt
    cd build  && rm -rf CMakeCache.txt ENGINE  CMakeFiles NOTIFICATIONS Makefile  __pycache__  cmake_install.cmake 
    cmake ..
    make -j{NUMBER_OF_THREADS}
#If successful ... you will have the compiled project name i.e NOTIFICATIONS
    CMakeCache.txt  CMakeFiles  NOTIFICATIONS app.ini  Makefile  app.py  cmake_install.cmake  libfacedetect.so wsgi.py
#start flask server
    uwsgi --ini app.ini
```
