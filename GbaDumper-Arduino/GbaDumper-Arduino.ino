/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */


 /*
ROM       Arduino   Atmega328P
Pin-Name  Pin-Name  Port-Name
**********************************
VCC       3.3V                    
CLK (unused)                      
WR        GND                     

RD        12        PB4 (PORT-B 4)
CS        13        PB5 (PORT-B 5)

0         2         PD2 (PORT-D 2)
1         3         PD3 (PORT-D 3)
2         4         PD4 (PORT-D 4)
3         5         PD5 (PORT-D 5)

4         6         PD6 (PORT-D 6)
5         7         PD7 (PORT-D 7)
6         A0        PC0 (PORT-C 0)
7         A1        PC1 (PORT-C 1)

8         A2        PC2 (PORT-C 2)
9         A3        PC3 (PORT-C 3)
10        A4        PC4 (PORT-C 4)
11        A5        PC5 (PORT-C 5)

12        8         PB0 (PORT-B 0)
13        9         PB1 (PORT-B 1)
14        10        PB2 (PORT-B 2)
15        11        PB3 (PORT-B 3)

CS2       3.3V
IRQ       GND
GND       GND
*/

// data is valid on the rising edge of /RD
int rd = 12;

// put low to latch address
int cs = 13;

// propagation delay time
int globalWait = 5;

void setup()
{
  pinMode(cs, OUTPUT);
  pinMode(rd, OUTPUT);
  Serial.begin(230400);
  delay(10);
  
  Serial.println("");
  Serial.print("<CMD:START_DUMP>");
  delay(10);
}

void loop()
{
  dumpNonSequential(0x0000, 0xFFFE - 2);
  dumpSequential(0xFFFE, 0xFFFFFF);
  //dumpSequential(0x0000, 0xFFFFFF);
  halt();
}

// we use long data type becouse Arduino Uno int is only 16 bit and we need
// long 32 bit
void dumpNonSequential(unsigned long startAddress, unsigned long endAddress)
{
  unsigned int value;
   
  for (unsigned int address = startAddress; address <= endAddress; address += 2) {
    latchAddress(address);
    unsigned int value = readBusValue();
    printBinary(value);
  }
}

void dumpSequential(unsigned long startAddress, unsigned long endAddress)
{
  unsigned int value;
  unsigned long address = startAddress;
  
  latchAddress(address);
  while(address <= endAddress) {
    value = readBusValue();
    printBinary(value);
    toggleHighLow(rd);
    address += 2;
  }
}

void latchAddress(unsigned int address)
{
  digitalWrite(rd, HIGH);
  wait();
  
  digitalWrite(cs, HIGH);
  wait();

  setBusAsOutput();
  wait();
  
  setValueOnBus(address / 2);
  wait();
  
  digitalWrite(cs, LOW);
  wait();
  
  setBusAsInput();
  
  digitalWrite(rd, LOW);
  wait();
}

void toggleHighLow(int port)
{
  digitalWrite(port, HIGH);
  wait();
  
  digitalWrite(port, LOW);
  wait();
}

void halt()
{
  while(true);
}

void wait()
{
  delayMicroseconds(globalWait);
}

void setValueOnBus(unsigned int value)
{
  char pinD = value << 02 & 0b11111100;
  char pinC = value >> 06 & 0b00111111;
  char pinB = value >> 12 & 0b00001111;

  PORTD = (PORTD & 0b00000011) | pinD;
  PORTC = (PORTC & 0b11000000) | pinC;
  PORTB = (PORTB & 0b11110000) | pinB;
}

unsigned int readBusValue()
{
  unsigned int valueD = (PIND >> 02) & 0b0000000000111111;
  unsigned int valueC = (PINC << 06) & 0b0000111111000000;
  unsigned int valueB = (PINB << 12) & 0b1111000000000000;

  unsigned int value = valueD | valueC | valueB;
  return value;
}

void setBusAsOutput()
{
    //       Pin Nr 76543210
    DDRD = DDRD | 0b11111100;
    DDRB = DDRB | 0b00001111;
    DDRC = DDRC | 0b00111111;
}

void setBusAsInput()
{
    DDRD = DDRD & 0b00000011;        
    DDRB = DDRB & 0b11110000;
    DDRC = DDRC & 0b11000000;

    // set pullup resistors
    setValueOnBus(0xffff);
}

// Step 2
void printBinary(unsigned int value)
{
    Serial.write(value & 0xFF);
    Serial.write((value >> 8) & 0xFF);
    Serial.flush();
}
