import paho.mqtt.client as mqtt

# MQTT broker bilgileri
mqtt_username = "raspberrypi"
mqtt_password = "mqttDemo123"
mqtt_topic1 = "node1"  # 1. Topic: node1 (Temperature, Humidity, Air Quality)
mqtt_topic2 = "node2"  # 2. Topic: node2 (Acceleration)
mqtt_broker_ip = "192.168.124.20"  # MQTT broker IP adresi

client = mqtt.Client()
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)

# Bu fonksiyon, MQTT client broker'a bağlandığında çalışır
def on_connect(client, userdata, flags, rc):
    # rc, broker'a bağlanırken dönen hata kodudur
    print("Connected! Return code: " + str(rc))
    
    # Bağlantı başarılı olduktan sonra iki konuya abone ol
    client.subscribe(mqtt_topic1)  # node1 konusuna abone ol
    client.subscribe(mqtt_topic2)  # node2 konusuna abone ol
    
def on_message(client, userdata, msg):
    # Herhangi bir topic'e mesaj geldiğinde bu fonksiyon çalışır
    print("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    
    # Mesaj geldiğinde yapılacak işlemler:
    if msg.topic == mqtt_topic1:
        # node1 verisi geldiğinde, sıcaklık, nem ve hava kalitesini işleme
        print("Received data from node1:", str(msg.payload))
        # Burada veriyi ayrıştırıp işlemeniz gerekebilir.
        
    elif msg.topic == mqtt_topic2:
        # node2 verisi geldiğinde, ivme verisini işleme
        print("Received data from node2:", str(msg.payload))
        # Burada ivme verisini işleyebilirsiniz.

# Bu fonksiyonları MQTT client'a bağlıyoruz
client.on_connect = on_connect
client.on_message = on_message

# MQTT broker'a bağlanıyoruz
client.connect(mqtt_broker_ip, 1883)

# Bağlantı sağlandıktan sonra sürekli dinleme işlemi başlatılıyor
client.loop_forever()
