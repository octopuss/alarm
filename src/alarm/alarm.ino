#include <LowPower.h>

#include <SPI.h>
#include <ADXL362.h>

ADXL362 xl;
int16_t interruptPin = 2;          //Setup ADXL362 interrupt output to Interrupt 0 (digital pin 2)
int16_t interruptStatus = 0;
int16_t XValue, YValue, ZValue, Temperature;
const int buzzer = A5;
int started = 0;


void setup(){
     pinMode(buzzer, OUTPUT);
    // Startup, soft reset
    Serial.begin(9600);
    xl.begin();                //soft reset
    delay(1000);    
    xl.setupDCActivityInterrupt(300, 10);   // 300 code activity threshold.  With default ODR = 100Hz, time threshold of 10 results in 0.1 second time threshold
    xl.setupDCInactivityInterrupt(80, 200);   // 80 code inactivity threshold.  With default ODR = 100Hz, time threshold of 30 results in 2 second time threshold
    Serial.println(); 
    // Map Awake status to Interrupt 1
    // *** create a function to map interrupts... coming soon
    xl.SPIwriteOneRegister(0x2A, 0x40);    
    // Setup Activity/Inactivity register
    xl.SPIwriteOneRegister(0x27, 0x3F); // Referenced Activity, Referenced Inactivity, Loop Mode  
    // turn on Autosleep bit
    byte POWER_CTL_reg = xl.SPIreadOneRegister(0x2D);
    POWER_CTL_reg = POWER_CTL_reg | (0x04);       // turn on POWER_CTL[2] - Autosleep bit
    xl.SPIwriteOneRegister(0x2D, POWER_CTL_reg);
    xl.beginMeasure();                      // DO LAST! enable measurement mode   
    xl.checkAllControlRegs();               // check some setup conditions    
    delay(100);
    pinMode(2, INPUT); 
    delay(10000);
    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(buzzer);
    attachInterrupt(0, interruptFunction, RISING);  // A high on output of ADXL interrupt means ADXL is awake, and wake up Arduino 
}

void loop(){
  interruptStatus = digitalRead(interruptPin);

// if ADXL362 is asleep, call LowPower.powerdown  
  if(interruptStatus == 0) { 
    //Serial.println("AT$P=1");
    delay(100);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);     
  }
  // give circuit time to settle after wakeup
  delay(20);
}

void interruptFunction(){
  if(started == 0) {
    started = 1;
    return;
  }
  Serial.print("\n");
  delay(5000);
  Serial.println("AT$SF=BABADEDA");
}

