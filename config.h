/******************************************
 * 
 * Configuration of WortUhr TomSoft
 * 
 ******************************************/

//*****************************************
// Neopixel einrichten
#define NUM_PIXELS   114
#define NUM_SMATRIX  114
#define ROWS_MATRIX   10

#define LEDPIN         2  // GPIO2 = D4
#define LDRPIN        A0  // analogue input for LDR

#define LDR_HYSTERESE   50
#define LDR_MIN         20
#define LDR_MAX         255
#define LDR_CHECK_RATE  100


#define ESIST 0
#define VOR   1
#define NACH  2
#define UHR   3

#define FUENF       4
#define ZEHN        5
#define VIERTEL     6
#define DREIVIERTEL 7
#define ZWANZIG     8
#define HALB        9
#define EINS       10

#define H_EIN    11
#define H_ZWEI   12
#define H_DREI   13
#define H_VIER   14
#define H_FUENF  15
#define H_SECHS  16
#define H_SIEBEN 17
#define H_ACHT   18
#define H_NEUN   19
#define H_ZEHN   20
#define H_ELF    21
#define H_ZWOELF 22
