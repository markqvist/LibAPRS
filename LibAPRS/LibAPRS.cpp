#include "Arduino.h"
#include "AFSK.h"
#include "AX25.h"

Afsk modem;
AX25Ctx AX25;
extern void aprs_msg_callback(struct AX25Msg *msg);
#define countof(a) sizeof(a)/sizeof(a[0])

int LibAPRS_vref = REF_3V3;
bool LibAPRS_open_squelch = false;

unsigned long custom_preamble = 350UL;
unsigned long custom_tail = 50UL;

AX25Call src;
AX25Call dst;
AX25Call path1;
AX25Call path2;

#define MAX_CALL_LENGTH 7
char CALL[MAX_CALL_LENGTH] = "NOCALL";
int CALL_SSID = 0;
char DST[MAX_CALL_LENGTH] = "APZMDM";
int DST_SSID = 0;
char PATH1[MAX_CALL_LENGTH] = "WIDE1";
int PATH1_SSID = 1;
char PATH2[MAX_CALL_LENGTH] = "WIDE2";
int PATH2_SSID = 2;
uint8_t MICE_MSG;
uint8_t MICE_SSID;

AX25Call path[4];

// Location packet assembly fields
char latitude[9];
char longtitude[10];
char symbolTable = '/';
char symbol = 'n';

uint8_t power = 10;
uint8_t height = 10;
uint8_t gain = 10;
uint8_t directivity = 10;
uint16_t speed;
uint16_t course;
/////////////////////////

// Message packet assembly fields
char message_recip[7];
int message_recip_ssid = -1;

int message_seq = 0;
char lastMessage[67];
size_t lastMessageLen;
bool message_autoAck = false;
/////////////////////////

void APRS_init(int reference, bool open_squelch) {
    LibAPRS_vref = reference;
    LibAPRS_open_squelch = open_squelch;

    AFSK_init(&modem);
    ax25_init(&AX25, aprs_msg_callback);
}

void APRS_poll(void) {
    ax25_poll(&AX25);
}

void APRS_setCallsign(char *call, int8_t ssid) {
    memset(CALL, 0, MAX_CALL_LENGTH);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        CALL[i] = call[i];
        i++;
    }
    CALL_SSID = ssid;
}

void APRS_setDestination(char *call, int8_t ssid) {
    memset(DST, 0, MAX_CALL_LENGTH);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        DST[i] = call[i];
        i++;
    }
    DST_SSID = ssid;
}

void APRS_setPath1(char *call, int8_t ssid) {
    memset(PATH1, 0, MAX_CALL_LENGTH);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        PATH1[i] = call[i];
        i++;
    }
    PATH1_SSID = ssid;
}

void APRS_setPath2(char *call, int8_t ssid) {
    memset(PATH2, 0, MAX_CALL_LENGTH);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        PATH2[i] = call[i];
        i++;
    }
    PATH2_SSID = ssid;
}

void APRS_setMessageDestination(char *call, int8_t ssid) {
    memset(message_recip, 0, 6);
    int i = 0;
    while (i < 6 && call[i] != 0) {
        message_recip[i] = call[i];
        i++;
    }
    message_recip_ssid = ssid;
}

void APRS_setPreamble(unsigned long pre) {
    custom_preamble = pre;
}

void APRS_setTail(unsigned long tail) {
    custom_tail = tail;
}

void APRS_useAlternateSymbolTable(bool use) {
    if (use) {
        symbolTable = '\\';
    } else {
        symbolTable = '/';
    }
}

void APRS_setSymbol(char sym) {
    symbol = sym;
}

void APRS_setLat(char *lat) {
    memset(latitude, 0, 9);
    int i = 0;
    while (i < 8 && lat[i] != 0) {
        latitude[i] = lat[i];
        i++;
    }
}

void APRS_setLon(char *lon) {
    memset(longtitude, 0, 10);
    int i = 0;
    while (i < 9 && lon[i] != 0) {
        longtitude[i] = lon[i];
        i++;
    }
}

void APRS_setPower(int s) {
    if (s >= 0 && s < 10) {
        power = s;
    }
}

void APRS_setHeight(int s) {
    if (s >= 0 && s < 10) {
        height = s;
    }
}

void APRS_setGain(int s) {
    if (s >= 0 && s < 10) {
        gain = s;
    }
}

void APRS_setDirectivity(int s) {
    if (s >= 0 && s < 10) {
        directivity = s;
    }
}

// Set the speed in knots. Valid speeds are 0-799 knots
void APRS_setSpeed(int s) {
    if (s >= 0 && s < 800) {
        speed = s;
    } else {
        speed = 0;
    }
}

// Set the course, valid courses are 0-360 where 0 is unknown and 360 is due north
void APRS_setCourse(int c) {
    if (c >= 0 && c <= 360) {
        course = c;
    } else {
        course = 0;
    }
}

void APRS_printSettings() {
    Serial.println(F("LibAPRS Settings:"));
    Serial.print(F("Callsign:     ")); Serial.print(CALL); Serial.print(F("-")); Serial.println(CALL_SSID);
    Serial.print(F("Destination:  ")); Serial.print(DST); Serial.print(F("-")); Serial.println(DST_SSID);
    Serial.print(F("Path1:        ")); Serial.print(PATH1); Serial.print(F("-")); Serial.println(PATH1_SSID);
    Serial.print(F("Path2:        ")); Serial.print(PATH2); Serial.print(F("-")); Serial.println(PATH2_SSID);
    Serial.print(F("Message dst:  ")); if (message_recip[0] == 0) { Serial.println(F("N/A")); } else { Serial.print(message_recip); Serial.print(F("-")); Serial.println(message_recip_ssid); }
    Serial.print(F("TX Preamble:  ")); Serial.println(custom_preamble);
    Serial.print(F("TX Tail:      ")); Serial.println(custom_tail);
    Serial.print(F("Symbol table: ")); if (symbolTable == '/') { Serial.println(F("Normal")); } else { Serial.println(F("Alternate")); }
    Serial.print(F("Symbol:       ")); Serial.println(symbol);
    Serial.print(F("Power:        ")); if (power < 10) { Serial.println(power); } else { Serial.println(F("N/A")); }
    Serial.print(F("Height:       ")); if (height < 10) { Serial.println(height); } else { Serial.println(F("N/A")); }
    Serial.print(F("Gain:         ")); if (gain < 10) { Serial.println(gain); } else { Serial.println(F("N/A")); }
    Serial.print(F("Directivity:  ")); if (directivity < 10) { Serial.println(directivity); } else { Serial.println(F("N/A")); }
    Serial.print(F("Latitude:     ")); if (latitude[0] != 0) { Serial.println(latitude); } else { Serial.println(F("N/A")); }
    Serial.print(F("Longtitude:   ")); if (longtitude[0] != 0) { Serial.println(longtitude); } else { Serial.println(F("N/A")); }
}

void APRS_sendPkt(void *_buffer, size_t length) {

    uint8_t *buffer = (uint8_t *)_buffer;

    memcpy(dst.call, DST, 6);
    dst.ssid = DST_SSID;

    memcpy(src.call, CALL, 6);
    src.ssid = CALL_SSID;

    memcpy(path1.call, PATH1, 6);
    path1.ssid = PATH1_SSID;

    memcpy(path2.call, PATH2, 6);
    path2.ssid = PATH2_SSID;

    path[0] = dst;
    path[1] = src;
    path[2] = path1;
    path[3] = path2;

    ax25_sendVia(&AX25, path, countof(path), buffer, length);
}

// 3 bits of MIC-E message. Bit A is the most significant bit.
// If custom is set then we use the custom message bits when encoding
//         Standard messages     Custom Messages
//   0x07: M0: Off Duty          C0: Custom-0
//   0x06: M1: En route          C1: Custom-1
//   0x05: M2: In service        C2: Custom-2
//   0x04: M3: Returning         C3: Custom-3
//   0x03: M4: Committed         C4: Custom-4
//   0x02: M5: Special           C5: Custom-5
//   0x01: M6: Priority          C6: Custom-6
//   0x00: Emergency             Emergency
void APRS_set_mice_msg(uint8_t msg, bool custom) {
    MICE_MSG = msg & 0x07;
    // If custom message bits, store the custom flag in bit 7 of the MICE_MSG
    if (custom) {
        MICE_MSG |= 0x80;
    }
}

void APRS_set_mice_ssid(uint8_t ssid) {
    MICE_SSID = ssid & 0x0F;
}

uint8_t APRS_sendLoc_mice(void *_buffer, size_t length) {
    uint8_t path_len;
    uint8_t payloadLength = 9 + length;
    uint8_t *packet = (uint8_t*)malloc(payloadLength);
    // Sanity check the latitude and longtitude
    if (latitude[7] != 'N' && latitude[7] != 'S') {
      return 1;
    }
    if (longtitude[8] != 'E' && longtitude[8] != 'W') {
      return 1;
    }
    // Build the Destination callsign with the latitude information
    DST[0] = (latitude[0] & 0x0F) | 0x30;
    if (MICE_MSG & 0x04) {
        if (MICE_MSG & 0x80) {
            DST[0] += 0x17; // Custom message bit
        } else {
            DST[0] += 0x20; // Standard message bit
        }
    }
    DST[1] = (latitude[1] & 0x0F) | 0x30;
    if (MICE_MSG & 0x02) {
        if (MICE_MSG & 0x80) {
            DST[1] += 0x17; // Custom message bit
        } else {
            DST[1] += 0x20; // Standard message bit
        }
    }
    DST[2] = (latitude[2] & 0x0F) | 0x30;
    if (MICE_MSG & 0x01) {
        if (MICE_MSG & 0x80) {
            DST[2] += 0x17; // Custom message bit
        } else {
            DST[2] += 0x20; // Standard message bit
        }
    }
    DST[3] = (latitude[3] & 0x0F) | 0x30;
    if (latitude[7] == 'N') { // North/South Latitude Indicator
        DST[3] += 0x20;
    }
    DST[4] = (latitude[5] & 0x0F) | 0x30; // Use latitude[5] becuase latitude[4] is a .
    // if (longtitude[0] == '1') { // If the longtitude is > 100, set this bit
    //     DST[4] += 0x20;
    // }
    DST[5] = (latitude[6] & 0x0F) | 0x30;
    if (longtitude[8] == 'W') { // If the longtitude is > 100, set this bit
        DST[4] += 0x20;
    }

    packet[0] = 0x60; // The ` character indicating valid GPS data
    // Degrees. If longtitude > 100 the +100 longtitude bit is set in the Destination field, but only in
    // certain circumstances, see http://www.aprs.org/doc/APRS101.PDF page 47
    uint8_t lon_deg = ((longtitude[0] & 0x0F) * 100 + (longtitude[1] & 0x0F) * 10 + (longtitude[2] & 0x0F));
    if (lon_deg < 10) {
        packet[1] = 118 + lon_deg;
        DST[4] += 0x20;
    } else if (lon_deg < 100) {
        packet[1] = 38 + lon_deg - 10;
    } else if (lon_deg < 110) {
        packet[1] = 108 + (lon_deg - 100);
        DST[4] += 0x20;
    } else {
        packet[1] = 38 + (lon_deg - 110);
        DST[4] += 0x20;
    }

    uint8_t lon_min = ((longtitude[3] & 0x0F) * 10) + (longtitude[4] & 0x0F);
    if (lon_min < 10) {
        packet[2] = 88 + lon_min;
    } else {
        packet[2] = 38 + lon_min - 10;
    }
    packet[3] = ((longtitude[6] & 0x0F) * 10) + (longtitude[7] & 0x0F) + 28;

    // bytes 4, 5 and 6 encode the speed and course
    // Page 50 of the APRS spec
    packet[4] = (speed / 10) + 28; // 100's an 10's of knots
    packet[5] = (speed % 10) * 10 + 32; // 1's of knots
    if (course > 299) {
        packet[5] += 3;
    } else if (course > 199) {
        packet[5] += 2;
    } else if (course > 99) {
        packet[5] += 1;
    }
    packet[6] = (course % 100) + 28;
    packet[7] = (uint8_t)symbol;
    packet[8] = (uint8_t)symbolTable;

    if (MICE_SSID != 0) {
      path_len = 0;
    } else {
      path_len = 4;
    }

    memcpy(dst.call, DST, 6);
    dst.ssid = DST_SSID;

    memcpy(src.call, CALL, 6);
    src.ssid = CALL_SSID;

    memcpy(path1.call, PATH1, 6);
    path1.ssid = PATH1_SSID;

    memcpy(path2.call, PATH2, 6);
    path2.ssid = PATH2_SSID;

    path[0] = dst;
    path[1] = src;
    path[2] = path1;
    path[3] = path2;
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(&packet[9], buffer, length);
    }
    ax25_sendVia(&AX25, path, path_len, packet, payloadLength);
    free(packet);
    return 0;
}

// Dynamic RAM usage of this function is 30 bytes
void APRS_sendLoc(void *_buffer, size_t length) {
    size_t payloadLength = 20+length;
    bool usePHG = false;
    if (power < 10 && height < 10 && gain < 10 && directivity < 9) {
        usePHG = true;
        payloadLength += 7;
    }
    uint8_t *packet = (uint8_t*)malloc(payloadLength);
    uint8_t *ptr = packet;
    packet[0] = '=';
    packet[9] = symbolTable;
    packet[19] = symbol;
    ptr++;
    memcpy(ptr, latitude, 8);
    ptr += 9;
    memcpy(ptr, longtitude, 9);
    ptr += 10;
    if (usePHG) {
        packet[20] = 'P';
        packet[21] = 'H';
        packet[22] = 'G';
        packet[23] = power+48;
        packet[24] = height+48;
        packet[25] = gain+48;
        packet[26] = directivity+48;
        ptr+=7;
    }
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(ptr, buffer, length);
    }

    APRS_sendPkt(packet, payloadLength);
    free(packet);
}

// Dynamic RAM usage of this function is 18 bytes
void APRS_sendMsg(void *_buffer, size_t length) {
    if (length > 67) length = 67;
    size_t payloadLength = 11+length+4;

    uint8_t *packet = (uint8_t*)malloc(payloadLength);
    uint8_t *ptr = packet;
    packet[0] = ':';
    int callSize = 6;
    int count = 0;
    while (callSize--) {
        if (message_recip[count] != 0) {
            packet[1+count] = message_recip[count];
            count++;
        }
    }
    if (message_recip_ssid != -1) {
        packet[1+count] = '-'; count++;
        if (message_recip_ssid < 10) {
            packet[1+count] = message_recip_ssid+48; count++;
        } else {
            packet[1+count] = 49; count++;
            packet[1+count] = message_recip_ssid-10+48; count++;
        }
    }
    while (count < 9) {
        packet[1+count] = ' '; count++;
    }
    packet[1+count] = ':';
    ptr += 11;
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(ptr, buffer, length);
        memcpy(lastMessage, buffer, length);
        lastMessageLen = length;
    }

    message_seq++;
    if (message_seq > 999) message_seq = 0;

    packet[11+length] = '{';
    int n = message_seq % 10;
    int d = ((message_seq % 100) - n)/10;
    int h = (message_seq - d - n) / 100;

    packet[12+length] = h+48;
    packet[13+length] = d+48;
    packet[14+length] = n+48;

    APRS_sendPkt(packet, payloadLength);
    free(packet);
}

void APRS_msgRetry() {
    message_seq--;
    APRS_sendMsg(lastMessage, lastMessageLen);
}

// For getting free memory, from:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15

extern unsigned int __heap_start;
extern void *__brkval;

struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

extern struct __freelist *__flp;

int freeListSize() {
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  return total;
}

int freeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}
