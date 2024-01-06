#define PIN_HDATA	36
#define PIN_SCL		13
#define PIN_SDA		12
#define PIN_VSYNC	25
#define PIN_HSYNC	14
#define PIN_PCLK	27
#define PIN_MCLK	26

#define DATA_WIDTH	5
#define SCCB_ID		0x42


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 




//#define GPIO_READ_0(PIN) !!(REG_READ(GPIO_IN_REG) & 1 << PIN) 
#define GPIO_READ(PIN) !!((*(volatile uint32_t *)(GPIO_IN_REG)) & (1 << PIN)) 
#define GPIO_READ_1(PIN) !!(REG_READ(GPIO_IN1_REG) & 1 << (PIN >> 5))