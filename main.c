
#include <16F877A.h>      //khai báo thu vien tiêu chuan cho pic
#include <LCD.C> // THU VIEN CHO LCD
#device ADC=16
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES HS
#use delay(crystal=20M)  // cai dat thach anh 20M hz 
#USE rs232(baud=115200,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)    // -> câu hinh UART (RS232) cho pic16f877a

//=============================NGÕ RA DIEU KHIEN RELAY======================//
#DEFINE TB_1 PIN_A0
#DEFINE TB_2 PIN_A1
#DEFINE TB_3 PIN_A2
#DEFINE TB_4 PIN_A3
//===========================CAU HINH CHO MAN HINH LCD=====================//
#define LCD_ENABLE_PIN PIN_D2
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7

//=========================================================================//
//===================== KHAI CAC BIÊN SU DUNG TRONG CHUONG TRINH===========//
CHAR SMS[80]; // BIEN CHAR Luu tru cac ki tu mà sim gui vê pic16f877a
CHAR c=0X00,c1=0X00,c2=0X00,c3=0X00,c4=0X00; // bien kiem tra la có tin nhan moi  ( neu c='+' c1='C' c2='M' c3='T' <+CMT>: la co tin nhan moi gui ve )
CHAR NEWSMS=0; // co bao tin hieu co tin nhan moi 
char INDEX=0;  // chi so index
CHAR SODIENTHOAI[]="0332456657";  // so dien thoai muon sim gui toi 
CHAR STATUS[]={'0','0','0','0'}; // bien trang cho cac ngõ ra relay

int1 STATUS_1=0; 
int1 STATUS_2=0;
int1 STATUS_3=0;
int1 STATUS_4=0;

VOID _DELAY(CHAR TIME); // ham delay tu tao, de tranh su dung ham delay();
VOID _CAIDAT(VOID);     // khai bao ngat UART cho pic16f877a
VOID _DIEUKHIEN(CHAR TENTHIETBI, INT1 LEVEL); // lenh dieu khien thiet bi
VOID _BAT_ALL(VOID); // bat het thiet bi
VOID _TAT_ALL(VOID); // tat het thiet bi
VOID _RESET(UNSIGNED INT16 TIME,CHAR OPTION); // reset tat ca cac noi dung tin nhan da nhan 
VOID _CAIDATSIM();   // cau hinh tap lenh AT cho sim
VOID _NHANTIN(SIGNED CHAR SODIENTHOAI[],CHAR OPTION); // lenh nhan tin
VOID _GUITINNHAN(SIGNED CHAR SODIENTHOAI[],CHAR CONTENT[]); // lenh gui tin nhan
SIGNED INT8 _DOCSODIENTHOAI(CHAR SODIENTHOAI[]); //lenh nay ko dung
VOID _GHISODIENTHOAI(CHAR SODIENTHOAI[]);//lenh nay ko dung
VOID _SEND_DATA(CHAR CONTENT[]); // gui noi dung tin nhan tu sim7680C -> sim trong dien thoai
void LCD_GMS(char TENTHIETBI,CHAR OPTION);
VOID KTRA1(INT1 STATUS); // check trang thai thiet bi 1 de hien thi len lcd 
VOID KTRA2(INT1 STATUS);
VOID KTRA3(INT1 STATUS);
VOID KTRA4(INT1 STATUS);
VOID LCD_TRANGTHAI(CHAR TENTHIETBI,INT1 STATUS); // khong dung den 
VOID _KIEMTRANUTNHAN(); // nut nhan

//========================================================================================================================================================================
//==============================================Phan hien thi=============================================================================================================
void _MANHINHCHINH()
{
  lcd_gotoxy(1,1);
  lcd_putc("TB1");
  lcd_gotoxy(1,2);
  lcd_putc("OFF");
  
  lcd_gotoxy(5,1);
  lcd_putc("TB2");
  lcd_gotoxy(5,2);
  lcd_putc("OFF");
  
  lcd_gotoxy(9,1);
  lcd_putc("TB3");
  lcd_gotoxy(9,2);
  lcd_putc("OFF");
  
  lcd_gotoxy(13,1);
  lcd_putc("TB4");
  lcd_gotoxy(13,2);
  lcd_putc("OFF");
}   
VOID LCD_TATHET()
{
  lcd_gotoxy(1,2);
  lcd_putc("OFF");
  lcd_gotoxy(5,2);
  lcd_putc("OFF");
  lcd_gotoxy(9,2);
  lcd_putc("OFF");
  lcd_gotoxy(13,2);
  lcd_putc("OFF");
}
VOID LCD_BATHET()
{
lcd_gotoxy(1,2);
  lcd_putc("ON ");
  lcd_gotoxy(5,2);
  lcd_putc("ON ");
  lcd_gotoxy(9,2);
  lcd_putc("ON ");
  lcd_gotoxy(13,2);
  lcd_putc("ON ");
}
//============================================================================================================
//=====================================Ngat UART==============================================================
#int_RDA
VOID NGAT_isr(void)
{

  c=getc(); // ki tu nhan duoc thong qua giao tiep uart (getc là ki tu nhan duoc)
  IF(c=='+') c1=c;
  IF(c=='C') c2=c;
  IF(c=='M') c3=c;
  IF(c=='T') c4=c;
  
  IF((c1=='+') && (c2=='C') &&(c3=='M')&&(c4=='T')) // nhan duoc PIC nhan DUOC chuoi "+CMT" la thong bao co tin nhan den 
  {       
          NEWSMS=1; // muc bao cho co bao tin nhan 
          
  }
  IF(c!='\0' && NEWSMS==1)
  {
  SMS[INDEX++] = c; // noi dung tin nhan se duoc gan bao mang SMS
  }
  IF(INDEX>=80)
  {
  INDEX=0;   // gioi han noi dung la 80 ki tu thoi 
  }
}
//============================================================================================================
CHAR CONTENT[]=" DA SAN SANG , HAY GUI LENH TOI ";// khi sim va pic ket noi duoc voi nhau se gui noi dung nay toi dien thoai cua ban 
//===========================================================================================================
void main()
{ 
  SET_TRIS_A(0X00); // cau hinh ngo ra cho relay
  OUTPUT_A(0X00);   // trang thai muc thap
  //================khai bao ngat================
  ENABLE_INTERRUPTS(INT_RDA); 
  ENABLE_INTERRUPTS(GLOBAL);
  //============================================
   char J;
   char TEMP;
   lcd_init(); // khai bao lcd
   lcd_putc('\f'); // xoa man hinh lcd
//!   CHAR CONTENT[]=" DA SAN SANG , HAY GUI LENH TOI ";
 //============================================================
 //===khoi dong dong chuong trinh==============================
   _CAIDATSIM(); 
   _GUITINNHAN(SODIENTHOAI,CONTENT);
   _TAT_ALL();
    lcd_putc('\f');
  _MANHINHCHINH();
  //===========================================================
   while(TRUE)
   {   
      
      IF(NEWSMS==1)   // neu nhan duoc tin nhan tu dien thoai gui toi pic16f thì NEWSMS=1 khong thi =0;
      {
       _DELAY(10);   // delay de nhan duoc het noi dung
       _RESET(1,2);  // xoa co báo newwsms = 0 ;
       
                   
         FOR(J=0;J<80;J++)
           {
         //----------------------------------------------------------------------//
         //----------------- BAT TAT TOAN BO THIET BI ---------------------------//
         //----------------------------------------------------------------------//
          IF((SMS[J]=='T'||SMS[J]=='t') && (SMS[J+1]=='A'||SMS[J+1]=='a') && 
          (SMS[J+2]=='T'||SMS[J+2]=='t') && (SMS[J+3]==' ') && (SMS[J+4]=='H'||SMS[J+4]=='h') &&
          (SMS[J+5]=='E'||SMS[J+5]=='e') && (SMS[J+6]=='T'||SMS[J+6]=='t'))
          { 
            LCD_TATHET();
            _TAT_ALL(); // tet het thiet bi relay 
            
            SPRINTF(CONTENT,"DA TAT HET THIET BI"); 
            _GUITINNHAN(SODIENTHOAI,CONTENT); // gui tin nhan tren 
            //LCD
            BREAK;
          }
          
          IF((SMS[J]=='B'||SMS[J]=='b') && (SMS[J+1]=='A'||SMS[J+1]=='a') && 
          (SMS[J+2]=='T'||SMS[J+2]=='t') && (SMS[J+3]==' ') && (SMS[J+4]=='H'||SMS[J+4]=='h') &&
          (SMS[J+5]=='E'||SMS[J+5]=='e') && (SMS[J+6]=='T'||SMS[J+6]=='t'))
          { 
            LCD_BATHET();
            _BAT_ALL();
            SPRINTF(CONTENT,"DA BAT HET THIET BI");
            _GUITINNHAN(SODIENTHOAI,CONTENT);

            BREAK;
          }
          
          //-----------------------------------------------------------------//
          //-------------------- DIEU KHIEN THIET BI SO : X -----------------//
          //-----------------------------------------------------------------//
          
          IF((SMS[J]=='T'||SMS[J]=='t') && (SMS[J+1]=='A'||SMS[J+1]=='a') && 
          (SMS[J+2]=='T'||SMS[J+2]=='t') && (SMS[J+3]==' ') && (SMS[J+4]=='t'||SMS[J+4]=='T') &&
          (SMS[J+5]=='B'||SMS[J+5]=='b'))
           { 
             TEMP=SMS[J+6]; // temp la ten thiet bi 1,2,3,4 (neu dt gui tin nhan là "BAT TB1" thì bât tb1 ) sô 1 là biên tmp gia tri duoc gan 
             _DIEUKHIEN(TEMP,0); // diue khien thiet bi relay 1 (len muc 1)
             LCD_GMS(TEMP,0); // hien thi trang thai "ON" tren LCD
             SPRINTF(CONTENT,"DA TAT THIET BI %c",TEMP); 
             _GUITINNHAN(SODIENTHOAI,CONTENT); // dong thoi gui tin nhan 
             //LCD
             BREAK;
           }
          IF((SMS[J]=='B'||SMS[J]=='b') && (SMS[J+1]=='A'||SMS[J+1]=='a') && 
          (SMS[J+2]=='T'||SMS[J+2]=='t') && (SMS[J+3]==' ') && (SMS[J+4]=='t'||SMS[J+4]=='T') &&
          (SMS[J+5]=='B'||SMS[J+5]=='b'))
           { 
             TEMP=SMS[J+6]; 
             _DIEUKHIEN(TEMP,1);
              LCD_GMS(TEMP,1);
             SPRINTF(CONTENT," DA BAT THIET BI %c",TEMP);
             _GUITINNHAN(SODIENTHOAI,CONTENT);
             BREAK;
           }
         }
         //============================================================================================//
         //===================RESET NOI DUNG TIN NHAN DE NHAN TIN NHAN MOI============================//
         _DELAY(10);
         _RESET(10,1);
         
      }
      ELSE
      {
      _KIEMTRANUTNHAN(); // LUON KIEM TRA NUT nhan
      }
   }
}
//---------------------------------------------------------//
//-----------   CHUONG TRINH CHO HAM CON    ---------------// 
//---------------------------------------------------------//
VOID _CAIDAT(VOID)
{
  ENABLE_INTERRUPTS(INT_RDA);
  ENABLE_INTERRUPTS(GLOBAL);
}
VOID _DIEUKHIEN(CHAR TENTHIETBI, INT1 LEVEL)
{
 SWITCH(TENTHIETBI)
 {
  CASE '1' :
    {
     OUTPUT_BIT(TB_1, LEVEL); 
     STATUS_1=LEVEL;
     IF(LEVEL==0)
     {
      STATUS[0]='0';
     }
     ELSE STATUS[0]='#';
     BREAK;
    }
    
  CASE '2' :
    {
     OUTPUT_BIT(TB_2,LEVEL);
     STATUS_2=LEVEL;
     IF(LEVEL==0)
     {
      STATUS[1]='0';
     }
     ELSE STATUS[1]='#';
     BREAK;
    }
   CASE '3' :
    {
     OUTPUT_BIT(TB_3,LEVEL);
     STATUS_3=LEVEL;
     IF(LEVEL==0)
     {
      STATUS[2]='0';
     }
     ELSE STATUS[2]='#';
     BREAK;
    } 
    CASE '4' :
    {
     OUTPUT_BIT(TB_4,LEVEL);
     STATUS_4=LEVEL;
     IF(LEVEL==0)
     {
      STATUS[3]='0';
     }
     ELSE STATUS[3]='#';
     BREAK;
    }
 }
}
VOID _BAT_ALL(VOID)
{
  CHAR ARRAY[]={'1','2','3','4'};
  FOR(INT I=0; I<=3;i++)
  {
    _DIEUKHIEN(ARRAY[I],1);
    STATUS[I]='#';
  }
}

VOID _TAT_ALL(VOID)
{
  CHAR ARRAY[]={'1','2','3','4'};
  FOR(INT I=0; I<=3;i++)
  {
    _DIEUKHIEN(ARRAY[I],0);
    STATUS[I]='0';
  }
}

VOID _RESET(UNSIGNED INT16 TIME, CHAR OPTION)
{
 UNSIGNED INT16 K,N; // so lan 

 SWITCH(OPTION)
 {
 //============================= XOA het tat ca noi dung tin nhan mà module sim nhan duoc====================//
  CASE 1:
  {
   FOR(N=0; N<TIME;N++)
   {
    FOR(K=0;K<=80;K++)
    {
    SMS[K]='\0';
    }
    
    
    c1=c2=c3=c4='\0';
    NEWSMS=0;
    INDEX=0;
    
    _DELAY(1);
   }
   BREAK;
  }
  //=================== chi reset NEWSMS thoi====================//
  CASE 2 :
  {
  FOR(N=0;N<TIME;N++)
  {
    
     c1=c2=c3=c4='\0';
     NEWSMS=0;
    INDEX=0;
    _DELAY(1);
  }
  }
 }
}
// thay the cho lenh delay_MS() , cho delay 100MS
VOID _DELAY(int8 time) 
{
  INT8 I;
 FOR( I=0 ; I < time ; I++)
 {
   DELAY_MS(100);
 }
}

//=============================================================================================================//
//================================= Cau hinh tap lenh AT module SIM ===========================================//
VOID _CAIDATSIM()
{ 
 LCD_PUTC('\F');
 lcd_gotoxy(1,1);
 lcd_putc("  DO AN MON VDK  ");
 lcd_gotoxy(1,2);
 lcd_putc("DANG KHOI DONG..");
 FOR(CHAR I=0; I<16;I++)
  PRINTF("AT\r\n");
 _DELAY(5);
 
 PRINTF("AT+CPIN?\r\n");
 _DELAY(5);
 
  PRINTF("ATZ\r\n");
 _DELAY(5);
 
 PRINTF("ATE1\r\n");
 _DELAY(5);
 
 PRINTF("ATE+ICF=3,3\r\n");
 _DELAY(5);
 
  PRINTF("AT+IPR=115200\r\n");
 _DELAY(5);
 
  PRINTF("AT&W\r\n");
 _DELAY(5);
 
 PRINTF("AT+CMGF=1\r\n");
 _DELAY(5);
 
 PRINTF("AT+CNMI=2,2,0,0,0\r\n");
 _DELAY(5);
 
  PRINTF("AT+CSAS\r\n");
 _DELAY(5);
 
 PRINTF("AT+CMGD=1,4\r\n");
 _DELAY(5);
}
//===========================================================================================================//
// lenh gui noi dung tin nhan tu SIM -> dien thoai 
VOID _GUITINNHAN(SIGNED CHAR SODIENTHOAI[],CHAR CONTENT[])
{
  PRINTF("AT+CMGS=\"%c%c%c%c%c%c%c%c%c%c\"\r\n",SODIENTHOAI[0],
  SODIENTHOAI[1],SODIENTHOAI[2],SODIENTHOAI[3],SODIENTHOAI[4],
  SODIENTHOAI[5],SODIENTHOAI[6],SODIENTHOAI[7],SODIENTHOAI[8],
  SODIENTHOAI[9]);
  _DELAY(5);
  _SEND_DATA(CONTENT);
  _DELAY(5);
  PUTC(26);
  _DELAY(5);
}
// gui tung ki tuw tù noi dung tin nhan (vd:"xinchao" thi lenh nay se gui 'x'->'i'->'n'->'c'->'h'->'a'->'o'")
VOID _SEND_DATA(CHAR CONTENT[])
{
  UNSIGNED CHAR I=0;
  WHILE(TRUE)
  {
   IF(CONTENT[I]=='#' || CONTENT[I]=='\0' ||I>=80)
   {
    BREAK;
   }
   PUTC(CONTENT[I]);
   I++;
  }
}
//==================================lenh nay ko dung================================
VOID LCD_TRANGTHAI(CHAR TENTHIETBI,INT1 STATUS)
{
 IF(TENTHIETBI=='1')
 {
  IF(STATUS==1)
   {
    lcd_gotoxy(1,2);
    lcd_putc("ON ");
   }
   else 
   {
    lcd_gotoxy(1,2);
    lcd_putc("OFF");
   }
 }
 //KTRA TB2
 IF(TENTHIETBI=='2')
 {
  IF(STATUS==1)
   {
    lcd_gotoxy(5,2);
    lcd_putc("ON ");
   }
   else 
   {
    lcd_gotoxy(5,2);
    lcd_putc("OFF");
   }
 }
 //KTRA TB3
 IF(TENTHIETBI=='3')
 {
  IF(STATUS==1)
   {
    lcd_gotoxy(9,2);
    lcd_putc("ON ");
   }
   else 
   {
    lcd_gotoxy(9,2);
    lcd_putc("OFF");
   }
 }
 //KTRA TB4
 IF(TENTHIETBI=='4')
 {
  IF(STATUS==1)
   {
    lcd_gotoxy(13,2);
    lcd_putc("ON ");
   }
   else 
   {
    lcd_gotoxy(13,2);
    lcd_putc("OFF");
   }
 }
}   
//==================================================================================
VOID KTRA1(INT1 STATUS){
   
   IF(STATUS==1)
   {
    _DIEUKHIEN('1',1);
     sprintf(CONTENT,"BAT THIET BI 1");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
   ELSE 
   {
    _DIEUKHIEN('1',0);
    sprintf(CONTENT,"TAT THIET BI 1");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
    BREAK;
   }
}
VOID KTRA2(INT1 STATUS)
{
   
   IF(STATUS==1)
   {
     _DIEUKHIEN('2',1);
     sprintf(CONTENT,"BAT THIET BI 2");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
   ELSE 
   {
    _DIEUKHIEN('2',0);
     sprintf(CONTENT,"TAT THIET BI 2");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
}
VOID KTRA3(INT1 STATUS){
   
   IF(STATUS==1)
   {
    _DIEUKHIEN('3',1);
     sprintf(CONTENT," BAT THIET BI 3");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
   ELSE 
   {
    _DIEUKHIEN('3',0);
     sprintf(CONTENT," TAT THIET BI 3");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
}
VOID KTRA4(INT1 STATUS){
   
   IF(STATUS==1)
   {
   _DIEUKHIEN('4',1);
     sprintf(CONTENT,"BAT THIET BI 4");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
   ELSE 
   {
     _DIEUKHIEN('4',0);
     sprintf(CONTENT,"TAT THIET BI 4");
    _GUITINNHAN(SODIENTHOAI,CONTENT);
   }
}
//========================================================================================================================================//
//===================================== hien thi trang thai thiet bi len LCD khi gui qua tin nhan (dieu khien)============================//
void LCD_GMS(char TENTHIETBI,CHAR OPTION)
{
  IF(OPTION==1)
  {
   IF(TENTHIETBI=='1')
   {
    LCD_GOTOXY(1,2);
    LCD_PUTC("ON ");
   }
   
    IF(TENTHIETBI=='2')
   {
    LCD_GOTOXY(5,2);
    LCD_PUTC("ON ");
   }
    IF(TENTHIETBI=='3')
   {
    LCD_GOTOXY(9,2);
    LCD_PUTC("ON ");
   }
    IF(TENTHIETBI=='4')
   {
    LCD_GOTOXY(13,2);
    LCD_PUTC("ON ");
   }
  }
  IF(OPTION==0)
  {
   IF(TENTHIETBI=='1')
   {
    LCD_GOTOXY(1,2);
    LCD_PUTC("OFF");
   }
   
    IF(TENTHIETBI=='2')
   {
    LCD_GOTOXY(5,2);
    LCD_PUTC("OFF");
   }
    IF(TENTHIETBI=='3')
   {
    LCD_GOTOXY(9,2);
    LCD_PUTC("OFF");
   }
    IF(TENTHIETBI=='4')
   {
    LCD_GOTOXY(13,2);
    LCD_PUTC("OFF");
   }
  }
}
//=============================================================================================
VOID _KIEMTRANUTNHAN()
{ 
//NUT NHAN DIEU KHIEN TB1
  IF(INPUT(PIN_B0)==0)
  {
    DELAY_MS(2);
    IF(INPUT(PIN_B0)==0)
    {
     STATUS_1=~STATUS_1;
     LCD_TRANGTHAI('1',STATUS_1);
     KTRA1(STATUS_1);
     WHILE(INPUT(PIN_B0)==0);
    }
  }
 //NUT NHAN DIEU KHIEN TB2
   IF(INPUT(PIN_B1)==0)
  {
    DELAY_MS(3);
    IF(INPUT(PIN_B1)==0)
    {
     STATUS_2=~STATUS_2;
     LCD_TRANGTHAI('2',STATUS_2);
     KTRA2(STATUS_2);
     WHILE(INPUT(PIN_B1)==0);
    }
  }
  //NUT NHAN DIEU KHIEN TB3
   IF(INPUT(PIN_B2)==0)
  {
    DELAY_MS(2);
    IF(INPUT(PIN_B2)==0)
    {
     STATUS_3=~STATUS_3;
     LCD_TRANGTHAI('3',STATUS_3);
     KTRA3(STATUS_3);
     WHILE(INPUT(PIN_B2)==0);
    }
  }
    //NUT NHAN DIEU KHIEN TB4
   IF(INPUT(PIN_B3)==0)
  {
    DELAY_MS(2);
    IF(INPUT(PIN_B3)==0)
    {
     STATUS_4=~STATUS_4;
     LCD_TRANGTHAI('4',STATUS_4);
     KTRA4(STATUS_4);
   
     WHILE(INPUT(PIN_B3)==0);
    }
  }
  
}
