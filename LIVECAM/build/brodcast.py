import os
import signal
import string
import requests
import simplejson as json
from flask_pymongo import PyMongo
from flask_cors import CORS, cross_origin
from flask import Flask, flash, request, redirect, render_template ,jsonify

app=Flask(__name__)


cors = CORS(app, resources={

    r"/*": {
        "origins": "*"

    }
})

app.secret_key = "secret key"

import sys
import os.path
sys.path.append(os.path.abspath(os.path.join(
    os.path.dirname(__file__), os.path.pardir)))

app.config['MONGO_DBNAME'] = 'photo_bomb'
app.config['MONGO_URI'] = 'mongodb://192.168.0.51:27017/photo_bomb'

mongo = PyMongo(app)

#BROADCAST UPON STARTING CAMERA
@app.route('/controls/api/broadcast/start/<id>' , methods=['POST'])
def channels(id):
    url = "http://127.0.0.1:5008"
    payload ={'id':str(id)}
    headers = {'Content-type': 'application/json'}
    requests.post(url, data=json.dumps(payload), headers=headers,timeout=10)
    return '200'

#BROADCAST UPON STOPING CAMERA
@app.route('/controls/api/broadcast/stop/<id>' , methods=['POST'])
def processes(id):
    '''
    url = "http://127.0.0.1:5019"
    payload ={'pid':str(id)}
    headers = {'Content-type': 'application/json'}
    requests.post(url, data=json.dumps(payload), headers=headers,timeout=10)
    '''
    try:
        os.kill(int(id), signal.SIGTERM) #or signal.SIGKILL 
    except:
        print('process not found')
    return '200'
   
if __name__ == "__main__":
    app.run(host='0.0.0.0',port=5007,use_reloader=False)