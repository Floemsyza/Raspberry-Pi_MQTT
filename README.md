# Raspberry-Pi_MQTT
Raspberry Pi'ı, MQTT Broker ve client olacak şekilde kullanmak için ESP8266 ile haberleşmesine yarayan kodları bulundurmaktadır. Gördüğüm kadarıyla bilgi eksikliği olan bu alandaki bendenizin deneyimlerinden yola çıkarak oluşturulmuş kodlar ve yönergeleri içerir.

## AŞAĞIDAKİ YAZI MEDIUM YAZIMDAN ALINTIDIR. GÖRSELLİ ANLATIM İÇİN MEDIUMDAKİ YAZIMI KONTROL EDEBİLİRSİNİZ. ##

Raspberry Pi ve ESP8266’yı MQTT ile nasıl bağlarız?

IoT projelerimizde Raspberry Pi ve ESP kartlarını kablosuz haberleştirmeye ihtiyaç duyabiliriz (biz duyduk). Bunun için bazı teknikler olmakla birlikte internette bilgi eksikliği olduğunu düşündüğüm bu konuda yazmaya karar verdim. Bu çeşitli tekniklerden de bir tanesi MQTT Broker kullanmak. Öncelikle temelden başlayalım: MQTT ve MQTT Broker Nedir?
MQTT (Message Queuing Telemetry Transport), kısaca hafif bir mesajlaşma protokolü. IoT’nin temeli de diyebiliriz. MQTT Broker da bu protokolü kullanan bir mesajlaşma sisteminin merkezinde yer alan sunucudur. 

Yazıda paylaşacağım kodlar bizzat projemizin kendisinde kullanılan kodlar olduğu için ESP8266’dan sıcaklık/nem sensörlerinin (DHT11) ve hava kalitesi sensörünün (MQ-135) verilerinin gönderip Raspberry Pi üzerinden okuyacağız.



Adım 1:  MQTT’’yi anladığımıza göre öncelikle Raspberry Pi üzerindeki Broker kurulumlarını yapacağız. Güncellemeleri yaparak başlayalım. Terminalde bunları kodlayalım:
sudo apt-get update
sudo apt-get upgrade
Güncellemelerimizi yaptık, şimdi MQTT broker örneği olan “mosquitto”yu indireceğiz:
sudo apt-get install mosquitto -y 
sudo apt-get install mosquitto-clients -y 
(BREAKPOINT: Eğer terminalde bunları yazdığınızda uyarı mesajı alıyorsanız sanal ortam içine kurulum yapmayı deneyebilir ya da genel kurulum yapmak için aşağıdaki komutları kullanabilirsiniz: 
sudo apt-get install mosquitto -y --break-system-packages
sudo apt-get install mosquitto-clients -y --break-system-packages )
Bunları yaptıktan sonra mosquitto’nun konfigürasyonlarını yapmamız gerekli:
sudo nano /etc/mosquitto/mosquitto.conf
bu dosyayı bulduğunuzda “include_dir /etc/mosquitto/conf.d”  bu satırı silin ve bu satırı ekleyin: “listener 1883 “ satırını ekleyin. 1883 numaralı bağlantı noktasındaki mesajları dinlemesini söyledik. 
(BREAKPOINT: Daha güvenlikli bir sistem oluşturmak adına mosquitto’ya bir şifre ve kullanıcı adı vermeniz önerilir. Bunu yapmak isterseniz, listener satırının altına bu satırları da eklemelisiniz: 
 allow_anonymous false
password_file /etc/mosquitto/pwfile ) (bunlara kullanıcı adı ve şifre atamalarını da birazdan anlatacağım.)
Şimdi bu düzenlemeleri yaptığımız dosyayı kaydedip kapatacağız. CTRL + x tuşlarını kullanın ve E’ye basın (İngilizce kullanıyorsanız Y). 
(Eğer şifre ve kullanıcı adı eklediyseniz: 
sudo mosquitto_passwd -c /etc/mosquitto/pwfile username 
komutunu kullanın username yazan yere kendi seçtiğiniz kullanıcı adını yazın Sonrasında sizden bir şifre isteyecek. Buraya da belirlediğiniz şifreyi girebilirsiniz.)
Konfigürasyonlar üzerinde değişiklikler yaptığımız için yeniden başlatıyoruz:
sudo reboot
Artık bir MQTT Broker’a sahibiz.
Adım 2: Brokerımızın çalışıp çalışmadığına bakacağız şimdi. 2 adet terminal ekranı açıyoruz ve bir tanesine 
mosquitto_sub -d -t test 
bu komutu, ötekine de 
mosquitto_pub -d -t test -m "Hello, World!"
bu komutu yazıyoruz. (buradaki -t’ler abone olmak istediğiniz “topic” ismini temsil eder. -d hata ayıklama modudur, bir hatayla karşılaşırsanız ekrana bi çıktı verir. -m yayınlamak istediğiniz mesajdır.)
(BREAKPOINT: Eğer kullanıcı adı ve şifre koyduysanız bu komutları girin sırasıyla: 
mosquitto_sub -d -u username -P password -t test
mosquitto_pub -d -u username -P password -t test -m "Hello, World!"
username ve password sizin belirlediğinizdir.)

Eğer bu adımlar sonucunda diğer terminalde yazdığınız mesajı görebiliyorsanız bu adım başarıyla gerçekleşmiştir. 

Adım 3: ESP8266 ile alakalı kurulumları yapacağız şimdi. (Kodlara ve devre kurulumuna Github hesabımdan ulaşabilirsiniz.) Kodlamada Arduino IDE kullanacağız. Dosya > Tercihler > Kart Yöneticileri URL’sine  “http://arduino.esp8266.com/stable/package_esp8266com_index.json” adresini yapıştırın. Araçlar > Kartlar> Kart yöneticisi kısmını açın ve ESP8266’yı bulup indirin (esp8266 by ESP8266 Community şeklinde olmalı.). Nodemcu 1.0 şeklindeki kartı seçebilirsiniz. Şimdi github üzerinden  indirdiğiniz ya da kendi düzenlediğiniz kodları ESP’ye yükleyin. Kod üzerinde size ait olan bilgileri değiştirmeyi unutmayın. Bu kodlamayı yaparken Arduino IDE’de MQTT haberleşmesi için PubSubClient kütüphanesini kullanacağız. 
Adım 4: Python Client yani istemcisini kuracağız. Bunu isterseniz kendi bilgisayarınızda isterseniz de Raspberry Pi üzerinde kurabilirsiniz. Bu yazıda Raspberry üzerinde kurmayı göstereceğim. Genel kurulum için aşağıdaki komutu kullanın: 
sudo pip3 install paho-mqtt --break-system-packages
(Sanal ortam üzerinde kurmak için de
pip install paho-mqtt
komutunu kullanabilirsiniz.)
Adım 5: Subscribe olma aşamasına geldik. Bu adımda da yine sizinle paylaştığım Python kodunu Raspberry’nin içine atabilirsiniz. Kendinize ait değişkenleri değiştirmeyi unutmayın. 

En sonunda her şeyi çalıştırmaya başladığınızda bir IoT cihazları ağı elde etmiş olacağız. Birbiri arasında mesajlaşan daha karmaşık bir ağ da kurabiliriz. Bu sizin projenizin büyüklüğüne göre değişecektir. İnternette bu alanda gördüğüm kaynak eksikliği üzerine deneyimleyerek edindiğim bu bilgiler umarım faydalı olmuştur. IoT ile kalın!!!


