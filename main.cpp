#include "mbed.h"
#include "bbcar.h"
 //#include "mbed_rpc.h"
 InterruptIn sw2(SW2);
 DigitalOut led1(LED1);
 PwmOut pin9(D9), pin8(D8);
 DigitalIn pin3(D13);
 DigitalInOut pin10(D10);
 Ticker servo_ticker;
 Ticker encoder_ticker;
 RawSerial xbee(D12, D11);
 RawSerial pc(USBTX, USBRX);
 Serial uart(D1,D0); //tx,rx0
 BBCar car(pin8, pin9, servo_ticker);
 EventQueue queue1, queue2;
 Thread t1, t2;
 int pingStart = 0;

 void xbee_rx_interrupt(void);

void xbee_rx(void);

void reply_messange(char *xbee_reply, char *messange);

void check_addr(char *xbee_reply, char *messenger);
/*void getAction(Arguments *in, Reply *out);
RPCFunction rpcAction(&getAction, "getAction");*/
parallax_ping  ping1(pin10);
parallax_encoder encoder0(pin3, encoder_ticker);
char image;
int  status = 0;
int idc;
void getAction();
 void goStraight(){
     car.goStraightCalib(40);
     pingStart = 1;
     led1=1;
     //status = 1;
     queue2.call(getAction);
 }
void go(){
  idc = queue1.call(goStraight);
}
void left(){
  encoder0.reset();
  car.turnCalibLeft(30, -30);
  while(encoder0.get_cm()<6) {
                  //pc.printf("%f\n", encoder0.get_cm());
    wait_ms(5);
  }
  car.stop();
  wait(1);
}
void right(){
  encoder0.reset();
  car.turnCalibRight(30, -30);
  while(encoder0.get_cm()<6) wait_ms(5);
               // wait(0.7);
  car.stop();
  wait(1);
}
 int main() {
     uart.baud(9600);
     pc.baud(9600);
     double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
     double speed_table0[] = {-16.348, -15.711, -14.833, -12.281, -6.460, 0.000, 5.104, 11.165, 14.435, 15.711, 16.269};
     double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
     double speed_table1[] = {-16.349, -16.109, -15.073, -12.202, -6.141, 0.000, 4.546, 11.324, 14.754, 16.030, 16.588};
     car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
     t1.start(callback(&queue1, &EventQueue::dispatch_forever));
     t2.start(callback(&queue2, &EventQueue::dispatch_forever));
     char xbee_reply[4];


    // XBee setting

    xbee.baud(9600);

    xbee.printf("+++");

    xbee_reply[0] = xbee.getc();

    xbee_reply[1] = xbee.getc();

    if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){

        pc.printf("enter AT mode.\r\n");

        xbee_reply[0] = '\0';

        xbee_reply[1] = '\0';

    }


    xbee.printf("ATMY 0x240\r\n");

    reply_messange(xbee_reply, "setting MY : 0x240");


    xbee.printf("ATDL 0x140\r\n");

    reply_messange(xbee_reply, "setting DL : 0x140");


    xbee.printf("ATID 0x1\r\n");

    reply_messange(xbee_reply, "setting PAN ID : 0x1");


    xbee.printf("ATWR\r\n");

    reply_messange(xbee_reply, "write config");


    xbee.printf("ATMY\r\n");

    check_addr(xbee_reply, "MY");


    xbee.printf("ATDL\r\n");

    check_addr(xbee_reply, "DL");


    xbee.printf("ATCN\r\n");

    reply_messange(xbee_reply, "exit AT mode");

    xbee.getc();


    // start

    //pc.printf("start\r\n");

    // Setup a serial interrupt function to receive data from xbee
     xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
     sw2.rise(queue1.event(goStraight));
     //sw2.rise(queue2.event(getAction));
     int wall = 0;
     status  = 1;
     while(1){
         if(pingStart == 1){
            if(uart.readable()){
                char recv = uart.getc();
                pc.putc(recv);
            }
            if((float)ping1 < 20 && wall == 0){
               //queue1.cancel(idc);
                wall = 1;
                led1 = 0;
                //xbee.printf("turn left");
                status = 2;
                left();
                encoder0.reset(); 
                queue1.call(goStraight);
                while(encoder0.get_cm()<85) wait_ms(50);
                //car.stop();
                //wait(3);
            }
            else if((float)ping1 < 10 && wall == 1){
                wall = 2;
                led1 = 0;
                status=3;
                right();
                char s[21];
                sprintf(s,"image_classification");
                uart.puts(s);
                int i = 0;
                while(i < 5000){
                  if(uart.readable()){
                    char recv = uart.getc();
                    image = recv;
                    pc.putc(recv);
                    pc.printf("\r\n");
                  }
                  i++;
                  wait_ms(1);
                }
                status = 4;
                wait(1);
                right();
                car.goStraightCalib(20);
                wait(3);
                car.stop();
                wait(1);
                car.stop();
                left();
                car.goStraightCalib(-20);
                wait(1.5);
                car.stop();
                wait(2);
                status=5;
                car.goStraightCalib(20);
                wait(1.5);
                right();
                wait(1);
                car.goStraightCalib(20);
                wait(1);
                car.stop();
                right();
                car.goStraightCalib(20);
                wall = 2;
            }
            else if((float)ping1 < 10 && wall == 2){
              car.stop();
              right();
              car.goStraightCalib(20);
              wall=3;
            }
            else if((float)ping1 < 10 && wall == 3){
              car.stop();
              right();
              car.goStraightCalib(20);
              status=6;
              break;
            }
         }
         wait(.1);
     }
  }
 void xbee_rx_interrupt(void)

{

  xbee.attach(NULL, Serial::RxIrq); // detach interrupt

  queue1.call(&xbee_rx);

}


void xbee_rx(void)

{

  static int i = 0;

  static char buf[100] = {0};

  while(xbee.readable()){

    char c = xbee.getc();

    if(c!='\r' && c!='\n'){

      buf[i] = c;

      i++;

      buf[i] = '\0';

    }else{

      i = 0;

      pc.printf("Get: %s\r\n", buf);

      xbee.printf("%s", buf);

    }

  }

  wait(0.1);

  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt

}


void reply_messange(char *xbee_reply, char *messange){

  xbee_reply[0] = xbee.getc();

  xbee_reply[1] = xbee.getc();

  xbee_reply[2] = xbee.getc();

  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){

   pc.printf("%s\r\n", messange);

   xbee_reply[0] = '\0';

   xbee_reply[1] = '\0';

   xbee_reply[2] = '\0';

  }

}


void check_addr(char *xbee_reply, char *messenger){

  xbee_reply[0] = xbee.getc();

  xbee_reply[1] = xbee.getc();

  xbee_reply[2] = xbee.getc();

  xbee_reply[3] = xbee.getc();

  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);

  xbee_reply[0] = '\0';

  xbee_reply[1] = '\0';

  xbee_reply[2] = '\0';

  xbee_reply[3] = '\0';

}

void getAction() {
  while(true){
    switch(status){
      case 1:
        xbee.printf("go straight\n");
        break;
      case 2:
        xbee.printf("turn left\n");
        break;
      case 3:
        xbee.printf("parking\n");
        break;
      case 4:
        xbee.printf("number is %c\n", image);
      case 5:
        xbee.printf("leave mission1\n");
      case 6:
        xbee.printf("leave maze\n");
      default:
        break;
    }
    wait(1);
  }
}