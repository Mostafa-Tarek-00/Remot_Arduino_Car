#include <SoftwareSerial.h>
SoftwareSerial BT_Serial(2, 3); // RX, TX
#include <IRremote.h>
const int RECV_PIN = A5;
IRrecv irrecv(RECV_PIN);
decode_results results;

#define enA 10
#define in1 9
#define in2 8
#define in3 7
#define in4 6
#define enB 5 

#define servo A4

#define right_ir A0
#define lift_ir A1

#define echo A2
#define trigger A3

int distance_L, distance_F = 30, distance_R;
long distance;
int set = 20;

int bt_ir_data;
int Speed = 130;  
int mode=0;
int IR_data;


long IRremote_data(){
     if(results.value==0xFF02FD){IR_data=1;}  
else if(results.value==0xFF9867){IR_data=2;} 
else if(results.value==0xFFE01F){IR_data=3;} 
else if(results.value==0xFF906F){IR_data=4;} 
else if(results.value==0xFF629D || results.value==0xFFA857){IR_data=5;} 
else if(results.value==0xFF30CF){IR_data=8;} 
else if(results.value==0xFF18E7){IR_data=9;} 
else if(results.value==0xFF7A85){IR_data=10;} 
return IR_data;
}

void servoPulse (int pin, int angle){
int pwm = (angle*11) + 500; // Convert angle to microseconds
 digitalWrite(pin, HIGH);
 delayMicroseconds(pwm);
 digitalWrite(pin, LOW);
 delay(50);
}


// Ultrasonic_read
long Ultrasonic_read(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  distance = pulseIn (echo, HIGH);
  return distance / 29 / 2;
}

void compareDistance(){
       if (distance_L > distance_R){
  turnLeft();
  delay(350);
  }
  else if (distance_R > distance_L){
  turnRight();
  delay(350);
  }
  else{
  backword();
  delay(300);
  turnRight();
  delay(600);
  }
}

void Check_side(){
    Stop();
    delay(100);
 for (int angle = 70; angle <= 160; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    distance_L = Ultrasonic_read();
    delay(100);
  for (int angle = 90; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }
    delay(500);
    distance_R = Ultrasonic_read();
    delay(100);
 for (int angle = 0; angle <= 90; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    compareDistance();
}

void forword(){
digitalWrite(in1, HIGH); //right forward
digitalWrite(in2, LOW);  //right backword
digitalWrite(in3, LOW);  //left backword
digitalWrite(in4, HIGH); //left forward
}

void backword(){
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
}

void turnRight(){
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
}

void turnLeft(){
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
}

void Stop(){
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
}

void setup(){

pinMode(right_ir, INPUT);
pinMode(lift_ir, INPUT);

pinMode(echo, INPUT );
pinMode(trigger, OUTPUT);

pinMode(enA, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
pinMode(enB, OUTPUT);

irrecv.enableIRIn();
irrecv.blink13(true);

Serial.begin(9600);
BT_Serial.begin(9600); 

pinMode(servo, OUTPUT);

 for (int angle = 70; angle <= 140; angle += 5)  {
   servoPulse(servo, angle);  }
 for (int angle = 140; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }

 for (int angle = 0; angle <= 70; angle += 5)  {
   servoPulse(servo, angle);  }
delay(500);
}


void loop(){  

if(BT_Serial.available() > 0){
bt_ir_data = BT_Serial.read(); 
Serial.println(bt_ir_data);     
if(bt_ir_data > 20){Speed = bt_ir_data;}      
}

if (irrecv.decode(&results)) {
Serial.println(results.value,HEX);
bt_ir_data = IRremote_data();
Serial.println(bt_ir_data); 
irrecv.resume(); // Receive the next value
delay(100);
}

     if(bt_ir_data == 8){mode=0; Stop();}    //Android Application and IR Remote Control   
else if(bt_ir_data == 9){mode=1; Speed=130;} //Line Follower
else if(bt_ir_data ==10){mode=2; Speed=255;} //Obstacle Avoiding

analogWrite(enA, Speed);
analogWrite(enB, Speed);

if(mode==0){     
  // Key Control Command
     if(bt_ir_data == 1){forword(); } // forward
else if(bt_ir_data == 2){backword();} // back
else if(bt_ir_data == 3){turnLeft();} // left
else if(bt_ir_data == 4){turnRight();}// right
else if(bt_ir_data == 5){Stop(); }    // Stop

// Voice Control Command   
else if(bt_ir_data == 6){turnLeft();  delay(400);  bt_ir_data = 5;}
else if(bt_ir_data == 7){turnRight(); delay(400);  bt_ir_data = 5;}
}

if(mode==1){    
// Line Follower Control
if((digitalRead(right_ir) == 0)&&(digitalRead(lift_ir) == 0)){forword();}
if((digitalRead(right_ir) == 1)&&(digitalRead(lift_ir) == 0)){turnRight();}
if((digitalRead(right_ir) == 0)&&(digitalRead(lift_ir) == 1)){turnLeft();}
if((digitalRead(right_ir) == 1)&&(digitalRead(lift_ir) == 1)){Stop();}
} 

if(mode==2){    
// Obstacle Avoiding Control
 distance_F = Ultrasonic_read();
 Serial.print("S=");Serial.println(distance_F);
  if (distance_F > set){forword();}
    else{Check_side();}
}

delay(10);
}
