// C++ code
//

#include<Servo.h>
#include<LiquidCrystal.h>
#include<SoftwareSerial.h>
#include<string.h>

#define out_pir 13
#define in_pir 11
#define led 12
#define time 500


int out_data=0,in_data=0,person_count=0,i,flag=0;

//creating objects
Servo door_lock;
LiquidCrystal lcd(10, 9, 8, 7, 6, 5 );  // this are pin numbers in arduino

//from GSM module Tx pin will connect to pin 4 and Rx pin will connect to pin 3 
SoftwareSerial module(4,3);             //this are pin numbers of arduino

void setup()
{
  pinMode(out_pir,INPUT);
  pinMode(in_pir,INPUT);
  pinMode(led,OUTPUT);
  Serial.begin(9600);  		// setting the baud rate of serial monitor
  module.begin(9600);       // setting the baud rate of GSM module
  lcd.begin(16,2);          // set up the LCD's number of colum and row
  door_lock.attach(2);
  door_lock.write(0);
  digitalWrite(led,LOW);  
}

void loop()
{
  out_data=digitalRead(out_pir); //reading data from out sensor
  in_data=digitalRead(in_pir);   // reading data from in sensor
  
  // for traking in/out data
  Serial.print("out= ");
  Serial.print(out_data);
  Serial.print("in= ");
  Serial.println(in_data);

  if(out_data==1)
  {
    digitalWrite(led,HIGH);
    delay(time);
    digitalWrite(led,LOW);
    in_data=0;
    person_count++;  //everytime out sensor is on thats mean someone entering in the room and adding up to here
    flag=0;        // using flag for  element conflict between commands
    delay(time);  // waiting for .5 seconsa to pass through in sensor
  }
  else if(in_data==1)
  {
    
    digitalWrite(led,HIGH);
    delay(time);
    digitalWrite(led,LOW);
    out_data=0;
    person_count--;                                  // every time in sensor is on thats mean someone is going out from room so decreasing person count 
    flag=1;
    //preventing (-) value in in_data
    if(person_count<0)
    {
      person_count=0;
      flag=0;
    }
    
    delay(time);                                      // waiting for .5 seconds to pass through out sensor
  }
  //traking person count
  Serial.print("Person=");
  Serial.println(person_count);
  
  if(person_count==1 && flag ==0)
  {
    
    door_lock.write(0);
    lcd.clear();
    lcd.setCursor(0,0);                               // setting cursore from where it will start writing in LCD
    lcd.print("Someone at HOME");
    send_massage("*****SOMEONE AT HOME*****");      // Calling send_massage function
    flag=1;
  }
  else if(person_count==0 && flag==1)
  {
    lcd.clear();
    lcd.setCursor(0,0);                               // setting cursore from where it will start writing in LCD
    lcd.print("Home is EMPTY");
    
    //moving motor for 180 degree
    for(i=0;i<180;i++)
    {
      door_lock.write(i);
      
    }
    delay(1500);
    lcd.setCursor(0,1);                               // set cursore in the next line in LCD
    lcd.print("Door is locked");
    send_massage("***HOME IS EMPTY, DOOR IS LOCKED****");  //calling send_massage function
    door_lock.write(0); 							  // set the motor in initial state
    flag=0;
  }

}
// This code is for send notification to cell phone

void send_massage(String a)
{
  module.println("AT+CMGF=1");                       // set module in send SMS mode
  delay(1000);                                       // wait for 1 second
  module.println("AT+CMGS=\"+467200xxxxx\"\r");      // replace x with recivers mobile number 
  module.println(a);                                 // for print the massage
  delay(100);
  module.println((char)26);  						 // ASCII code for CTRL+Z(to exit)
  delay(time);
  
}

