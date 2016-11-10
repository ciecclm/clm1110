unsigned int HighLen = 0;
unsigned int LowLen  = 0;
unsigned int Len_mm  = 0;
int count=0;
int back=0;
int state = 1;
int k1 = 0;
int k2 = 0;
int s=0;
int oldlen;
int down1;
int down2;
int down3;
int down4;
unsigned long Len_mmt=3100;
class robomodule
{
public:
  robomodule();
  robomodule(int port);
  int reset(int g,int n);             //复位
  int setmode(int mode,int g,int n);   //模式设置
  int speedwheel(int temp_pwm,int temp_velocity,int g,int n);  //速度控制模式
private:
  int serialport;  //通讯端口
  int group;       //驱动组号 
  int number;      //驱动编号
  
};
robomodule::robomodule()
{
    serialport=3;
    group=0;
    number=1;
}
robomodule::robomodule(int port)
{
    serialport=port;
    group=0;
    number=1;
}
int robomodule::reset(int g=0,int n=0)
{ 
  noInterrupts();
  group=g;
  number=n;
  char cnum=(char)(number&0xff);
  char temp=0;
  temp=(char)((cnum<<4)&0xf0);
  char command[14]={0x00,0x00,0x00,(char)group,temp,0x08,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
  int i;
  if(serialport==1)
      for(i=0;i<14;i++)
         Serial.write(command[i]);
  if(serialport==2)
      for(i=0;i<14;i++)
         Serial2.write(command[i]);
  if(serialport==3)
      for(i=0;i<14;i++)
         Serial3.write(command[i]);
  delay(500);
  interrupts();
  return 0;
}
int robomodule::setmode(int mode,int g=0,int n=0)
{
  group=g;
  number=n;
  char cnum=(char)(number&0xff);
  char temp=0;
  temp=(char)(((cnum<<4)|0x01)&0xf1);
  char command[14]={0x00,0x00,0x00,(char)group,temp,0x08,(char)mode,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
  int i;
  if(serialport==1)
      for(i=0;i<14;i++)
         Serial.write(command[i]);
  if(serialport==2)
      for(i=0;i<14;i++)
         Serial2.write(command[i]);
  if(serialport==3)
      for(i=0;i<14;i++)
         Serial3.write(command[i]);
  delay(4);
  return 0;
}
int robomodule::speedwheel(int temp_pwm,int temp_velocity,int g=0,int n=0)
{
    noInterrupts();
    group=g;
    number=n;
    char cnum=(char)(number&0xff);
    char temp=0;
    temp=(char)(((cnum<<4)|0x04)&0xf4);
    char command[14]={0x00,0x00,0x00,(char)group,temp,0x08,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    int i;
    command[6] = ( char)((temp_pwm>>8)&0xff);
    command[7] = ( char)(temp_pwm & 0xff);
    command[8] = ( char)((temp_velocity>>8) & 0xff);
    command[9] = ( char)(temp_velocity & 0xff);
    if(serialport==1)
        for(i=0;i<14;i++)
           Serial.write(command[i]);
    if(serialport==2)
        for(i=0;i<14;i++)
           Serial2.write(command[i]);
    if(serialport==3)
        for(i=0;i<14;i++)
           Serial3.write(command[i]);
   delay(5);
   interrupts();
   return 0;
}
robomodule od(3);
void setup() {
  // put your setup code here, to run once: 
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial2.begin(9600);
    Serial3.begin(115200);
    pinMode(7, OUTPUT);
    pinMode(2,INPUT_PULLUP);
    pinMode(3,INPUT_PULLUP);
   // attachInterrupt(2, turningRIGHT, HIGH);//中断，2号高电平时去运行turningRIGHT（）函数
    //attachInterrupt(3, turningLEFT, HIGH); //中断，3号高电平时去运行turningleft（）函数
    digitalWrite(7, LOW);
    delay(10);
    Serial3.write("AT+CAN_FRAMEFORMAT=0,0,0,0\r\n");
    delay(1000);
    digitalWrite(7, HIGH);
    delay(500);
    od.reset();
    od.setmode(3);
    delay(100);
}
void turningRIGHT()
{
   noInterrupts();
   down1=digitalRead(2);
   down2=digitalRead(3);
   if(down1==HIGH||down2==HIGH)
   {
       od.speedwheel(5000,-6000);
       delay(1000); 
       od.speedwheel(5000,6000,0,1);
       od.speedwheel(5000,6000,0,2);
       od.speedwheel(5000,-6000,0,3);
       od.speedwheel(5000,-6000,0,4);
       delay(1000);
   }
   else
   {
       od.speedwheel(5000,6000,0,1);
       od.speedwheel(5000,6000,0,2);
       od.speedwheel(5000,-6000,0,3);
       od.speedwheel(5000,-6000,0,4);
   }
   interrupts();
}
void turningLEFT()
{
   noInterrupts();
   down1=digitalRead(2);
   down2=digitalRead(3);
   if(down1==HIGH||down2==HIGH)
   {
       od.speedwheel(5000,-6000);
       delay(1000); 
       od.speedwheel(5000,6000,0,1);
       od.speedwheel(5000,6000,0,2);
       od.speedwheel(5000,-6000,0,3);
       od.speedwheel(5000,-6000,0,4);
   }
   else
   {
       od.speedwheel(5000,-6000,0,1);
       od.speedwheel(5000,-6000,0,2);
       od.speedwheel(5000,6000,0,3);
       od.speedwheel(5000,6000,0,4);
   }
   interrupts();
  
}
int measure()
{
    noInterrupts();
    Serial2.flush();     // clear receive buffer of serial port
    Serial2.write(0X55); // trig US-100 begin to measure the distance
    delay(100);          //delay 500ms to wait result
    if(Serial2.available() >= 2)                    //when receive 2 bytes 
    {
        HighLen = Serial2.read();                   //High byte of distance
        LowLen  = Serial2.read();                   //Low byte of distance
        Len_mm  = HighLen*256 + LowLen;             //Calculate the distance
        if((Len_mm > 1) && (Len_mm < 10000))        //normal distance should between 1mm and 10000mm (1mm, 10m)
        {
            Serial.print("Present Length is: ");   //output the result to serial monitor
            Serial.print(Len_mm, DEC);             //output the result to serial monitor
            Serial.println("mm");                  //output the result to serial monitor
        }
    }
    interrupts();
    return Len_mm;
}
void loop() 
{
     
     // put your main code here, to run repeatedly:
     if(s==0)
     {   
         od.speedwheel(5000,6000);
         delay(2500);
         od.speedwheel(5000,0);//刹车放铲
         delay(1000);
         od.speedwheel(5000,6000,0,1);
         od.speedwheel(5000,6000,0,2);
         od.speedwheel(5000,-6000,0,3);
         od.speedwheel(5000,-6000,0,4);
         delay(1000);//上坡后右转大概45度
         s=1;
     }
     if(oldlen=measure()<600)
     {
         od.speedwheel(5000,6000);
     }
     else
     {
         od.speedwheel(5000,6000,0,1);
         od.speedwheel(5000,6000,0,2);
         od.speedwheel(5000,-6000,0,3);
         od.speedwheel(5000,-6000,0,4);
         //oldlenmeasure()
         count++;
         if(count<1000)
         {
             od.speedwheel(5000,6000);
             delay(random(1000,3000));//延时随机3秒以内
         }
   }
}
