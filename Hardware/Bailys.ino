/*************
 作品：鲸鱼刷卡机
 作者：黄家俊
 *************/
#include <Keypad.h>
#include <SPI.h>
#include <RFID.h>
/**********************************************蜂鸣器*****************************************************/
int BEEP=11;
/********************************************电机驱动*****************************************************/
int IN1=28;
int IN2=29;
/********************************************电机驱动*****************************************************/

/**********************************************数码管*****************************************************/
    //不带小数点
    unsigned char LED_0F[] = 
    {// 0   1    2    3  4  5    6    7  8  9    A    b  C    d    E    F    -
      0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf,0x0f
    };
    //带小数点的数字
    unsigned char LED_F[] = 
    {//0. 1. .2. 3. 4. 5. 6. 7. 8. 9.
      0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf,0x0f
    };
    unsigned char LED[4]; //用于LED的4位显示缓存
    int SCLK = 24;
    int RCLK = 23;
    int DIO = 22; //这里定义了那三个脚    //不带小数点
/**********************************************数码管*****************************************************/

/*******************************************矩阵键盘*****************************************************/
const byte ROWS = 4; 
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {3,4, 5, 6};//对应键盘：6 5 4 3
byte colPins[COLS] = {7, 8, 9, 10};//对应键盘：2 1 0 7
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
/*******************************************矩阵键盘******************************************************/

/********************************************RFID模块*****************************************************/
RFID rfid(53,2);    //53--读卡器MOSI引脚、2--读卡器RST引脚
//4字节卡序列号，第5字节为校验字节
unsigned char serNum[5];
//写卡数据
unsigned char writeDate[16] ={'G', 'e', 'e', 'k', '-', 'W', 'o', 'r', 'k', 'S', 'h', 'o', 'p', 0, 0, 0};
//原扇区A密码，16个扇区，每个扇区密码6Byte
unsigned char sectorKeyA[16][16] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
//新扇区A密码，16个扇区，每个扇区密码6Byte
unsigned char sectorNewKeyA[16][16] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
/********************************************RFID模块*****************************************************/

/*****************************************************/
//刷卡函数
//返回卡号
/*****************************************************/
unsigned int Pay()
{
  unsigned char i,tmp;
  unsigned char status;
  unsigned char str[MAX_LEN];
  unsigned char RC_size;
  unsigned char blockAddr;        //选择操作的块地址0～63
  //找卡
  rfid.isCard();
  //读取卡序列号
  if (rfid.readCardSerial())
  {
   return rfid.serNum[0];
  }
  else
  {
    return 0;
  }

  //选卡，返回卡容量（锁定卡片，防止多次读写）
  rfid.selectTag(rfid.serNum);
  
  //写数据卡
  blockAddr = 7;                //数据块7
  if (rfid.auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr/4], rfid.serNum) == MI_OK)  //认证
  {
    //写数据
    status = rfid.write(blockAddr, sectorNewKeyA[blockAddr/4]);
//    Serial.print("set the new card password, and can modify the data of the Sector: ");
//    Serial.println(blockAddr/4,DEC);
    //写数据
    blockAddr = blockAddr - 3 ; //数据块4
    status = rfid.write(blockAddr, writeDate);
    if(status == MI_OK)
    {
//      Serial.println("Write card OK!");
    }
  }

  //读卡
  blockAddr = 7;                //数据块7
  status = rfid.auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[blockAddr/4], rfid.serNum);
  if (status == MI_OK)  //认证
  {
    //读数据
    blockAddr = blockAddr - 3 ; //数据块4
    if( rfid.read(blockAddr, str) == MI_OK)
    {
 //     Serial.print("Read from the card ,the data is : ");
//      Serial.println((char *)str);
    }
  }
  
  rfid.halt();  
}
void Beep(char a)
{
  if(a==0)
  {
    digitalWrite(BEEP,LOW);
  }
  else
  {
    digitalWrite(BEEP,HIGH);
  }
}
/*****************************/
//电机驱动函数
//1和2分别控制正反转
//0为停止
/*****************************/
void RUN(char a)
{
  if(a==0) 
  {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,HIGH);
  }
  else if(a==1)
  {
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);   
  }
  else if(a==2)
  {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }
}
/*************初始化****************/
void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.init();//RFID模块初始化
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(SCLK,OUTPUT);
  pinMode(RCLK,OUTPUT);
  pinMode(DIO,OUTPUT); //让三个脚都是输出状态
  pinMode(BEEP,OUTPUT); //蜂鸣器
}
/************主函数*****************/
void loop()
{
  unsigned int Card_Num;
  unsigned char x,t,nn;
  unsigned char JE[4]={0,0,0,0};
  char *bb = "150202102365001000";
  char *cc = "150202102370001000";
  String aa="";
  char flag;
  unsigned int dd,bai,ge,shi;
   LED[0]=1;//数码管
   LED[1]=2;
   LED[2]=3;
   LED[3]=4; 
   t = flag = 0;//标志位初始化
/*********************输入金额**************************/
  while(1)
  {
    char cKey = customKeypad.getKey();
    char *p = &bb[0];
    if ((cKey>='0')&&(cKey<='9')&&(t<3)&&(cKey!='#')){
      JE[3 - t]=cKey-48;
      if(t==0) 
      {
        bai=ge=cKey;
        bb[17]=ge;
      }
      else if(t==1)
      {
        bb[16]=ge;
        shi=ge=cKey;
        bb[17]=ge;
      }
      else if(t==2)
      {
        ge=cKey;
        bb[15]=bai;
        bb[16]=shi;
        bb[17]=ge;
      }
      t++;
    }
    if ((cKey=='#')&&(t>0))
    {
      break;
    }
    if(t==0) LED4_Display (0x0f,0x0f,0x0f,0x0f);
    else if(t==1) LED4_Display (0x0f,JE[3],0x0f,0x0f);
    else if(t==2) LED4_Display (0x0f,JE[2],JE[3],0x0f);
    else if(t==3) LED4_Display (JE[0],JE[1],JE[2],JE[3]);
    //LED4_Display (JE[0],JE[1],JE[2],JE[3]);
  }
/***************刷卡并发送数据到服务器**********************/
  while(1)
  {
    Card_Num=Pay();
    if(Card_Num!=0)
    {
        Beep(1);
        for(dd=0;dd<200;dd++)
        {
          LED4_Display (JE[0],JE[1],JE[2],JE[3]);
          delay(1);
        }
          LED4_Display (0x0f,0x0f,0x0f,0x0f);
        Beep(0);
        if(Card_Num==87)
        {
          Serial.print(bb);
              
/********************等待服务器回应*************************/
          while(1)
          {
             while(Serial.available() > 0)
            {
              aa += char(Serial.read());
              delay(2);
            }
            if(aa.length() > 0) 
            {
/***************收到吞卡指令***********************/
              if(aa[0]=='A') 
              {
                Eating();//开始吞卡
                aa = "";
                flag = 1;
                break;
              }
/***************收到错误信息***********************/
              else if(aa[0]=='B') 
              {
                aa = "";
                flag = 1;
                break;
              }
/******************收到余额*************************/
              else
              {
                JE[3]=aa[0]-48;
                JE[2]=aa[1]-48;
                JE[1]=aa[2]-48;
                   for(dd=0;dd<1500;dd++)
                  {
                    LED4_Display (JE[0],JE[1],JE[2],JE[3]);
                    delay(1);
                  }   
                aa = "";
                flag = 1;
                break;          
              }
              aa = "";
            }
          }
        }
        if(flag==1) break;
        for(nn=0;nn<6;nn++)
        {
          Beep(1);
          delay(200);
          Beep(0);
          delay(200);
        }
        break;
        
    }
  }
}

/*****************吞卡动作******************/
//电机转动数码管闪烁
/*******************************************/
void Eating()
{
  unsigned int dd,bb;
  RUN(1);
  for(bb=0;bb<5;bb++)
  {
    Beep(1);
    for(dd=0;dd<200;dd++)
    {
    LED4_Display (16,16,16,16);
    delay(1);
    }
    Beep(0);
    for(dd=0;dd<200;dd++)
    {
    LED4_Display (9,9,9,9);
    delay(1);
    }
  }
  RUN(0);
}

/********************数码管显示************************/
    void LED4_Display (char a,char b,char c,char d)
    {
      unsigned char *led_table;          // 查表指针
      unsigned char i;
      //显示第1位
      led_table = LED_0F + a;
      i = *led_table;
      LED_OUT(i);     
      LED_OUT(0x01);    
        digitalWrite(RCLK,LOW);
        digitalWrite(RCLK,HIGH);
      //显示第2位
      led_table = LED_F + b;
      i = *led_table;
      LED_OUT(i);   
      LED_OUT(0x02);    
        digitalWrite(RCLK,LOW);
        digitalWrite(RCLK,HIGH);
      //显示第3位,第3位带小数点
      led_table = LED_0F + c;
      i = *led_table;
      LED_OUT(i);     
      LED_OUT(0x04);  
        digitalWrite(RCLK,LOW);
        digitalWrite(RCLK,HIGH);
      //显示第4位
      led_table = LED_0F + d;
      i = *led_table;
      LED_OUT(i);     
      LED_OUT(0x08);    
        digitalWrite(RCLK,LOW);
        digitalWrite(RCLK,HIGH);
    }
    
    void LED_OUT(unsigned char X)
    {
      unsigned char i;
      for(i=8;i>=1;i--)
      {
        if (X&0x80) 
                {
                  digitalWrite(DIO,HIGH);
                 }  
                else 
                {
                  digitalWrite(DIO,LOW);
                }
        X<<=1;
                digitalWrite(SCLK,LOW);
                digitalWrite(SCLK,HIGH);
      }
    }
