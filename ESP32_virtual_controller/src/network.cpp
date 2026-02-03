#include <WiFi.h>
#include <WiFiUdp.h>

static WiFiUDP udp;
static const int PORT = 14550;  //can change

// Choose static IP
IPAddress local_IP(192, 168, 0, 69); // ESP32's fixed IP
IPAddress gateway(192, 168, 0, 1);   // Router IP
IPAddress subnet(255, 255, 255, 0);  // Subnet mask
IPAddress dns(8, 8, 8, 8);         // DNS server

void network_begin(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);

    // Apply static IP BEFORE Wifi.begin()
    if (!WiFi.config(local_IP, gateway, subnet, dns)) {
        Serial.println("Static IP config failed");
    }

    WiFi.begin("ADSS", "ADSS69420");

    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
    }
    udp.begin(PORT);
}

int network_receive(char* buffer, size_t maxLen) {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        int len = udp.read(buffer, maxLen - 1);
        if (len > 0) {
            buffer[len] = '\0'; // Null-terminate the received data
        }
        return len;
    }
    return 0; // No packet received
}