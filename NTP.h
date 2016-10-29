/*
    NTP.h - Network Time Protocol Functions

    Portions of this code were extracted from the
    Time library examples by Michael Margolis and
    Paul Stoffregen. Other portions from the NTPClient
    example program from the Arduino ESP8266 package.

    Implementation by: TomSoft
    Last Update: 12.08.2016
*/

#ifndef NTP_H
#define NTP_H

#include <WiFiUdp.h>

#define LOCALPORT     2390 // Local port to listen for UDP packets, can also be 8888
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

byte packetBuffer[NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

// Don't hardwire the IP address or we won't get the benefits of the time server pool.
IPAddress timeServerIP;
const char *ntpServerName = "fritz.box";

// Send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {

  // Set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // All NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); // NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// NTP Time Provider Code
time_t getNTPTime() {

  int attempts = 10;

  // Try multiple attempts to return the NTP time
  while (attempts--) {

    // Get a server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP);
#ifdef DEBUG
    Serial.print("Time server IP address: ");
    Serial.println(timeServerIP);
#endif

    while (udp.parsePacket() > 0); // Discard any previously received packets

#ifdef DEBUG
    Serial.println("Transmitted NTP Request");
#endif
    sendNTPpacket(timeServerIP);

    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
      int size = udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) {
#ifdef DEBUG
        Serial.println("Received NTP Response");
#endif
        udp.read(packetBuffer, NTP_PACKET_SIZE);  // Read packet into the buffer
        time_t secsSince1900;

        // Convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long) packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long) packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long) packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long) packetBuffer[43];
        time_t secsSince1970 = secsSince1900 - 2208988800UL;

#ifdef DEBUG
        Serial.println("Got the time");
#endif

        return secsSince1970;
      }
      delay(10);
    }
#ifdef DEBUG
    Serial.println("Retrying NTP request");
#endif
    delay(4000);
  }
#ifdef DEBUG
  Serial.println("No NTP Response");
#endif
  return 0;
}

// Open UPD port and assign the time sync provider
void initNTP() {

  // set UDP local port
  udp.begin(LOCALPORT);

}

#endif


