#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#include <PubSubClient.h>


#include <NTPClient.h>
#include <WiFiUdp.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or 1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
# define BUZZER 5
# define LED_1 15

# define PB_CANCEL 34

#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 13

#define NTP_SERVER     "pool.ntp.org"
//define UTC_OFFSET     0
#define UTC_OFFSET_DST 0
#define LDR2 A0
#define LDR1 A3
#define MOTOR 18

//declare objects
Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, & Wire, OLED_RESET);
DHTesp dhtSensor;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
char tempAr[10];
char lightAr[10];

WiFiUDP ntpUDP ;
NTPClient timeClient(ntpUDP);

Servo servo;


bool isScheduledON = false;
unsigned long scheduledOnTime;




//Global variables 
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0 ;
int UTC_OFFSET = 0; // Declare UTC_OFFSET as a global variable

float angle = 0;
float Angle_Offset = 30;
float intensity1=0;
float intensity2=0;
const float GAMMA = 0.7;
const float RL10 = 50;
float Controlling_Factor = 0.75;
double max_angle =180;
float max_angle1 =180;

// UTC Offset parameter in seconds
long utc_offset_sec = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

bool alarm_enabled =  true;
int n_alarms = 2;
int alarm_hours[] = {0,1};
int alarm_minutes[] = {1,10};
bool alarm_triggered[]= {false , false};



int n_notes = 8;     
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440 ; 
int B =  494 ;
int C_H = 523 ;
int notes[]= {C, D , E ,F , A , B, C_H};

int current_mode = 0;
int max_modes = 5;
String modes[]={ "1 - Set TimeZone","2- Set Alarm 1","3 - Set Alarm 2","4- Set Alarm 3","5 - Disable Alarms"};

 
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER ,OUTPUT );
  pinMode(LED_1 ,OUTPUT);
  pinMode( PB_CANCEL,  INPUT );
  pinMode( PB_OK,  INPUT );
  pinMode( PB_UP,  INPUT );
  pinMode( PB_DOWN,  INPUT );
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  pinMode(MOTOR, OUTPUT);

  setupWifi();
  setupMqtt();

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  
  timeClient.begin();
  timeClient.setTimeOffset(5.5 * 3600);

  digitalWrite(BUZZER, LOW);


  

  

// put your setup code here, to run once:
 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed")); 
    for(;;); // Don't proceed, loop forever
 }
// Show initial display buffer contents on the screen --
// the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WIFI" , 0,0,2);
    
  }

  display.clearDisplay();
  print_line("Connected to WIFI" , 0,0,2);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  servo.attach(MOTOR, 500, 2400);


// Clear the buffer
  display.clearDisplay();
  print_line("welcome to medibox!",10 , 20 ,2);
  delay(500);
    display.clearDisplay();
}
void loop(){
 update_time_with_check_alarm();

if(!mqttClient.connected()){
    connectToBroker();
  }
  mqttClient.loop();

  
  Serial.println(tempAr);
  

  updateLightIntensity_and_rotateMotor();
  Serial.println(lightAr);
  mqttClient.publish("ENTC-ADMIN-TEMP",tempAr);
  mqttClient.publish("ADMIN-LIGHT-INTENSITY",lightAr);
  

  checkSchedule();
  delay(1000);




 if (digitalRead(PB_OK)==LOW){
  delay(200);
  go_to_menu();
 }
 check_temp_and_humidity();


}



void setupWifi(){
  WiFi.begin("Wokwi-GUEST","");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");


  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
}




void buzzerOn(bool on){
  if(on){
    tone(BUZZER, 256);

  }else{
    noTone(BUZZER);
  }
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);

  
}

void receiveCallback(char* topic,byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");

  char payloadCharAr[length];
  for(int i = 0 ;i < length;i++){
    Serial.print((char)payload[i]);
    payloadCharAr[i] =(char)payload[i];

}
  Serial.println("");


  if (strcmp(topic, "ENTC-ADMIN-MIN-ANGLE") == 0){
    Angle_Offset = atoi(payloadCharAr);
    Serial.println("Min angle =" + String(Angle_Offset));
  }
  //receive control factor
  if (strcmp(topic, "ENTC-ADMIN-CONTROL-FACTOR") == 0) {
    Controlling_Factor = atof(payloadCharAr);
    Serial.println("controlling factor = "+ String(Controlling_Factor));
  }

  if (strcmp(topic, "ENTC-ADMIN-MAIN-ON-OFF")==0){
    buzzerOn(payloadCharAr[0]=='1');
     
    }else if(strcmp(topic, "ENTC-ADMIN-SCH-ON")==0){
      if(payloadCharAr[0]=='N'){
        isScheduledON = false;
      }else{
        isScheduledON = true;
        scheduledOnTime = atol(payloadCharAr);

      }
    }

  }


void connectToBroker(){
  while(!mqttClient.connected()){
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-122364556")){
      Serial.println("connected");
      mqttClient.subscribe("ENTC-ADMIN-MAIN-ON-OFF");
      mqttClient.subscribe("ENTC-ADMIN-SCH-ON");
      mqttClient.subscribe("ENTC-ADMIN-MIN-ANGLE");
      mqttClient.subscribe("ENTC-ADMIN-CONTROL-FACTOR");
    }else{
      Serial.print("failed");
      Serial.print(mqttClient.state());
      delay(5000);

    }



  }
  }



unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();


}
void checkSchedule(){
  if(isScheduledON){
    unsigned long currentTime = getTime();
    if (currentTime > scheduledOnTime){
      buzzerOn(true);
      isScheduledON = false;
      mqttClient.publish("ENTC-ADMIN-MAIN-ON-OFF-ESP", "1");
      mqttClient.publish("ENTC-ADMIN-SCH-ESP-ON", "0");

      Serial.println("Scheduled ON");


    }
  }

}




void updateLightIntensity_and_rotateMotor(){
  float ldr1 = analogRead(LDR1);
  float ldr2 = analogRead(LDR2);
    
  
  


  const float analogMinValue = 32.0;   // Minimum analogRead value
  const float analogMaxValue = 4063.0; // Maximum analogRead value
  
  const float intensityMax = 1.0;  // Maximum intensity value

  
  
  intensity1 = intensityMax+(analogMinValue-ldr1)/(analogMaxValue-analogMinValue);

  
   

    intensity2 = intensityMax+(analogMinValue-ldr2)/(analogMaxValue-analogMinValue);

    Serial.println("intensity1 = "+ String(intensity1));
    Serial.println("intensity2  =" +String( intensity2));

   if (intensity1 > intensity2){
    String(intensity1).toCharArray(lightAr,10);
    mqttClient.publish("MAX-LIGHT-INTENSITY-LDR","Right LDR");
    angle = min(Angle_Offset*0.5 + (180 - Angle_Offset) * intensity1 * Controlling_Factor, max_angle);
    
    servo.write(angle);

    Serial.println("ldr1 (angle):" + String(angle));

    delay(15);
   }
   else if( intensity1 == intensity2){
    String(intensity1,2).toCharArray(lightAr,10);
    mqttClient.publish("MAX-LIGHT-INTENSITY-LDR","Right LDR  = LEFT LDR");
    angle = min(Angle_Offset + (180 - Angle_Offset) * intensity2 * Controlling_Factor, max_angle1);
    servo.write(angle);
    
    Serial.println("ldr2 (angle)= ldr1 (angle) = " + String(angle));
    delay(15);
   }
   else if(intensity2 > intensity1){
     String(intensity2,2).toCharArray(lightAr,10);
     mqttClient.publish("MAX-LIGHT-INTENSITY-LDR","LEFT LDR");
     angle =min(Angle_Offset*1.5 + (180 - Angle_Offset) * intensity2 * Controlling_Factor, max_angle);
     servo.write(angle);
    
     Serial.println("ldr2 (angle):" +String(angle));
     delay(15);
   
   }



 



  //Serial.println(lux);
 

 
  // Serial.println( adcValue);
 
 
}

















void print_line(String text,int column, int row , int text_size){
  


  display.setTextSize(text_size); 
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(column , row); // Normal 1:1 pixel scale // Draw white text // Start at top-left corner display.println(F("Welcome to Medibox!"));
  display.println(text);
  display.display();
}

void print_time_now(void){
  display.clearDisplay();
  print_line(String(days),0,0,2);
  print_line(";",20,0,2);
  print_line(String(hours),30,0,2);
  print_line(";",20,0,2);
  print_line(String(minutes),60,0,2);
  print_line(";",20,0,2);
  print_line(String(seconds),90,0,2);
  

}

void update_time(){
    struct tm timeinfo;
    getLocalTime(&timeinfo);

    char timeHour[3];
    strftime(timeHour,3, "%H", &timeinfo);
    hours = atoi(timeHour);

    char timeMinute[3];
    strftime(timeMinute,3, "%M", &timeinfo);
    minutes = atoi(timeMinute);

    char timeSecond[3];
    strftime(timeSecond,3, "%S", &timeinfo);
    seconds = atoi(timeSecond);

    char timeDay[3];
    strftime(timeDay,3, "%d", &timeinfo);
    days = atoi(timeDay);
}


// Added a new function to set the time zone
void set_time_zone() {
  int current_offset_hours = utc_offset_sec / 3600;
  int current_offset_minutes = (utc_offset_sec % 3600) / 60;
   display.clearDisplay();

  int offset_hours = read_value("Enter offset hours: ", current_offset_hours, 14, -12);
   display.clearDisplay();
  int offset_minutes = read_value("Enter offset minutes: ", current_offset_minutes, 59, 0);

  utc_offset_sec = offset_hours * 3600 + offset_minutes * 60;
  configTime(utc_offset_sec, UTC_OFFSET_DST, NTP_SERVER);

   display.clearDisplay();
  print_line("Timezone set to " + String(offset_hours) + ":" + String(offset_minutes), 0, 0, 2);
  delay(1000);
}

void ring_alarm(){
  display.clearDisplay();
  print_line ("Medicine Time!" , 0 , 0 , 2);
  digitalWrite(LED_1 , HIGH );

  bool break_happened = false ;

  while (digitalRead(PB_CANCEL)==HIGH){
    for (int i =0 ; i< n_notes; i++){
      if (digitalRead(PB_CANCEL)== LOW){
        delay(200);

        break_happened = true ;

        break;
      }
    tone(BUZZER , notes[i]);
    delay(500);
    noTone(BUZZER);
    delay(2);

  }
  digitalWrite(LED_1 ,LOW);
  display.clearDisplay();
  
  // RING THE BUZZER
  
  }


}

  void update_time_with_check_alarm(void){
    update_time();
    print_time_now();

    if (alarm_enabled == true ){
      for (int i = 0 ; i < n_alarms ; i++){
        if (alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minutes[i] == minutes){
          ring_alarm();
          alarm_triggered[i] =  true;
        }
      }
    }
  }

  

int wait_for_button_press(){
  while(true) {
    if (digitalRead(PB_UP) == LOW){
      delay(200);
      return PB_UP;
    }

    else if (digitalRead(PB_DOWN) == LOW){
      delay(200);
      return PB_DOWN;
    }

    else if (digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }

    else if (digitalRead(PB_CANCEL) == LOW){
      delay(200);
      return PB_CANCEL;
    }
    update_time();
  }
}  

void go_to_menu(){
  while (digitalRead(PB_CANCEL) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode], 0 ,0 , 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP){
      delay(200);
      current_mode += 1;
      current_mode = current_mode % max_modes;

    }

    else if (pressed == PB_DOWN){
      delay(200);
      current_mode -= 1;
      if (current_mode < 0){
        current_mode = max_modes-1;


      }
    }

    else if (pressed == PB_OK){
      delay(200);
      
      run_mode(current_mode);

    }
    
    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }

}


void set_alarm(int alarm){

  
  int temp_hour = alarm_hours[alarm];

  while(true){

    display.clearDisplay();
    print_line("Enter hour:" + String(temp_hour), 0,0,2);

    int pressed = wait_for_button_press();
      if (pressed == PB_UP){
        delay(200);
        temp_hour += 1;
        temp_hour = temp_hour % 24;


    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0){
        temp_hour = 23;


      }
    }

    else if (pressed == PB_OK){
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
      

    }
    
    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }


  }
 

 int temp_minute = alarm_minutes[alarm];

  while(true){

    display.clearDisplay();
    print_line("Enter minute:" + String(temp_minute), 0,0,2);

    int pressed = wait_for_button_press();
      if (pressed == PB_UP){
        delay(200);
        temp_minute += 1;
        temp_minute = temp_minute % 60;


    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0){
        temp_minute = 59;


      }
    }

    else if (pressed == PB_OK){
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
      

    }
    
    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }


  }

  display.clearDisplay();
  print_line("Alarm is set", 0,0,2);
  delay(1000);




}

void run_mode(int mode){
  if (mode == 0 ){
    set_time_zone();
  }

  else if (mode == 1 || mode == 2|| mode == 3){
    set_alarm(mode -1 );

  }
  else if (mode == 4 ){
    alarm_enabled = false;
  }

  

}




void check_temp_and_humidity(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String(data.temperature,2).toCharArray(tempAr, 10);

  if (data.temperature > 32){
    display.clearDisplay();
    print_line("TEMP HIGH",0,40,1);
    delay(200);
  }
  else if (data.temperature < 26){
    display.clearDisplay();
    print_line("TEMP LOW",0,40,1);
    delay(200);
  }
  if (data.humidity > 80){
    display.clearDisplay();
    print_line("HUMIDITY HIGH",0,50,1);
    delay(200);
  }
  else if (data.humidity < 60){
    display.clearDisplay();
    print_line("HUMIDITY LOW",0,50,1);
    delay(200);
  }


}


/**
   Function to read user input value using push buttons.
   return int value based on user input
   text - Text message for user
   init_value - Initial Value
   max_value - Maximum allowed value
   min_value - Minimum allowed value
*/
int read_value(String text, int init_value, int max_value, int min_value) {
  int temp_value = init_value;
  while (true) {
    display.clearDisplay();
    print_line(text + String(temp_value), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_value += 1;
      if (temp_value > max_value) {
        temp_value = min_value;
      }
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      temp_value -= 1;
      if (temp_value < min_value) {
        temp_value = max_value;
      }
    }

    else if (pressed == PB_OK) {
      delay(200);
      return temp_value;
    }

    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }

  return init_value;
}






