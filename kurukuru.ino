#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <NeoPixelBrightnessBus.h> 
#include <VL53L0X.h>

#define PIXEL_PIN 8
#define PIXEL_COUNT 19

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(PIXEL_COUNT, PIXEL_PIN);

VL53L0X m_sensor;
int sound_mode, sound_mode_old=0;
int music_var=0;
uint32_t g_lastmode_ms = 0;

#define START_DIST 225
#define MODE1_DIST 180
#define MODE2_DIST 130
#define MODE3_DIST  80
#define MODE4_DIST  40

//Sound設定
DFRobotDFPlayerMini myDFPlayer;
#define SE_LC 1
#define SE_E 2
#define SE_G 3
#define SE_HC 4
#define SE_DESIGN_A1 5
#define SE_DESIGN_A2 6
#define SE_DESIGN_A3 7
#define SE_DESIGN_A4 8
#define SE_DESIGN_A5 9
#define SE_EF_1 10
#define SE_EF_2 11
#define SE_EF_3 12
#define SE_EF_4 13
#define SE_EF_5 14

void setup() {
  Serial.begin(115200);
  Serial.println("SetUp Start");
  Wire.begin();
  // センサー初期化設定
  m_sensor.init();
  delay(100);
  //m_sensor.setAddress(24);
  m_sensor.setTimeout(500);
  m_sensor.startContinuous();  

  // Speaker設定
  Serial1.begin(9600);
  Serial.println("start_speaker");
  myDFPlayer.begin(Serial1);
  myDFPlayer.volume(26);  //Set volume value. From 0 to 30
  //myDFPlayer.play(SE_C);

  //LED
  strip.Begin();
  strip.SetBrightness(80);
  strip.Show(); // Initialize all pixels to 'off'
  rainbowCycle(2, 0, PIXEL_COUNT-1);

  g_lastmode_ms = millis();  //リピートモード用の時間リセット
}


void loop() {
  unsigned int m_sensor_tmp = m_sensor.readRangeContinuousMillimeters();

  switch(sound_mode){
    case 0:
      if( m_sensor_tmp <= START_DIST) sound_mode++;
      //LED
      setAll(RgbColor(0,0,0));
      break;
    case 1:
      if( m_sensor_tmp <= MODE1_DIST) sound_mode++;
      //LED
      setAll(RgbColor(80,0,0));
      break;
    case 2:
      if( m_sensor_tmp <= MODE2_DIST) sound_mode++;
      //LED
      setAll(RgbColor(128,30,0));
      break;
    case 3:
      if( m_sensor_tmp <= MODE3_DIST) sound_mode++;
      //LED
      setAll(RgbColor(180,120,0));
      break;
    case 4:
      if( m_sensor_tmp <= MODE4_DIST) sound_mode++;
      //LED
      setAll(RgbColor(0,120,180));
      break;
    case 5:
      rainbowCycle(1, 0, PIXEL_COUNT-1);
      //delay(1500);
      sound_mode = 0;
      if(music_var == 2) music_var =0;
      else music_var++;
    default:
      break;
  }


  //SE設定
  if(sound_mode != sound_mode_old){
   switch(sound_mode){
      case 1:
        if(music_var == 0)  myDFPlayer.play(SE_LC);
        else if(music_var == 1) myDFPlayer.play(SE_DESIGN_A1);
        else  myDFPlayer.play(SE_EF_1);
        Serial.println("SOUND C");
        break;
      case 2:
        if(music_var == 0)  myDFPlayer.play(SE_E);
        else if(music_var == 1) myDFPlayer.play(SE_DESIGN_A2);
        else  myDFPlayer.play(SE_EF_2);
        Serial.println("SOUND E");
        break;
      case 3:
        if(music_var == 0)  myDFPlayer.play(SE_G);
        else if(music_var == 1) myDFPlayer.play(SE_DESIGN_A3);
        else  myDFPlayer.play(SE_EF_3);
        Serial.println("SOUND G");
        break;
      case 4:
        if(music_var == 0)  myDFPlayer.play(SE_HC);
        else if(music_var == 1) myDFPlayer.play(SE_DESIGN_A4);
        else  myDFPlayer.play(SE_EF_4);
        Serial.println("SOUND HC");
        break;
      case 5:
        if(music_var == 1)  myDFPlayer.play(SE_DESIGN_A5);
        else if(music_var == 2)  myDFPlayer.play(SE_EF_5);
        //myDFPlayer.play(SE_G);
        //Serial.println("SOUND F");
        break;
        }
        g_lastmode_ms = millis(); 
  }

  //LED追加エフェクト
  if(sound_mode >=1 && sound_mode <= 4){
    int led_white_num = Time_Measure(g_lastmode_ms)/50;
    strip.SetPixelColor(led_white_num-1, RgbColor(160,160,160));
    strip.SetPixelColor(led_white_num, RgbColor(255,255,255));
    strip.SetPixelColor(led_white_num+1, RgbColor(160,160,160));
    strip.Show();
  }

  
  Serial.print("Mode:");
  Serial.print(sound_mode);  
  Serial.print("   dist:");
  Serial.println(m_sensor_tmp);

  //状態更新
  sound_mode_old = sound_mode;
  delay(10);
}

void rainbowCycle(int SpeedDelay,  int start_led, int end_led) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for (int i=start_led; i<= end_led; i++) {
      strip.SetPixelColor(PIXEL_COUNT-1-i, Wheel(((i * 256 / (end_led-start_led)) + j) & 255));
    }
    strip.Show();
    delay(SpeedDelay);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
RgbColor Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return RgbColor(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return RgbColor(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return RgbColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void setAll(RgbColor color) {
  for(int i = 0; i <PIXEL_COUNT; i++ ) {
    strip.SetPixelColor(i, color); 
  }
  strip.Show();
}


uint32_t Time_Measure( uint32_t st_time ){
  return millis() - st_time;
}
