#pragma once

void network_begin(const char* ssid, const char* password);
int network_receive(char* buffer, size_t maxLen);