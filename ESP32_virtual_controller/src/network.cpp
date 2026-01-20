#include <WiFi.h>
#include <WiFiUdp.h>

static WiFiUDP udp;
static const int PORT = 14550;  //can change

void network_begin(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
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