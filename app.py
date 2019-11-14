import paho.mqtt.client as mqtt
from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit

app = Flask(__name__,static_url_path='/static')
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
# The callback for when the client receives a CONNACK response from the server.


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/esp8266/uid")
    client.subscribe("/esp8266/access")

# The callback for when a PUBLISH message is received from the ESP8266.


def on_message(client, userdata, message):
    #socketio.emit('my variable')
    print("Received message '" + str(message.payload) + "' on topic '"
          + message.topic + "' with QoS " + str(message.qos))
    if message.topic == "/esp8266/uid":
        print("UID update")
        socketio.emit('uid', {'data': str(message.payload)})
    if message.topic == "/esp8266/access":
        print("ACCESS update")
        socketio.emit('access', {'data': str(message.payload)})


mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("raspberry pi IP (broker)", 1883, 60)
mqttc.loop_start()

# Create a dictionary called pins to store the pin number, name, and pin state:
pins = {
    0: {'name': 'GPIO 0', 'board': 'esp8266', 'topic': '/esp8266/0', 'state': 'False'},
    2: {'name': 'GPIO 2', 'board': 'esp8266', 'topic': '/esp8266/2', 'state': 'False'}
}

# Put the pin dictionary into the template data dictionary:
templateData = {
    'pins': pins
}


@app.route("/")
def main():
    # Pass the template data into the template main.html and return it to the user
    return render_template('main.html', async_mode=socketio.async_mode, **templateData)

# The function below is executed when someone requests a URL with the pin number and action in it:
@app.route("/<board>/<changePin>/<action>")
def action(board, changePin, action):
    # Convert the pin from the URL into an integer:
    changePin = int(changePin)
    # Get the device name for the pin being changed:
    devicePin = pins[changePin]['name']
    # If the action part of the URL is "1" execute the code indented below:
    if action == "1" and board == 'esp8266':
        mqttc.publish(pins[changePin]['topic'], "1")
        pins[changePin]['state'] = 'True'
    if action == "0" and board == 'esp8266':
        mqttc.publish(pins[changePin]['topic'], "0")
        pins[changePin]['state'] = 'False'
    # Along with the pin dictionary, put the message into the template data dictionary:
    templateData = {
        'pins': pins
    }
    return render_template('main.html', **templateData)


@socketio.on('my event')
def handle_my_custom_event(json):
    print('received json data here: ' + str(json))


if __name__ == "__main__":
    socketio.run(app, host='localhost', port=8181, debug=True)
