#include <SPI.h>
#include <LoRa.h>

#define START_THRESHOLD 750 
#define MAX_RACE_TIME 10000

unsigned long raceStartTime = 0;
unsigned long finishTime1 = 0;
unsigned long finishTime2 = 0;
bool raceInProgress = false;
bool raceIsReady = false;
String msg = "";
int ldr_1 = 0;
int ldr_2 = 0;

void setup()
{
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  Serial.begin(115200);
  while (!Serial);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("setup complete");
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(8);
}

void resetRace()
{
  raceStartTime = 0;
  finishTime1 = 0;
  finishTime2 = 0;
  raceInProgress = false;
  raceIsReady = false;
  msg = "";
  Serial.println("Race has been reset");
}

void loop()
{
  // Wait for the 'r' or 'R' key press to indicate race readiness
  while (!raceIsReady)
  {
    if (Serial.available())
    {
      char input = Serial.read();
      if (input == 'r' || input == 'R')
      {
        Serial.println("Race is ready!");
        raceIsReady = true;
      }
    }
  }

  // Race is ready, now wait for the start line to trigger the race
  while (!raceInProgress)
  {
    ldr_1 = analogRead(A3);
    ldr_2 = analogRead(A4);
  
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
  
    if (ldr_1 > START_THRESHOLD || ldr_2 > START_THRESHOLD)
    {
      raceStartTime = millis();
      raceInProgress = true;
      raceIsReady = false;
      // Race has started, send LoRa Signal or start timer
      Serial.println("go1");
      LoRa.beginPacket();
      LoRa.print("GO1");
      LoRa.endPacket();
      delay(10); // Small delay to prevent multiple triggers
    }
  }

  // Wait to receive finish times and handle reset
  while (raceInProgress)
  {
    // Check LoRa for messages
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      // Received a packet
      msg = "";
      while (LoRa.available())
      {
        msg += (char)LoRa.read();
      }
      // Serial.print("Got this...");
      // Serial.println(msg);
      // Serial.println(msg.indexOf("car1"));
      // Serial.println(msg.indexOf("car2"));
      // Check if a car has finished
      if (msg.indexOf("car1") == 0)
      {
        finishTime1 = millis() - raceStartTime;
        Serial.print("car1: ");
        Serial.println(finishTime1);
      }
      if (msg.indexOf("car2") == 0)
      {
        finishTime2 = millis() - raceStartTime;
        Serial.print("car2: ");
        Serial.println(finishTime2);
      }

      // Check if both cars have finished
      if (finishTime1 > 0 && finishTime2 > 0)
      {
        Serial.println("Race has finished!");
        raceInProgress = false;
      }
      
      msg = ""; // Clear the message after processing
    }
    // Handle reset after the race finishes
    if (!raceInProgress)
    {
      while (raceIsReady)
      {
        if (Serial.available())
        {
          char input = Serial.read();
          if (input == 'r' || input == 'R')
          {
            Serial.println("Race Reset");
            resetRace();
          }
        }
      }
    }
  }
}
