//MAC address B8:D6:1A:44:3B:4C
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>

#include <TFT_eSPI.h>
#include <SPI.h>

#define PI 3.14159265358979323846

TFT_eSPI tft = TFT_eSPI();

//---------------------------
//ESP-NOW STUFF
//---------------------------

typedef struct structMessage {
  int up;
  int down;
  int right;
  int left;
};
structMessage js;


void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&js, incomingData, sizeof(js));
}

//---------------------------

const unsigned long frameRateInterval = 8;
unsigned long previousFrameTime = 0;

//---------------------------

void setup() {
  Serial.begin(115200);

  //ESP STUFF
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;
  esp_now_register_recv_cb(OnDataRecv);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  int currentTime = millis();

  if(currentTime - previousFrameTime >= frameRateInterval){
    previousFrameTime = currentTime;
    AchtungDieKurve();
  }
}

//---------------------------
//Achtung Die Kurve!

typedef struct structKurve{
  float angle;

  float posX;
  float posY;

  int intPosX;
  int intPosY;

  int prevPosX;
  int prevPosY;

  float moveSpeed = 0.3;
  int turnSpeed = 3;
};

structKurve kurve1;

uint8_t matrix[128][160] = {0};

/*
void AchtungDieKurve(){
  kurve1.angle += js.left * kurve1.turnSpeed;
  kurve1.angle -= js.right * kurve1.turnSpeed;

  kurve1.posX += cos(kurve1.angle / 360 * PI);
  kurve1.posY -= sin(kurve1.angle / 360 * PI);

  tft.drawPixel(kurve1.posX, kurve1.posY, TFT_BLUE);

  if(kurve1.prevPosX != int(kurve1.posX) or kurve1.prevPosY != int(kurve1.posY)){
    matrix[int(kurve1.prevPosY)][int(kurve1.prevPosX)] = 1;
  }

  if(matrix[int(kurve1.posY)][int(kurve1.posX)] == 1){
    tft.drawPixel(kurve1.posX, kurve1.posY, TFT_RED);
  }

  //-------------------------------
  //CHECK FOR COLLISION
  //Check if going UP
  if(kurve1.prevPosY - 1 == int(kurve1.posY)){
    //CHeck if going RIGHT
    if(kurve1.prevPosX + 1 == int(kurve1.posX)){
      // Serial.println("^>");

      if(matrix[int(kurve1.posY) + 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) - 1] == 1){
        // Serial.println("COLLLSIIIISIISIIIONONONONOONNNN!!!!");
        tft.drawPixel(kurve1.posX, kurve1.posY, TFT_GREEN);
      }
    }
    else if(kurve1.prevPosX - 1 == int(kurve1.posX)){
      // Serial.println("<^");

      if(matrix[int(kurve1.posY) + 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) + 1] == 1){
        // Serial.println("COLLLSIIIISIISIIIONONONONOONNNN!!!!");
        tft.drawPixel(kurve1.posX, kurve1.posY, TFT_GREEN);
      }
    }
  }
  //Check if going DOWN
  else if (kurve1.prevPosY + 1 == int(kurve1.posY)){
    // Serial.println("DOWN");

    //CHeck if going RIGHT
    if(kurve1.prevPosX + 1 == int(kurve1.posX)){
      // Serial.println("RIGHT");

      if(matrix[int(kurve1.posY) - 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) - 1] == 1){
        // Serial.println("COLLLSIIIISIISIIIONONONONOONNNN!!!!");
        tft.drawPixel(kurve1.posX, kurve1.posY, TFT_GREEN);
      }
    }
    //Check if going LEFT
    else if(kurve1.prevPosX - 1 == int(kurve1.posX)){
      // Serial.println("LEFT");

      if(matrix[int(kurve1.posY) - 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) + 1] == 1){
        // Serial.println("COLLLSIIIISIISIIIONONONONOONNNN!!!!");
        tft.drawPixel(kurve1.posX, kurve1.posY, TFT_GREEN);
      }
    }
  }
  //-------------------------------

  // kurve1.prevPosX = int(kurve1.posX);
  // kurve1.prevPosY = int(kurve1.posY);
  
  // matrix1[kurve1.posY][kurve1.posX] = true;

  // kurve1.posX += js.right;
  // kurve1.posX -= js.left;
  // kurve1.posY -= js.up;
  // kurve1.posY += js.down;

  // tft.drawPixel(kurve1.posX, kurve1.posY, TFT_BLUE);

  // if(matrix1[kurve1.posY][kurve1.posX]){
  //   tft.drawPixel(kurve1.posX, kurve1.posY, TFT_RED);
  // }
}*/

void AchtungDieKurve(){
  //Update Angle
  kurve1.angle += js.left * kurve1.turnSpeed;
  kurve1.angle -= js.right * kurve1.turnSpeed;
  //Update Position
  kurve1.posX += cos(kurve1.angle / 360 * PI) * kurve1.moveSpeed;
  kurve1.posY -= sin(kurve1.angle / 360 * PI) * kurve1.moveSpeed;

  //Check For Collision
  if((int(kurve1.posX) != kurve1.prevPosX or int(kurve1.posY) != kurve1.prevPosY) and matrix[int(kurve1.posY)][int(kurve1.posX)] == 1 ){
    GameOver();
  }
  //Check For Diagonal Jump
  //Check if going UP
  
  else if(kurve1.prevPosY - 1 == int(kurve1.posY)){
    //CHeck if going RIGHT
    if(kurve1.prevPosX + 1 == int(kurve1.posX)){

      if(matrix[int(kurve1.posY) + 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) - 1] == 1){
        //GAME OVER
        // GameOver();
      }
    }
    else if(kurve1.prevPosX - 1 == int(kurve1.posX)){

      if(matrix[int(kurve1.posY) + 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) + 1] == 1){
        //GAME OVER
        // GameOver();
      }
    }
  }
  //Check if going DOWN
  else if (kurve1.prevPosY + 1 == int(kurve1.posY)){

    //CHeck if going RIGHT
    if(kurve1.prevPosX + 1 == int(kurve1.posX)){

      if(matrix[int(kurve1.posY) - 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) - 1] == 1){
        //GAME OVER
        // GameOver();
      }
    }
    //Check if going LEFT
    else if(kurve1.prevPosX - 1 == int(kurve1.posX)){

      if(matrix[int(kurve1.posY) - 1][int(kurve1.posX)] == 1 and matrix[int(kurve1.posY)][int(kurve1.posX) + 1] == 1){
        //GAME OVER
        // GameOver();
      }
    }
  }
  
  //Update matrix to 1 at posX, posY
  matrix[int(kurve1.posY)][int(kurve1.posX)] = 1;
  //Draw Kurve on that pixel
  tft.drawPixel(kurve1.posX, kurve1.posY, TFT_BLUE);

  //Update Previous Positions At The End
  kurve1.prevPosX = int(kurve1.posX);
  kurve1.prevPosY = int(kurve1.posY);
}

void GameOver(){
  tft.fillScreen(TFT_BLACK);
  memset(matrix, 0, sizeof(matrix));
}
