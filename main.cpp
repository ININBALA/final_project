#include "mbed.h"
#include "bbcar.h"
 
 InterruptIn sw2(SW2);
 DigitalOut led1(LED1);
 PwmOut pin9(D9), pin8(D8);
 DigitalInOut pin10(D10);
 Ticker servo_ticker;
 Serial pc(USBTX, USBRX);
 Serial uart(D1,D0); //tx,rx0
 BBCar car(pin8, pin9, servo_ticker);
 EventQueue queue1;
 Thread t1;

 void goStraight(){
     car.goStraightCalib(40);
 }

 int main() {
     uart.baud(9600);
     parallax_ping  ping1(pin10);
     double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
     double speed_table0[] = {-16.109, -15.790, -14.674, -12.202, -6.220, 0.000, 6.699, 12.202, 14.913, 16.030, 16.508};
     double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
     double speed_table1[] = {-16.508, -16.189, -15.152, -12.202, -6.141, 0.000, 4.785, 11.245, 14.913, 16.189, 16.668};
     car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
     t1.start(callback(&queue1, &EventQueue::dispatch_forever));
     sw2.rise(queue1.event(goStraight));
     int wall = 0;
     while(1){
        if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
        }
        //pc.printf("%f\r\n", (float)ping1);
         if((float)ping1 < 20 && wall == 0){
             wall = 1;
             led1 = 0;
             car.turnCalibLeft(30, -30);
             wait(0.527);
             car.stop();
             wait(0.2);
             queue1.call(goStraight);
             wait(4.5);
         }
         else if((float)ping1 < 5 && wall == 1){
             wall = 1;
             led1 = 0;
             car.turnCalibRight(40, -40);
             wait(0.7);
             car.stop();
             car.goStraightCalib(-20);
             wait(1.5);
             car.stop();
             break;
         }
         wait(.1);
     }

 }