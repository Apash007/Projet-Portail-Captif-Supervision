import paho.mqtt.client as mqtt
import subprocess

MQTT_BROKER = "127.0.0.1"
MQTT_PORT = 1883
ZABBIX_SERVER = "127.0.0.1"
ZABBIX_HOST = "ESP32-Capteur"

def send_to_zabbix(key, value):
    cmd = ["zabbix_sender", "-z", ZABBIX_SERVER, "-s", ZABBIX_HOST, "-k", key, "-o", str(value)]
    subprocess.run(cmd, capture_output=True)
    print(f"Envoye a Zabbix : {key} = {value}")

def on_message(client, userdata, msg):
    topic = msg.topic
    value = msg.payload.decode("utf-8")
    print(f"MQTT recu : {topic} = {value}")
    if "temperature" in topic:
        send_to_zabbix("esp32.temperature", value)
    elif "humidite" in topic:
        send_to_zabbix("esp32.humidite", value)

client = mqtt.Client()
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe("iot/#")
print("Pont MQTT Zabbix demarre. En attente de donnees...")
client.loop_forever()
