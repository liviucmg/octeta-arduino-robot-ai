//constantele motoarelor:
int MOTOR1_PIN1 = 3;
int MOTOR1_PIN2 = 5;
int MOTOR2_PIN1 = 6;
int MOTOR2_PIN2 = 9;

struct movement {
  int mode;
  int ticks;
};

void setup()
{
  //setam slot-urile motoarelor pe OUTPUT
  pinMode(MOTOR1_PIN1, OUTPUT); //
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);

  // setam toate slot-urile cu senzori pe INPUT pt a primi informatie
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

movement m[10];
int
  moves = 0,
  colorThresholdFata = 600,
  colorThresholdSpate = 750,
  distThreshold = 350,
  ignoreWhiteTicks = 0;

const int
  MOVE_MODE_FATA = 0,
  MOVE_MODE_STANGA = 1,
  MOVE_MODE_DREAPTA = 2,
  MOVE_MODE_SPATE = 3;
  
const int
  ROTATE_TICKS = 200,
  IGNORE_WHITE_TICKS = 100;

void loop()
{
  int S_FS = analogRead(0); // fata stanga // culoare stanga
  int S_FD = analogRead(1); // fata dreapta // culoare dreapta
  int S_SS = analogRead(3); // spate stanga // lat dr
  int S_SD = analogRead(2); // spate dreapta // lat stg
  int dist = analogRead(4); // distanta
  /*
  Serial.print(S_FS);
  Serial.print(" ");
  Serial.print(S_FD);
  Serial.print(" ");
  Serial.print(S_SS);
  Serial.print(" ");
  Serial.print(S_SD);
  Serial.println("");
  */
  if (true || moves == 0)  {    
//  if (ignoreWhiteTicks == 0) {
    if (
      S_SS < colorThresholdSpate &&
      S_SD < colorThresholdSpate
    ) {
        Serial.println("Spate doua;");        
        moves = 0;
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS * 4;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;    
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
    }   
    else if (
      S_FS < colorThresholdFata &&
      S_FD < colorThresholdFata
    ) {
        Serial.println("Fata doua;");        
        moves = 0;
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS * 4;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;    
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
    }
    else if (
      S_FS < colorThresholdFata &&
      S_SS < colorThresholdSpate
    ) {
        Serial.println("Stanga doua;");        
        moves = 0;
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;    
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
    }
    else if (
      S_FD < colorThresholdFata &&
      S_SD < colorThresholdSpate
    ) {
        Serial.println("Dreapta doua;");        
        moves = 0;
        m[moves].mode = MOVE_MODE_STANGA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;    
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
    }
    else if (S_FS < colorThresholdFata) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        moves = 0;
        Serial.println("Fata stanga; il vad in fata;");
        m[moves].mode = MOVE_MODE_STANGA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_SPATE;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
      else {
        moves = 0;
        Serial.println("Fata stanga;");
        m[moves].mode = MOVE_MODE_SPATE;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
    }
    else if (S_FD < colorThresholdFata) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        moves = 0;
        Serial.println("Fata dreapta; il vad in fata;");
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_SPATE;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
      else {
        moves = 0;
        Serial.println("Fata dreapta;");
        m[moves].mode = MOVE_MODE_SPATE;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        m[moves].mode = MOVE_MODE_STANGA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
    }
    else if (S_SS < colorThresholdSpate) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        moves = 0;
        Serial.println("Spate stanga; il vad in fata;");
        m[moves].mode = MOVE_MODE_STANGA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
      else {
        moves = 0;
        Serial.println("Spate stanga;");
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
    }
    else if (S_SD < colorThresholdSpate) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        moves = 0;
        Serial.println("Spate dreapta; il vad in fata;");
        m[moves].mode = MOVE_MODE_DREAPTA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
      else {
        moves = 0;
        Serial.println("Spate dreapta;");
        m[moves].mode = MOVE_MODE_STANGA;
        m[moves].ticks = ROTATE_TICKS;
        moves++;
        m[moves].mode = MOVE_MODE_FATA;
        m[moves].ticks = ROTATE_TICKS * 2;
        moves++;
        ignoreWhiteTicks = IGNORE_WHITE_TICKS;
      }
    }
  }
  else {
    ignoreWhiteTicks--;
  }
  
  if (moves > 0) {
    m[moves - 1].ticks--;
      //Serial.println(m[moves - 1].ticks);
         
    // Cum ne miscam astazi?
    switch(m[moves - 1].mode) {
      case MOVE_MODE_FATA:
        go(255, 240);
        break;
        
      case MOVE_MODE_STANGA:
        go(-255, 255);
        break;
        
      case MOVE_MODE_DREAPTA:
        go(255, -255);
        break;
        
      case MOVE_MODE_SPATE:
        go(-255, -255);
        break;
    } 
    
    // Termina move-ul
    if (m[moves - 1].ticks == 0) {
      Serial.println("Am terminat un move.");
      moves--;
    }
  }
  else {
    // Default move -> attack!
    go(255, 240);     
    if (random(0, 1000) == 0) {
      m[0].mode = MOVE_MODE_STANGA;
      m[0].ticks = ROTATE_TICKS;
      moves++;
    }
  }  
}

void go(int speedLeft, int speedRight) {
  if (speedLeft > 0) {
    analogWrite(MOTOR1_PIN1, speedLeft);
    analogWrite(MOTOR1_PIN2, 0);
  }
  else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speedLeft);
  }
  
  if (speedRight > 0) {
    analogWrite(MOTOR2_PIN1, speedRight);
    analogWrite(MOTOR2_PIN2, 0);
  }
  else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -speedRight);
  }
}
