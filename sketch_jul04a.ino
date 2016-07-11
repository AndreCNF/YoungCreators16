int distancia_pin = A0;
int bumperLeft_pin = 7;
int bumperRight_pin = 8;
int led_pin = 13;
int led_green = A1;
int led_red = A2;
int led_blue = A4;
int distancia;
int pos = 0;
int potencia = 0;
//int range_pot = A3;
int fotoleft = A5;
int fotoright = A3;
int resultdist = 400;
int active_leds = 0; //0 = verde e vermelho; 1 = vermelho; 2 = azul
int leftlight;
int rightlight;
int flag_dark = 0;
int flag_rotatation_right = 0;
int flag_rotatation_left = 0;
int flag_turn = 0;
int flag_bumper_right = 0;
int flag_bumper_left = 0;
int peso_left = 0;
int peso_right = 0;
//int button = B2;

#include <Metro.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

Metro ledMetro = Metro(500); 
Metro sweepMetro = Metro(200);
Metro turnMetro = Metro(300); //Coming from dark corner

Metro quick_backMetro = Metro(400); //To back after bumping
Metro bumperMetro = Metro(400); //Turn after bumping


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(led_pin, OUTPUT);
  pinMode(bumperLeft_pin, INPUT_PULLUP);
  pinMode(bumperRight_pin, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  myservo.attach(9);
  pinMode(11, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_blue, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //choque e volta
  leftlight = analogRead(fotoleft);
  rightlight = analogRead(fotoright);

  //Serial.println(leftlight);
  //Serial.println(rightlight);
  //int resultadoMapPot = map(potencia, 1000, 0, 50, 2000);
  
  if(leftlight < 700 && rightlight < 700)
  {
    DiffTurn(-250, -250);
    flag_dark = 1;
    flag_rotatation_right = 1;
    flag_rotatation_left = 0;
    peso_right = 0;
    peso_left = 0;
    pos = 20;
    active_leds = 2;
  }
  
  else if(flag_dark == 1) 
  { 
    //SERVO SWEEP
    DiffTurn(0, 0);

    if(flag_rotatation_right == 1 && sweepMetro.check() == 1)
    {
      myservo.write(pos);
      
      if(pos < 70)
      {
        pos += 10;
      }
      else
      {
        flag_rotatation_right = 0;
        flag_rotatation_left = 1;
        pos = 110;
      }

      peso_right += analogRead(distancia_pin);
      sweepMetro.reset();
    }

    else if(flag_rotatation_left == 1 && sweepMetro.check() == 1)
    {
      myservo.write(pos);
      
      if(pos < 160)
      {
        pos += 10;
      }
      else
      {
        flag_rotatation_right = 0;
        flag_rotatation_left = 0;
        flag_turn = 1;
        Serial.print(peso_left);
        Serial.print(" - ");
        Serial.println(peso_right);
        pos = 90;
        myservo.write(pos);
        turnMetro.reset();
      }

      peso_left += analogRead(distancia_pin);
      sweepMetro.reset();
    }

    else if(flag_turn == 1)
    {
      if(peso_left > 800 && peso_right > 800)
      {
        DiffTurn(-255,-255);
        if(turnMetro.check() == 1)
        {
          DiffTurn(0,0);
          flag_rotatation_right = 1;
          flag_dark = 1;
          flag_turn = 0;
        }
        turnMetro.reset();
      }
      else
      {
        if(peso_left < peso_right)
        {
          DiffTurn(255,-255);
          Serial.println("turning left");
          if(turnMetro.check() == 1)
          {
            DiffTurn(0,0);
            flag_turn = 0;
            flag_dark = 0;
            turnMetro.reset();
          }
          
        }
        else
        {
          DiffTurn(-255,255);
          Serial.println("turning right");
          if(turnMetro.check() == 1)
          {
            DiffTurn(0,0);
            flag_turn = 0;
            flag_dark = 0;
            turnMetro.reset();
          }
          
        }
      }
    }
  }

  //Go Forward
  else
  {
    if(digitalRead(bumperRight_pin) == 1  || flag_bumper_right == 1)
    {
      if(flag_turn == 0)
      {
        DiffTurn(-150, -150);
        if(quick_backMetro.check() == 1) 
         {
         Serial.println("backing up");
         DiffTurn(-255, 255);
         flag_turn = 1;
         quick_backMetro.reset();
         bumperMetro.reset();
          }
      }
      flag_bumper_right = 1;

      
      if(bumperMetro.check() == 1)
      {
         Serial.println("turning, bumper");
         DiffTurn(0, 0);
         bumperMetro.reset();
         flag_bumper_right = 0;
         flag_turn = 0;
      }
    }
    else if(digitalRead(bumperLeft_pin) == 1  || flag_bumper_left == 1)
    {
      if(flag_turn == 0)
      {
        DiffTurn(-150, -150);
        if(quick_backMetro.check() == 1) 
         {
         DiffTurn(255, -255);
         flag_turn = 1;
         quick_backMetro.reset();
         bumperMetro.reset();
          }
      }
        flag_bumper_left = 1;

      if(bumperMetro.check() == 1)
      {
         DiffTurn(0, 0);
         bumperMetro.reset();
         flag_bumper_left = 0;
         flag_turn = 0;
      }
    }
    else
    {
      DiffTurn(200, 190);
    }
  }

  //Gestão LEDS
  distancia = analogRead(distancia_pin);
  LEDInterval();
  //Serial.println(distancia);
  if(distancia < 150 && flag_dark == 0)
  {
    active_leds = 0;
  }
  else if(distancia > 150 && flag_dark == 0)
  {
    active_leds = 1;
  }
  if(ledMetro.check() == 1)
  {
    if(active_leds == 0)
    {
      digitalWrite(led_blue, LOW);
      digitalWrite(led_green, HIGH);
      ChangeLED(led_red);
    }
    else if(active_leds == 1)
    {
      digitalWrite(led_green, LOW);
      digitalWrite(led_blue, LOW);
      ChangeLED(led_red);
    }
    else if(active_leds == 2)
    {
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, LOW);
      ChangeLED(led_blue);
    }
    
    ledMetro.reset();
  } 

  Serial.print("bumpers:  ");
  Serial.print(flag_bumper_left);
  Serial.print(" - ");
  Serial.println(flag_bumper_right);
}
  /* POTENCIOMETRO
  potencia = analogRead(range_pot);
  resultadoMapPot = map(potencia, 1000, 0, 0, 150);
  analogWrite(11, resultadoMapPot);
  analogWrite(3, resultadoMapPot);
  distancia = analogRead(distancia_pin);
  Serial.println(distancia);
  if(distancia>250){
    analogWrite(11, 0);
    analogWrite(3, 0);
    analogWrite(5, resultadoMapPot);
    analogWrite(6, resultadoMapPot);
    delay(1500);
    analogWrite(5, 0);
    analogWrite(6, 0);
  }
  delay(1500);
  analogWrite(11, 0);
  delay(2000);*/


void ChangeLED(int _led){
  if(digitalRead(_led) == HIGH){
    digitalWrite(_led, LOW);
  }
  else{
    digitalWrite(_led, HIGH);
  }
}

void LEDInterval(){
  if(distancia < 150)
  {
    resultdist = 1000;
  }
  else
  {
    resultdist = map(distancia, 150, 600, 400, 0);
  }
  ledMetro.interval(resultdist);
}

void DiffTurn(int speedLeft, int speedRight) {
 
 if (speedLeft > 0) 
 {  
  analogWrite(6, speedLeft);
  digitalWrite(11, LOW);
 }
 
 if (speedLeft < 0) 
 {  
  digitalWrite(6, LOW);
  analogWrite(11, speedLeft*-1); 
 }
 
 if (speedLeft == 0) 
 {  
  digitalWrite(6, LOW);
  digitalWrite(11, LOW); 
 }
  
 if (speedRight > 0 ) 
 {
  analogWrite(3, speedRight);
  digitalWrite(5, LOW);
 }
 
 if (speedRight < 0 ) 
 {
  digitalWrite(3, LOW);
  analogWrite(5, speedRight*-1); 
 }
 
 if (speedRight == 0)
 {
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
 }
  
}
