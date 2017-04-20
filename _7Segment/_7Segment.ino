/*
*/

char mapping[7] = {2, 3, 4, A4, A5, A2, A1};
bool nums[10][7] = {{1, 1, 1, 0, 1, 1, 1},//0
                  {0, 0, 1, 0, 0, 0, 1},//1
                  {1, 1, 0, 1, 0, 1, 1},//2
                  {0, 1, 1, 1, 0, 1, 1},//3
                  {0, 0, 1, 1, 1, 0, 1},//4
                  {0, 1, 1, 1, 1, 1, 0},//5
                  {1, 1, 1, 1, 1, 1, 0},//6
                  {0, 0, 1, 0, 0, 1, 1},//7
                  {1, 1, 1, 1, 1, 1, 1},//8
                  {0, 1, 1, 1, 1, 1, 1} };//9


void displayGear(char gear){
    if(gear < 0){
      gear = 0;
    }
    
    for(int j=0; j<7; j++){
      digitalWrite(mapping[j], nums[gear][j]);
    }
}

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    delay(100); // wait for serial port to connect. Needed for native USB port only
  }

  displayGear(0);
}


void sequence(){
  for(int i = 0; i<10; i++){
    for(int j=0; j<7; j++){
      digitalWrite(mapping[j], nums[i][j]);
    }
    delay(1000);     
  }
}

// the loop function runs over and over again forever
void loop() {
  static char lastGear = 0;
  char gear = 0;

  if (Serial.available()){
    delay(100);
    gear = Serial.read();
    if(gear != lastGear){
      displayGear(gear);
      lastGear = gear;
    }
  }else{  
    delay(10);
  }
}

