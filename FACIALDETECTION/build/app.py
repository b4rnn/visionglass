import os
import signal
import string
import random
import subprocess
import itertools
import operator
import itertools
from werkzeug.utils import secure_filename
from flask_pymongo import PyMongo
from flask_cors import CORS, cross_origin
from bson.objectid import ObjectId
from bson.json_util import dumps
from flask import Flask, flash, request, redirect, render_template ,jsonify

app=Flask(__name__)

cors = CORS(app, resources={

    r"/*": {
        "origins": "*"

    }
})

app.secret_key = "secret key"
app.config['MONGO_DBNAME'] = 'photo_bomb'
app.config['MONGO_URI'] = 'mongodb://DATABASE_IP:27017/photo_bomb'
mongo = PyMongo(app)

@app.route('/' , methods=['POST'])
def channels():
    if request.method == 'POST':
        dict=request.json
        id =dict.get('id')
        camera_details = {}
        camera_details_list = []
        cameras = mongo.db.live_cams
        for  camera in cameras.find({'_id' : { "$eq" : ObjectId(str(id))}}):
            camera_details = {str(id+':'+camera['camera_ip'])}
            print(camera_details)
            camera_details_list.append(camera_details)
        proc = subprocess.Popen(["./ENGINE",str(camera_details_list)])
    return '200'
