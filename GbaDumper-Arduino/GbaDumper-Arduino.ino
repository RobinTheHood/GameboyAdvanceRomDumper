/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */
 
/*
Arduino   ROM       Atmega328P
Pin-Name  Pin-Name  Port-Name
**********************************
3.3V      VCC
GND       GND

GND       WR
3.3V      CS2
GND       IRQ
          CLK (unused)
 
A0        CS        PC0 (PORT-C 0)
A1        RD        PC1 (PORT-C 1)

2         0         PD2 (PORT-D 2) 
3         1         PD3 (PORT-D 3)
4         2         PD4 (PORT-D 4)
5         3         PD5 (PORT-D 5)
6         4         PD6 (PORT-D 6)
7         5         PD7 (PORT-D 7)
8         6         PB0 (PORT-B 0)
9         7         PB1 (PORT-B 1)

A2        8         PC2 (PORT-C 2)
A3        9         PC3 (PORT-C 3)
A4        10        PC4 (PORT-C 4)
A5        11        PC5 (PORT-C 5)

10        12        PB2 (PORT-B 2)
11        13        PB3 (PORT-B 3)
12        14        PB4 (PORT-B 4)
13        15        PB5 (PORT-B 5)
*/

// Put low to latch address
int cs = A1;

// Data is valid on the rising edge of /RD
int rd = A0;

// propagation delay time
int globalWait = 20;

void setup()
{
  pinMode(cs, OUTPUT);
  pinMode(rd, OUTPUT);
  
  Serial.begin(230400);
  delay(10);
  
  Serial.println("");
  Serial.print("***START DUMP***");
  delay(10);
}

void loop()
{
  dump(false);
}

void dump(boolean debug)
{
  dumpAddress(0x000, 0xFFFE - 2, debug);
  dumpSequentail(0xFFFE, 0xFFFFFFFF, debug);
  halt();
}

void dumpAddress(unsigned long startAddress, unsigned long endAddress, boolean debug)
{
  unsigned int value;
  unsigned int testValue;
  
  for (unsigned long address = startAddress; address <= endAddress; address += 2) {
    latchAddress(address);
    value = readBusValueNew();

    if (debug) {
      printAddressBlock(address, 16);
      printHexInv(value);
    } else {
      printBinary(value);
    }
  }
}

void dumpSequentail(unsigned long startAddress, unsigned long endAddress, boolean debug)
{
  unsigned int value;
  unsigned long address = startAddress;
  
  latchAddress(address);
  while(address <= endAddress) {
    value = readBusValueNew();
    
    if (debug) {
      printAddressBlock(address, 16);
      printHexInv(value);
    } else {
      printBinary(value);
    }
    
    toggleHighLow(rd);
    address += 2;
  }
}

unsigned int getSequentail(unsigned long address)
{
  latchAddress(address - 2);
  toggleHighLow(rd);
  return readBusValueNew();
}

void toggleHighLow(unsigned int port)
{
  digitalWrite(port, HIGH);
  delayMicroseconds(globalWait);
  
  digitalWrite(port, LOW);
  delayMicroseconds(globalWait);
}

void latchAddress(unsigned int address)
{
  digitalWrite(cs, HIGH);
  delayMicroseconds(globalWait);
  
  digitalWrite(rd, HIGH);
  delayMicroseconds(globalWait);

  setBusAsOutput();
  delayMicroseconds(globalWait);
  
  setValueOnBus(address/2);
  delayMicroseconds(globalWait);

  digitalWrite(cs, LOW);
  delayMicroseconds(globalWait);

  setValueOnBus(0xFFFF);
  delayMicroseconds(globalWait);
  
  setBusAsInput();
  delayMicroseconds(globalWait);
  
  digitalWrite(rd, LOW);
  delayMicroseconds(globalWait);
}

void halt()
{
  while(true);
}

void haltAtAddress(unsigned long address, unsigned long value)
{
  if (address >= value) {
    halt();
  }
}

void setValueOnBus(unsigned int value)
{
  unsigned int pinD1 = value << 02 & 0x00FC; // 00 - 05 => 2 - 7
  unsigned int pinB1 = value >> 06 & 0x0003; // 06 - 07 => 0 - 1
  unsigned int pinC1 = value >> 06 & 0x003C; // 08 - 11 => 2 - 5
  unsigned int pinB2 = value >> 10 & 0x003C; // 12 - 15 => 2 - 5

  PORTD = (PIND & 0x0003) | pinD1;
  PORTB = (PINB & 0x00C0) | pinB1 | pinB2;
  PORTC = (PINC & 0x00C3) | pinC1;
}

unsigned int readBusValueNew()
{
  unsigned int valueD1 = PIND; // 2 - 7 => 00 - 05
  unsigned int valueB1 = PINB; // 0 - 1 => 06 - 07
  unsigned int valueC1 = PINC; // 2 - 5 => 08 - 11
  unsigned int valueB2 = PINB; // 2 - 5 => 12 - 15

  valueD1 = (valueD1 >> 02) & 0x003F;
  valueB1 = (valueB1 << 06) & 0x00C0;
  valueC1 = (valueC1 << 06) & 0x0F00;
  valueB2 = (valueB2 << 10) & 0xF000;

  unsigned int value = valueB2 | valueC1 | valueB1 | valueD1;
  return value;
}

void setBusAsOutput()
{              
    //      Pin Nr 76543210
    DDRD = DDRD | B11111100; // set, 1 = output
    DDRB = DDRB | B00111111; // set, 1 = output
    DDRC = DDRC | B00111100; // set, 1 = output
}

void setBusAsInput()
{
    //      Pin Nr 76543210
    DDRD = DDRD & B00000011; // delete, 0 = output           
    DDRB = DDRB & B11000000; // delete, 0 = output
    DDRC = DDRC & B11000011; // delete, 0 = output
}

/*
 ********************
 * output functions *
 ********************
 */
 
void printAddressBlock(unsigned long address, unsigned long byteBlock)
{
  if (address % byteBlock == 0) {
      Serial.println("");
      printAddress(address);
      Serial.print(": ");
  }
}

void printAddress(unsigned long value)
{
  unsigned int lo = value & 0x000FFFF;
  unsigned int hi = (value >> 16) & 0x000FFFF;
  printHex(hi);
  printHex(lo);
}

void printBinary(unsigned int value)
{
    Serial.write(value & 0xFF);
    Serial.write((value >> 8) & 0xFF);
}

void printHexInv(unsigned int value)
{
    unsigned int value1 = value << 8 & 0xFF00;
    unsigned int value2 = value >> 8 & 0x00FF;
    unsigned int valueInv = value1 | value2;
    printHex(valueInv);
}

void printHex(unsigned int value)
{
  String hex = String(value, HEX);
    if (hex.length() == 1) {
      hex = "000" + hex;
    } else if (hex.length() == 2) {
      hex = "00" + hex;
    } else if (hex.length() == 3) {
      hex = "0" + hex;
    }
    Serial.print(hex + " ");
}
