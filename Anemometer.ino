//includes math library in order to use the natural logarithm at windspeed calculations
#include <math.h>

//initialsing  LEDs
int boardLed = D7;

//initialsing various variables used in the Hall Effect <-> RPM calculations
int HE = A0;
int counter = 0;
int Threshold = 100;
bool HEstatus = false;
int clockCounter = 0;
int measuredTime = 0;
int analogwaarde = 0;
float Tijd = 0;
float Toeren = 60;
int teller=0;
String ans = 0;

//windspeed variables
float U2 = 0;
float Uz = 0;
int hz = 1;

//defining a timer
Timer minuteTimer(60000,everyMinute);
void setup()
{
    //inputs and outputs
    pinMode(boardLed,OUTPUT);
    pinMode(HE,INPUT);

    //Start Outputs
    Serial.begin(9600);
    minuteTimer.start();
    Particle.variable("measuredTime",measuredTime);
    Particle.variable("analogRead",analogwaarde);
    Particle.variable("RPM",Toeren);
    Particle.variable("Teller", teller);

    //flashes the onboard LED 3 times
    digitalWrite(boardLed,HIGH);
    delay(100);
    digitalWrite(boardLed,LOW);
    delay(100);
    digitalWrite(boardLed,HIGH);
    delay(100);
    digitalWrite(boardLed,LOW);
    delay(100);
    digitalWrite(boardLed,HIGH);
    delay(100);
    digitalWrite(boardLed,LOW);

    //starts clock
    clockCounter = millis();


    Serial.println("Begin_metingen");
    delay(500);
}

/*Script that calculates the time between two rotations.
 Uses the "hall effect status': HEstatus to check wether the previous reading was in the same 'field'.
 if a full rotation is completed it will publish some related values */
void loop()
{
   /* if the readings from the sensor are higher then a certain threshold this will run*/
   if (analogRead(HE)>Threshold)
   {
     /*if the HEstatus of the previous reading was false it means that the previous reading was below the threshold.
     The newly read value is the first above the threshold.
     This means that a rotation has taken place*/

      if(HEstatus ==false)
      {
        /*The counter 'teller' increases by 1. this is used later in the code*/
          teller++;

          /* the ration and RPM are caculated. */
          measuredTime = millis() - clockCounter;
          clockCounter = millis();
          analogwaarde = analogRead(HE);
          Toeren = (0.95 * Toeren) + 0.05*(60000.0/(float)measuredTime);

          /*every three rotations these values will be published*/
          if(teller%3==1)
          {
            Spark.publish("Rotatietijd",(String) measuredTime);
            Spark.publish("RPM",(String) Toeren);
            Serial.print("Rotatietijd");
            Serial.println((String) measuredTime);
            Serial.print("Toeren:");
            Serial.println((String) Toeren);

          }

          /* The HEstatus is changed to true*/
          HEstatus = true;

      }
      /*if the readings are higher then the threshold and the HE status is True,
      nothing needs to be done*/
      else
      {
          //do nothing
      }
   }

    else
    {
      /* if the HE status is true but the readings are below the threshold:
       the HE status needs to be changed to false. The magnet has passed */
        if(HEstatus == true)
        {
            HEstatus = false;
        }

        else
        {
            //do nothing
        }
    }


//calculates the RPM every minute
}
void everyMinute()
{
  Serial.print("aantal toeren deze minuut: ");
  Serial.println(teller);
  /*only publishes if the windmeter rotated at least once.
  This prevents series of trivial values if the windmeter is idle.*/
  if(teller>0)
  {
    Particle.publish("RPM2",(String) teller);
  }
  /* cals another method that converts the RPM count to windspeeds*/
  Windsnelheid(teller);
  teller=0;
}

//converts the RPM to a windspeed at 2 m
void Windsnelheid(int Tellerin)
{
  /* only calculates non trivial windspeeds*/
  if(Tellerin>0)
  {
    Uz = (0.0931*(float)Tellerin)+0.498;

    /*the relation between windspeed at height z and at the standardized height of 2.*/
  //  U2 = Uz*(4.87/(log(67.8*hz-5.42)));

  /*publishes*/
    Particle.publish("Windsnelheid",(String) Uz);
    Serial.println((String) Uz);

    /*calls the method that converts the windspeeds to beaufort*/
    Beaufort(Uz);
  }
}

/* method that converts the windspeeds to beaufort and publishes the result*/
void Beaufort(float Windin)
{

/*rounds to 2 decimal places*/
String Wind2dec = String(Windin,2);

/*Several if statements for different windspeeds
they publish the windspeed and the relevant Beaufort scale as a String.
This used in a twitter message.*/
  if(Windin>0.0 && Windin<=0.2)
  {
    ans = Wind2dec + " m/s    (0 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>0.2 && Windin<=1.5)
  {
    ans = Wind2dec + " m/s \n\r(1 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>1.5 && Windin<=3.3)
  {
    ans = Wind2dec + " m/s \n\r(2 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>3.3 && Windin<=5.4)
  {
    ans = Wind2dec + " m/s \n\r(3 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>5.7 && Windin<=7.9)
  {
    ans = Wind2dec + " m/s \n\r(4 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>7.9 && Windin<=10.7)
  {
    ans = Wind2dec + " m/s \n\r(5 op schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }

  if(Windin>10.7)
  {
    ans = Wind2dec + " m/s \n\r(6 of meer op de schaal van Beaufort)\n\r";
    Particle.publish("Beaufort",ans);
  }
}
