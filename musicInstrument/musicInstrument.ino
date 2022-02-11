// Audio
#include <Audio.h>
#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>

#include <synth_simple_drum.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum2;          //xy=399,244
AudioSynthNoiseWhite     drum3;          //xy=424,310
AudioSynthSimpleDrum     drum1;          //xy=431,197
AudioSynthSimpleDrum     drum4;          //xy=464,374
AudioEffectFade          fade1;
AudioMixer4              mixer1;         //xy=737,265
AudioOutputI2S           i2s1;           //xy=979,214
AudioConnection          patchCord1(drum1, 0, mixer1, 0);
AudioConnection          patchCord2(drum2, 0, mixer1, 1);
AudioConnection          patchCord3(drum4, 0, mixer1, 2);
AudioConnection          patchCord4(drum3, fade1);
AudioConnection          patfchDord5(fade1,0, mixer1, 3);
AudioConnection          patchCord7(mixer1, 0, i2s1, 0);
AudioConnection          patchCord8(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=930,518
// GUItool: end automatically generated code
//---------------------
// ZX Sensors
#include <ZX_Sensor.h>

// Constants
const int ZX_ADDR = 0x10;  // ZX Sensor I2C address

// Global Variables
ZX_Sensor zx_sensor = ZX_Sensor(ZX_ADDR);
float x_pos;
float z_pos;

unsigned long myTime;

//int zxThreshold; //Maybe?
//---------------------
// Piezo Sensors
int piezoSensor1 = A0;
int piezoSensor2 = A1;
int piezoSensor3 = A2;

int piezoThreshold = 20;

int sensorReading1 = 0;
int sensorReading2 = 0;
int sensorReading3 = 0;

void setup() {
  Serial.begin(115200);

  myTime = millis();

  // audio library init
  AudioMemory(15);

  AudioNoInterrupts();

  drum1.frequency(550);
  drum1.length(400);
  drum1.secondMix(1.0);
  drum1.pitchMod(0.5);
  mixer1.gain(0, 10);
  
  drum2.frequency(60);
  drum2.length(300);
  drum2.secondMix(0.0);
  drum2.pitchMod(1.0);
  mixer1.gain(1, 10);
  
  //drum3.frequency(550);
  //drum3.length(400);
  //drum3.secondMix(1.0);
  //drum3.pitchMod(0.5);
  drum3.amplitude(0.0);
  mixer1.gain(3,0.8);

  drum4.frequency(1200);
  drum4.length(150);
  drum4.secondMix(0.0);
  drum4.pitchMod(0.0);
  mixer1.gain(2, 10);
  //reverb1.reverbTime(1);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  AudioInterrupts();
}

void loop() {
  runPiezoSensor();

  runZXSensor();
  
  delay(10);
  
  AudioProcessorUsageMaxReset();
}

void runPiezoSensor(){
  sensorReading1 = analogRead(piezoSensor1);
  sensorReading2 = analogRead(piezoSensor2);
  //sensorReading3 = analogRead(piezoSensor3);
  
  if(sensorReading1 >= piezoThreshold){  
    Serial.print("Sensor 1: ");
    Serial.println(sensorReading1);
    int drumFrequency = map(sensorReading1, piezoThreshold, 300, 500, 600);
    drum4.frequency(drumFrequency);
    drum4.noteOn();
  }

  if(sensorReading2 >= piezoThreshold){
    Serial.print("Sensor 2: ");
    Serial.println(sensorReading2);
    int drumFrequency = map(sensorReading2, piezoThreshold, 300, 40, 80);
    drum2.frequency(drumFrequency);
    drum2.noteOn();
  }

  if(sensorReading3 >= piezoThreshold){
    Serial.print("Sensor 3: ");
    Serial.println(sensorReading3);
    int drumFrequency = map(sensorReading3, piezoThreshold, 300, 400, 700);
    drum4.frequency(drumFrequency);
    drum4.noteOn();
  }
}

void runZXSensor(){
  // If there is position data available, read and print it
  if ( zx_sensor.positionAvailable()) {
    boolean checkX = false, checkY = false;
    x_pos = zx_sensor.readX();
    if ( x_pos != ZX_ERROR ) {
      //Serial.print("X: ");
      //Serial.print(x_pos);
      checkX = true;
    }
    z_pos = zx_sensor.readZ();
    if ( z_pos != ZX_ERROR ) {
      //Serial.print(" Z: ");
      //Serial.println(z_pos);
      checkY = true;
    }
    
    float mappedPitch = map(x_pos, 0.0, 240.0, 200, 1000);
    
    if(checkX && checkY && millis() >= myTime+mappedPitch){ // myTime + the delay in milliseconds
      //
      float mappedFrequency = map(z_pos, 0.0, 240.0, 0.1, 0.9);
      drum3.amplitude(mappedFrequency);
      //mixer1.gain(3, mappedPitch);

      Serial.print("ZX Freq: ");
      Serial.print(mappedFrequency);
      Serial.print(" ZX Pitch:");
      Serial.println(mappedPitch);
      
      myTime = millis();
    } else {
      drum3.amplitude(0.0);
      //fade1.fadeIn(1);
    }
  } else {
    drum3.amplitude(0.0);
  }
}
