#include "mbed.h"

WiFiInterface *wifi;
#define MQTTCLIENT_QOS2 1
 
#include "MQTTmbed.h"
#include "MQTTClientMbedOs.h"
 
DigitalOut mosfet(PB_6);
AnalogIn sensor(PA_0);

#define MQTT_HOST "192.168.42.1"
#define MQTT_PORT 1883
 
void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    if (message.payload == "Do"){
        
    mosfet = 1;
    thread_sleep_for(1000);
    mosfet = 0;
    thread_sleep_for(1000);}
    
}

void send_mqtt(NetworkInterface *net, char* topic, char* _message)
{
    float version = 0.6;
    
    TCPSocket socket;
    MQTTClient client(&socket);
    
 
    SocketAddress a;
    net->gethostbyname(MQTT_HOST, &a);
    a.set_port(MQTT_PORT);
 
    printf("Connecting to %s:%d\r\n", MQTT_HOST, MQTT_PORT);
    socket.open(net);
    printf("Opened socket\n\r");
    int rc = socket.connect(a);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);
    printf("Connected socket\n\r");
 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "client-2198912930293";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);
 
    if ((rc = client.subscribe(topic, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);
 if ((rc = client.subscribe("/class/stand3/pump", MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);
    MQTT::Message message;
 
    // QoS 0
    char buf[sizeof(_message)];
    sprintf(buf, _message, version);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
 
    //if ((rc = client.unsubscribe(topic)) != 0)
//        printf("rc from unsubscribe was %d\r\n", rc);
 
    if ((rc = client.disconnect()) != 0)
        printf("rc from disconnect was %d\r\n", rc);
 
    //_socket.close();
 
    printf("Version %.2f: finish %d msgs\r\n", version, 0);
 
    return;
}
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("Scan:\n");

    int count = wifi->scan(NULL,0);

    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);

    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;
    return count;
}



int main()
{
    printf("WiFi example\n");

    #ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    int count = scan_demo(wifi);
    if (count == 0) {
        printf("No WIFI APs found - can't continue further.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret == -3015) {
        printf("Already connected!\n\r");
    }
    else if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi->get_mac_address());
    SocketAddress a;
    wifi->get_ip_address(&a);
    printf("IP: %s\n", a.get_ip_address());
    wifi->get_netmask(&a);
    printf("Netmask: %s\n", a.get_ip_address());
    wifi->get_gateway(&a);
    printf("Gateway: %s\n", a.get_ip_address());
    printf("RSSI: %d\n\n", wifi->get_rssi());

    
        
        
        
        
    float version = 0.6;
    char* topic = "/class/stand3/pump";
 
    TCPSocket socket;
    MQTTClient client(&socket);
 
    char* hostname = "192.168.42.1";
    wifi->gethostbyname(hostname, &a);
    int port = 1883;
    a.set_port(port);
 
    printf("Connecting to %s:%d\r\n", hostname, port);
 
    socket.open(wifi);
    printf("Opened socket\n\r");
    int rc = socket.connect(a);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);
    printf("Connected socket\n\r");
 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-samplee4234edasda";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);
 
    if ((rc = client.subscribe(topic, MQTT::QOS2, messageArrived)) != 0)
        printf("rc from MQTT subscribe is %d\r\n", rc);
 
    MQTT::Message message;
 
    // QoS 0
    char buf[100];
    sprintf(buf, "Hello World!  QoS 0 message from app version %f\r\n", version);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (1 < 99999){
        char sensorstr[100];
        
        client.yield(100);
        float value = sensor.read() * 100;
        snprintf(sensorstr, sizeof(sensorstr), "%f\n", value);
        printf("Sensor: %f\n", value);
//        send_mqtt(wifi, "jhhjhj", sensorstr);
char _buf[100];
    sprintf(_buf, "Humidity sensor: %f\n", value);
message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
        rc = client.publish(topic, message);
        if (value<5){
            send_mqtt(wifi, "/class/stand3/pump", "watering");
            mosfet = 1;
            thread_sleep_for(1000);
            mosfet = 0;
            thread_sleep_for(1000);
            }
 }
    // QoS 1
    sprintf(buf, "Hello World!  QoS 1 message from app version %f\r\n", version);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    //while (arrivedcount < 2)
//        client.yield(100);
// 
//    while (arrivedcount < 3)
//        client.yield(100);
 
 
    if ((rc = client.unsubscribe(topic)) != 0)
        printf("rc from unsubscribe was %d\r\n", rc);
 
    if ((rc = client.disconnect()) != 0)
        printf("rc from disconnect was %d\r\n", rc);
 
    socket.close();
 
    printf("Version %.2f: finish %d msgs\r\n", version, 1);
 
    return 0;    
        
        
        
        
        
        
        
        
        
    //send_mqtt(wifi, "/class/stand3/state", "I am alive!");
//    while(1) {
//        char sensorstr[100];
//        float value = sensor.read() * 100;
//        snprintf(sensorstr, sizeof(sensorstr), "%f\n", value);
//        printf("Sensor: %f\n", value);
////        send_mqtt(wifi, "jhhjhj", sensorstr);
//        if (value<5){
//            send_mqtt(wifi, "/class/stand3/pump", "watering");
//            mosfet = 1;
//            thread_sleep_for(1000);
//            mosfet = 0;
//            thread_sleep_for(1000);
//            }
//        //if (value >= 40){
////            
////            }
//        thread_sleep_for(500);
//    }
    wifi->disconnect();

    printf("\nDone\n");
    
   
}
