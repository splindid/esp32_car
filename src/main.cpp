#include <Arduino.h>
#include <WiFi.h>
const char *SSID ="splendid";//填写你对应的用户名和密码
const char *password="qh2021dengwo";
const int Motor_INT1=5;///暂定的两个轮子的转向控制引脚
const int Motor_INT2=4;
//运用tcp通信来控制小车的运动
const  IPAddress  serivalIP(192,168,164,92);//tcp的地址
uint16_t serivalPort=1234;//tcp端口号，自己取的
WiFiClient  client;
int channel1=1,channel2=2;
int feq=2000;
int resolution=10;
bool val=0;
int fast=1023,medium=1000,low=500,stop=0;//定义转速的大小（即显示占空比）
char control;
void MOTOR_INIT(void)
{
     ledcSetup(channel1,feq,resolution);delay(20);
     ledcSetup(channel2,feq,resolution);delay(20);
     ledcAttachPin(Motor_INT1,channel1);delay(20);
     ledcAttachPin(Motor_INT2,channel2);delay(20);
     
}
void MOTOR_CONTROL(int control)
{   switch(control){
            case '1':ledcWrite(channel1,medium);delay(20);ledcWrite(channel2,medium);break;
            case '2':ledcWrite(channel1,stop);delay(20);ledcWrite(channel2,stop);break;
            case '3':ledcWrite(channel1,low);delay(20);ledcWrite(channel2,fast);break;
            case '4':ledcWrite(channel1,fast);delay(20);ledcWrite(channel2,low);break;
    }
}
void WiFi_init(const char *SSID,const char *password)
{
       WiFi.mode(WIFI_STA);
       WiFi.setSleep(false);
       WiFi.begin(SSID,password);
       while(WiFi.status()!=WL_CONNECTED)
       {
         Serial.print(".");
         delay(500);
       }
       Serial.println("WiFi connect!");
       Serial.println(WiFi.localIP());
       Serial.println("try to get the tcp");
}
void Project_init()
{
    WiFi_init(SSID,password);
    MOTOR_INIT();
}
/*任务一：获取串口传送数据，并把值变成全局变量然后进行扩展*/
void task1(void *pvParameters)
{
    while(1)
    {if(client.connect(serivalIP,serivalPort))
      {Serial.println("get in the tcp");
      client.println("hello the master");
        while(client.connected()||client.available())
        {
          if(client.available()>0)
          {
          control= client.read();
          client.println(control);
          delay(200);
              switch(control){
              case '1':client.println("forward!");break;
              case '2':client.println("Stop!");break;
              case '3':client.println("turn left!");break;
              case '4':client.println("turn right!");break;
            }
          }
            delay(200);
        }
      }}
}
/*任务二：运用pwm波控制左右轮子运动*/
void task2(void *pvParameters)
{
  while(1)
  {
    MOTOR_CONTROL(control);
    delay(10);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
  Project_init();
  xTaskCreate(task1,
            "task1",
            4096,
            NULL,
            1,
            NULL);
  xTaskCreate(
            task2,
            "task2",
            4096,
            NULL,
            2,
            NULL);
}
void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);
}
