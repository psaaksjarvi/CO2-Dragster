#include <SPI.h>
#include <LoRa.h>

#define LIGHT_THRESHOLD_CAR1 950
#define LIGHT_THRESHOLD_CAR2 1000
#define MAX_RACE_TIME 10000

int car1_ldr1 = 0;
int car1_ldr2 = 0;
int car1_ldr3 = 0;

int car2_ldr1 = 0;
int car2_ldr2 = 0;
int car2_ldr3 = 0;

float car1_time = 0.0;
float car2_time = 0.0;

unsigned long startTime = 0;
unsigned long raceTime = 0;

bool racing = false;
bool car1_racing = false;
bool car2_racing = false;

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Waiting for LoRa and Race");
  
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Started OK\nFinish Line waiting for Race");
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(8);
}

void resetRace()
{
  startTime = 0;
  raceTime = 0;
  racing = false;
  car1_racing = false;
  car2_racing = false;
  car1_time = 0.0;
  car2_time = 0.0;
  Serial.println("Waiting for Race");
}

void loop()
{
  // Wait for race to start
  while (!racing)
  {
    // Try to parse LoRa
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      String msg = "";
      while (LoRa.available())
      {
        msg += (char)LoRa.read();
      }
      //Serial.println(sizeof(msg));

      if (msg == "GO1")
      {
        racing = true;
        car1_racing = true;
        car2_racing = true;
        startTime = millis();
        Serial.println("Race Started!");
      }
      if(msg.indexOf('r') != -1)
      {
        Serial.print(msg == 'r');
        resetRace();
      }
    }

    // Check for a keyboard stroke to reset the race
    if (Serial.available())
    {
      char input = Serial.read();
      if (input == 'r' || input == 'R') // Press 'r' or 'R' to reset the race
      {
        Serial.println("Race Reset");
        resetRace();
      }
    }
  }

  while (racing)
  {
    car1_ldr1 = analogRead(A0);
    car1_ldr2 = analogRead(A1);
    car1_ldr3 = analogRead(A2);
    car2_ldr1 = analogRead(A3);
    car2_ldr2 = analogRead(A4);
    car2_ldr3 = analogRead(A5);

    // Check car 1 sensors
    if ((car1_ldr1 < LIGHT_THRESHOLD_CAR1 || car1_ldr2 < LIGHT_THRESHOLD_CAR1 || car1_ldr3 < LIGHT_THRESHOLD_CAR1) && car1_racing)
    {
      // Race has finished for car1, send message to master
      car1_racing = false;
      // Calculate and send race time for car1
      raceTime = millis() - startTime;
      Serial.print("car1 ");
      Serial.println(raceTime);
      car1_time = raceTime;
      //Serial.println("car1 finished");
      // // Check if max time has expired... i.e., car 2 didn't finish
      // if (!car1_racing && (raceTime >= MAX_RACE_TIME))
      // {
      //   car2_racing = false;
      //   LoRa.beginPacket();
      //   LoRa.println("car2 failed to finish!");
      //   LoRa.endPacket();
      //   Serial.println("car2 didnt finish");
      //   resetRace();
      // }
    }

    // Check car 2 sensors
    if ((car2_ldr1 < LIGHT_THRESHOLD_CAR2 || car2_ldr2 < LIGHT_THRESHOLD_CAR2 || car2_ldr3 < LIGHT_THRESHOLD_CAR2) && car2_racing)
    {
      // Race has finished for car2, send message to master
      car2_racing = false;
            // Calculate and send race time for car2
      raceTime = millis() - startTime;
      Serial.print("car2 ");
      Serial.println(raceTime);
      car2_time = raceTime;
      // LoRa.beginPacket();
      // LoRa.print("car2 ");
      // LoRa.println(raceTime);
      // LoRa.endPacket();
      //Serial.println("car2 finished");

      // Check if max time has expired... i.e., car 1 didn't finish
      // if (!car2_racing && (raceTime >= MAX_RACE_TIME))
      // {
      //   car1_racing = false;
      //   LoRa.beginPacket();
      //   LoRa.println("car1 failed to finish!");
      //   LoRa.endPacket();
      //   Serial.println("car 1 didnt finish");
      //   resetRace();
      // }
    }
    if(car1_racing == false && car2_racing == false)
    {
      racing = false;
    }
    // Check for a keyboard stroke to reset the race
    // if (Serial.available())
    // {
    //   char input = Serial.read();
    //   if (input == 'r' || input == 'R') // Press 'r' or 'R' to reset the race
    //   {
    //     Serial.println("Race Reset");
    //     resetRace();
    //   }
    // }
  }
}
