# PRODUCTION SETUP
*VISIONGLASS box consists of 8 containers*

   * `ALERTS`
    `DATABASE`
    `MEDIA PLAYER`
    `NOTIFICATIONS`
    `FACIAL DETECTION`
    `FACIAL RECOGNITION`
    `PERSON OF INTEREST`
    `VIDEO FRAME GRABBING / LIVESTREAM`
    
* See individual containers (optimized for CPU) for a distributed system setup.
# GOAL
```
 Analyze live cctv videos and record results as m3u8 
```

# OBJECTIVES
```
Grab Video frames
Detect Faces
Extract Faces
Compare Faces
Low-latency streaming
High-quality video for display and recording
Notify admin of any potential Person of Interest
Publish recognised and detected faces for analysis.
Update database with the right data based on metrics
```
# ARCHICTECTURE
```

![My Remote Image](https://www.dropbox.com/s/.../my-remote-image.jpg?dl=0)

```
# METRICS
```
ALL METRICS CAN BE AJUSTED BY ADMIN.HOWEVER, SPECIFIC CAMERA MUST BE RESTARTED FOR CHANGE TO TAKE EFFECT.
BITRATE [ ??]
FRAMERATE [DEFAULT = 15 FPS ]
POITHRESHOLD [DEFAULT = 30  ]
[INCOMING / OUTGOING CHANNEL COUNT PER NODE]
CAMERA THREAD IN PLAYER OR RECORDER = 1 THREAD/CAMERA
CAMERA MEMORY IN MB [(1 *1920*1080*3)%(1024*1024)]*FPS]~6MB*FPS
CAMERA COUNT PER NODE = [ ((NODE-MEMORY-2GB)*1024)%([[(1 *1920*1080*3)%(1024*1024)]*C]~6MB*C)]~(8GB-2GB)%(6MB*C)
CAMERA COUNT PER LIVE CAME OR DISPLAY NODE = [ ((NODE-MEMORY-2GB)*1024)%([[(1 *1920*1080*3)%(1024*1024)]*FPS*C]~6MB*FPS)]~(8GB-2GB)%(6MB*FPS)
```

# SUMMARY
```
| SERVER            | IP ADRRESS    | STORAGE | CPUS | MEMORY | DEFAULT FPS | THRESHOLD | LATENCY | OS           |  PORTS       |
| DATABASE          | 192.168.9.209 |  50 GB  |  8   | N/A        | Medium |  UBUNTU 18.0X|  5007/27107  |
| LIVESTREAM        | 192.168.8.203 |  50 GB  |  16  |  8     |  20         |  N/A      |  LOW    |  UBUNTU 18.0X|  5010        | 
| STREAM            | 192.168.8.209 |  50 GB  |  16  |  32    |  N/A        |  N/A      |  VLOW   |  UBUNTU 18.0X|  6379        | 
| PERSON OF INTEREST| 192.168.8.207 |  50 GB | 16 | 8 |  8     |  N/A        |  30       |  LOW    |  UBUNTU 18.0X|  5017        | 
| MEDIAPLAYER       | 192.168.8.202 |  50 GB | 16 | 8 |  8     |  15         |  N/A      |  LOW    |  UBUNTU 18.0X|  5011        | 
| NOTIFICATIONS     | 192.168.8.204 |  50 GB  |  16  |  8     |  N/A        |  N/A      |  LOW    |  UBUNTU 18.0X|  5015        | 
| FACIAL DETECTION  | 192.168.8.205 |  50 GB  |  16  |  8     |  N/A        |  N/A      |  LOW    |  UBUNTU 18.0X|  5012        | 
| ALERTS            | 192.168.8.208 |  50 GB  |  16  |  8     |  N/A        |  N/A      |  LOW    |  UBUNTU 18.0X|  5018        | 
| MEDIARECORDER     | 192.168.9.204 | 1.5 TB  |  16  |  8     |  15         |  N/A      |  LOW    |  UBUNTU 18.0X|  ??          | 
| FACIAL RECOGNITION| 192.168.8.206 |  50 GB  |  16  |  8     |  N/A        |  N/A      |  LOW    |  UBUNTU 18.0X|  5014        | 
```

# NOTICE
```
ALWAYS REMOVE SOURCE FILES FROM PRODUCTION SERVER UPON SUCCESSFUL DEPLOYMENT EXCEPT PYTHON SCRIPTS
```
