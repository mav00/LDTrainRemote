#include <Lpf2Hub.h>
#include <Lpf2HubConst.h>
#include <Bounce2.h>

Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

//Pin declaration
#define BTN_MUSIC 25
#define BTN_LICHT 26
#define BTN_WASSER 27
#define BTN_STOP 14
#define PTI_SPEED 15

Bounce tasterMusic = Bounce();
Bounce tasterLight = Bounce();
Bounce tasterWater = Bounce();
Bounce tasterStop = Bounce();
int gLastStatePtiSpeed = 0;

static int gLightOn = 0;
static short gColor = NONE;
static int gSpeed =0;

void handlePoti()
{
  int ptiSpeed = analogRead(PTI_SPEED);
  gLastStatePtiSpeed = ptiSpeed;
  //Serial.println(ptiSpeed);
  int speed = 0;
  if ( ptiSpeed > 1100) speed = 64;     //Fast foreward
  else if (ptiSpeed > 800) speed = 32;  //normal forweard
  else if (ptiSpeed > 600) speed = 16;  //slow forefard (might not work on low battery) 
  else if (ptiSpeed > 400) speed = 0;   //stop
  else if (ptiSpeed > 200) speed = -32; //slow backward
  else speed = -64;                     //fast foreward

  if(speed != gSpeed)
  { 
    if(gSpeed == 0 && speed > 0)
    {
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    gSpeed =speed;
    myHub.setBasicMotorSpeed(port, speed);
  }
}


Color getNextColor()
{
    if(gColor == 255)
    { 
        gColor = 0;
        return (Color)0;
    }
       
    gColor++;
    
    if(gColor == NUM_COLORS)
    {   
        gColor =255;
    }
    return (Color) gColor;
} 

void handleButtons()
{
  if(tasterMusic.update())
  {
    if(tasterMusic.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::HORN);
      delay(100);
    }
  }
  if(tasterLight.update())
  {
    if(tasterLight.fell())
    {
      myHub.setLedColor(getNextColor());
      delay(100);
    }
  }
  if(tasterWater.update())
  {
    if(tasterWater.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::WATER_REFILL);
      delay(500);  
    }
  }
  if(tasterStop.update())
  {
    if(tasterStop.fell())
    {
      myHub.setBasicMotorSpeed(port, 0);
      delay(100);
    }
  }
}

void setup() {
  Serial.begin(115200);
  //define Pin Modes
  tasterMusic.attach(BTN_MUSIC, INPUT_PULLUP);
  tasterLight.attach(BTN_LICHT, INPUT_PULLUP);
  tasterWater.attach(BTN_WASSER, INPUT_PULLUP);
  tasterStop.attach(BTN_STOP, INPUT_PULLUP);

  myHub.init();
}
// This is the Arduino main loop function.
void loop() {

    if (myHub.isConnecting()) {
        myHub.connectHub();
        if (myHub.isConnected()) {
            Serial.println("We are now connected to the HUB");
        } else {
            Serial.println("We have failed to connect to the HUB");
        }
    }
    if (myHub.isConnected()) {
        handleButtons();
        handlePoti();
    } 
    delay(50);
}