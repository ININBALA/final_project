import paho.mqtt.client as paho

import time

import matplotlib.pyplot as plt

import numpy as np
import logging


logging.basicConfig(level=logging.INFO, filename='mylog.log', filemode='w')

# https://os.mbed.com/teams/mqtt/wiki/Using-MQTT#python-client


# MQTT broker hosted on local machine

mqttc = paho.Client()


# Settings for connection

# TODO: revise host to your ip

host = "192.168.44.146"

topic = "Mbed"


# Callbacks
sd = ""
def on_connect(self, mosq, obj, rc):

      print("Connected rc: " + str(rc))


def on_message(mosq, obj, msg):

        print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
        global sd
        sd = str(msg.payload)


def on_subscribe(mosq, obj, mid, granted_qos):

      print("Subscribed OK")


def on_unsubscribe(mosq, obj, mid, granted_qos):

      print("Unsubscribed OK")


# Set callbacks

mqttc.on_message = on_message

mqttc.on_connect = on_connect

mqttc.on_subscribe = on_subscribe

mqttc.on_unsubscribe = on_unsubscribe


# Connect and subscribe

print("Connecting to " + host + "/" + topic)

mqttc.connect(host, port=1883, keepalive=60)

mqttc.subscribe(topic, 0)


# Publish messages from Python

num = 0
i = 0
while i < 40:

      ret = mqttc.publish(topic, "Message from Python!\n", qos=0)

      if (ret[0] != 0):
            print("Publish failed")
      print(sd)
      logging.info(sd)
      mqttc.loop()

      time.sleep(1)

      num += 1


# Loop forever, receiving messages
s.close()
mqttc.loop_forever()