int error,stopBit,Pro,Derivative,PIDvalue,i,j,uart_rd;
int Integral=0;
int Kp=8;
int Ki=0;
int Kd=2;
int previousError=0;
int flag=0;
int Mflag=0;
int manualMode=0;
int connectionError=0;
int downOkey=0;
int LiftFlag=0;
void calculatePID(),RX_From_Mega(),rightTurn(),leftTurn(),stopMotor();
void immeLeft(),immeRight(),manual(),forwardMotor(),backwardMotor(),slowStop();

void main() {
ANSEL=ANSELH=TRISC=TRISD=TRISE=0x00;
TRISB=0xFF;
UART1_Init(9600);
GIE_bit=1;
RBIF_bit=0;
RBIE_bit=1;
WPUB=0x3F;
OPTION_REG.F7=0;
IOCB=0b00111111; // Interrupt enable pins on RB pins
CCP1CON = 0x0C; //pwm mode is activated for ccp1.
CCP2CON = 0x0C; //pwm mode is activated for ccp2.
PR2 = 240; //period of the pwm

T2CON=0x04; //timer2 is on

while(1){
RX_From_Mega();

 if(manualMode==0){
  Mflag=0;
  RE0_bit=0;
  if(flag==1 ){
  RD4_bit=1;
  stopMotor();
  }
  else if(flag==0){
  RD4_bit=0;
  calculatePID();
  forwardMotor();
  }
  }
 else if(manualMode==1 && connectionError==0){
 flag=0;
 RE0_bit=1;
 manual();
 }
}
}

 void calculatePID(){
 Pro = error;
 Integral = Integral + error;
 Derivative = error-previousError;
 PIDvalue = (Kp*Pro) + (Ki*Integral) + (Kd*Derivative);
 previousError = error;
 CCPR1L=180+PIDvalue;  // when the way turning right leftMotorSpeed ll increase,RightMotorSpeed ll decrease
 CCPR2L=180-PIDvalue;  // when the way turning left RightMotorSpeed ll increase,leftMotorSpeed ll decrease
   if(CCPR1L>=245 || CCPR1L<=120 || CCPR2L>=245 || CCPR2L<=120){
   CCPR1L =180;
   CCPR2L =180;
          }
   }
void RX_From_Mega(){
if(UART1_Data_Ready()){
  char uart_rd = UART1_Read();
  connectionError=0;
  if(uart_rd=='0'){
  flag=0;  }
  else if(uart_rd=='1'){
  flag=1;  }
  else if(uart_rd=='w'){
         Mflag=1; }
  else if(uart_rd=='s'){
         Mflag=2; }
  else if(uart_rd=='a'){
         Mflag=3; }
  else if(uart_rd=='d'){
         Mflag=4; }
  else if(uart_rd=='q'){
         Mflag=5; }
  else if(uart_rd=='e'){
         Mflag=6; }
  else if(uart_rd=='z'){
         Mflag=7; }
  else if(uart_rd=='m'){
  manualMode=1;   }
  else if(uart_rd=='o'){
  manualMode=0;   }
  else if(uart_rd=='l'){
       LiftFlag=1;}
  else if(uart_rd=='r'){
       LiftFlag=0;}
 }
 else
 connectionError=1;
}
void interrupt(){
RBIF_bit=0;
 if(RB0_bit==0 && RB1_bit==0 && RB2_bit==1 && RB3_bit==0 && RB4_bit==0){
 error=0;
 } //straight line following
  else if(RB0_bit==0 && RB1_bit==0 && RB2_bit==1 && RB3_bit==1 && RB4_bit==0){
  error=1;
  } //the way taking right
   else if(RB0_bit==0 && RB1_bit==0 && RB2_bit==0 && RB3_bit==1 && RB4_bit==0){
   error=2;
   } //the way taking right faster than previous
    else if(RB0_bit==0 && RB1_bit==0 && RB2_bit==0 && RB3_bit==1 && RB4_bit==1){
    error=3;
    } //the way taking right faster than previous
     else if(RB0_bit==0 && RB1_bit==0 && RB2_bit==0 && RB3_bit==0 && RB4_bit==1){
     error=4;
     } //the way taking right faster than previous
      else if(RB0_bit==0 && RB1_bit==1 && RB2_bit==1 && RB3_bit==0 && RB4_bit==0){
      error=-1;
      } //the way taking right
       else if(RB0_bit==0 && RB1_bit==1 && RB2_bit==0 && RB3_bit==0 && RB4_bit==0){
       error=-2;
       } //the way taking right faster than previous
        else if(RB0_bit==1 && RB1_bit==1 && RB2_bit==0 && RB3_bit==0 && RB4_bit==0){
        error=-3;
        } //the way taking right faster than previous
         else if(RB0_bit==1 && RB1_bit==0 && RB2_bit==0 && RB3_bit==0 && RB4_bit==0){
         error=-4;
         } //the way taking right faster than previous
          else if(RB0_bit==1 && RB1_bit==1 && RB2_bit==1 && RB3_bit==1 && RB4_bit==1){
          stopBit=1;
          }//all black line so stop
}

void forwardMotor(){
RD0_bit=1;
RD1_bit=0;
RD2_bit=1;
RD3_bit=0;}
void backwardMotor(){
RD0_bit=0;
RD1_bit=1;
RD2_bit=0;
RD3_bit=1;}
void stopMotor(){
RD0_bit=0;
RD1_bit=0;
RD2_bit=0;
RD3_bit=0;}
void leftTurn(){
RD0_bit=1;
RD1_bit=0;
RD2_bit=0;
RD3_bit=0;}
void rightTurn(){
RD0_bit=0;
RD1_bit=0;
RD2_bit=1;
RD3_bit=0;}
void immeLeft(){
RD0_bit=1;
RD1_bit=0;
RD2_bit=0;
RD3_bit=1;}
void immeRight(){
RD0_bit=0;
RD1_bit=1;
RD2_bit=1;
RD3_bit=0;}

void manual(){
if(Mflag==1){
RD5_bit=0;
RD6_bit=1;
RD7_bit=0;
forwardMotor();}
else if(Mflag==2){
RD5_bit=1;
RD6_bit=0;
RD7_bit=1;
backwardMotor();}
else if(Mflag==3){
RD5_bit=1;
RD6_bit=0;
RD7_bit=0;
leftTurn();}
else if(Mflag==4){
RD5_bit=0;
RD6_bit=0;
RD7_bit=1;
rightTurn();}
else if(Mflag==5){
RD5_bit=1;
RD6_bit=1;
RD7_bit=0;
immeLeft();}
else if(Mflag==6){
RD5_bit=0;
RD6_bit=1;
RD7_bit=1;
immeRight();}
else if(Mflag==7){
RD5_bit=1;
RD6_bit=1;
RD7_bit=1;
stopMotor();}
}