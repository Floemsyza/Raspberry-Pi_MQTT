//---------------OTOBÜS TAKİP SİSTEMİ İÇİN NODE2'NİN KODLARI---------------------

#include <ESP8266WiFi.h> //esp8266'nın kütüphane tanımlaması
#include <PubSubClient.h>  // Raspberry pi üzerindeki mqtt broker'dan veri almak ve Broker'a veri göndermek için kütütphane
#include <DHT.h> //dht11'in tanımlamaları için kütüphane
#include <MQ135.h> //MQ135 hava kalite sensörü için kütüphane

// -----------------DHT11 Ayarları---------------------------
#define DHTPIN D1 // DHT11'in bağlandığı pin
#define DHTTYPE DHT11 // Sensör tipi: DHT11
DHT dht(DHTPIN, DHTTYPE);

//----------------- MQ135 Ayarları---------------------------
#define MQ135PIN A0 // MQ135'in bağlandığı pin
MQ135 mq135_sensor(MQ135PIN);

//---------------- WiFi Ayarları----------------------------
const char* ssid = "xxxx"; // WiFi adı
const char* wifi_password = "xxxxxx"; // WiFi şifresi

// -----------------MQTT Ayarları---------------------------
const char* mqtt_server = "xxxxxxxx"; // MQTT brokerın IP adresi
const char* mqtt_topic = "node2"; // MQTT'nin bağlanılacak Konusu (kendi konunuzu isteğinize göre değiştirebilirsiniz.)
const char* mqtt_username = "raspberrypi"; // MQTT brokerın kullanıcı adı (kendi kullanıcı adınızı isteğinize göre değiştirebilirsiniz.)
const char* mqtt_password = "xxxxxx"; // brokerın şifresi (kendi şifresinizi isteğinize göre değiştirebilirsiniz.)
const char* clientID = "esp_node2"; // ESP istemci ID'si (istemci adı mühim değildir değiştirilebilir ancak birden çok istemciyle çalışıyorsanız adların aynı olması karışıklık yapabilir.)

WiFiClient wifiClient; // Wificlint sınıfına ait wificlint nesnesi 
PubSubClient client(mqtt_server, 1883, wifiClient); // 1. boşluk bağlanılacak server, 2. boşluk broker üzerinde dinlenilen adres (genelde 1883) olur, 3. boşluk wificlint sınıfının nesnesi

void setup() {
  Serial.begin(9600);
  dht.begin(); // DHT11 sensörünü başlat
  Serial.println("DHT11 ve MQ135 Sensör Okuma Başlatıldı");

  //----------------- WiFi'ye bağlan-------------------------
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //------------------- MQTT Broker'a bağlan----------------------
  client.setServer(mqtt_server,1883);
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  } else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void loop() {
  //--------------------- Sensör verilerini oku --------------------------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float airQuality = mq135_sensor.getPPM(); // MQ135'ten hava kalitesi verisi al

  //--------------------- Hata kontrolü-------------------------------
  if (isnan(temperature) || isnan(humidity)) { //sensörden veri okunamadığında bildiriyor.
    Serial.println("DHT11 sensöründen veri okunamadı!");
    return;
  }

  //------------------ Hava kalitesi durumunu belirle---------------------------
  String airQualityStatus;
  if (airQuality <= 50) {
    airQualityStatus = "good";
  } else if (airQuality <= 100) {
    airQualityStatus = "middle";
  } else {
    airQualityStatus = "bad";
  }

  //------------------- MQTT Brokera Mesaj oluştur -------------------------------
  String mqttMessage = "Temperature: " + String(temperature)+ ", "
                       + "Humidity: " + String(humidity) + " %, "
                       + "Air quality(PPM): " + String(airQuality) + " -> Durum: " + airQualityStatus;

  // -----------------------MQTT Mesajını yayınla---------------------------------
  if (client.publish(mqtt_topic, mqttMessage.c_str())) {
    Serial.println("MQTT Mesajı Gönderildi: " + mqttMessage);
  } else {
    Serial.println("MQTT Mesajı Gönderilemedi. MQTT Broker'a yeniden bağlanılıyor...");
    client.connect(clientID, mqtt_username, mqtt_password); //mesajın doğru kanala iletilmesi için doğrulayıcılar
  }

  //----------------- Sonuçları seri portta yazdır-----------------
  Serial.println("------ Sensör Verileri ------");
  Serial.println(mqttMessage);
  Serial.println("-----------------------------");

  delay(5000); // 5 saniye bekle
}