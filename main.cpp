#include "mbed.h"

#include "bbcar.h"


 DigitalOut led1(LED1);

 PwmOut pin9(D9), pin8(D8);

 DigitalInOut pin10(D10);

 Ticker servo_ticker;

 Serial pc(USBTX, USBRX);

 BBCar car(pin8, pin9, servo_ticker);


 int main() {

     parallax_ping  ping1(pin10);

     double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};

     double speed_table0[] = {-16.428, -15.950, -14.754, -11.324, -4.785, 0.000, 6.699, 12.202, 14.913, 16.030, 16.508};

     double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};

     double speed_table1[] = {-16.428, -15.950, -14.754, -11.803, -5.582, 0.000, 4.306, 10.048, 12.042, 12.999, 13.159};

     car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
     car.goStraightCalib(30);

     while(1){
        pc.printf("%f\r\n", (float)ping1);
         if((float)ping1>20) led1 = 1;

         else{

             led1 = 0;

             car.turnCalibLeft(30, -30);
             wait(0.525);
             car.stop();
             break;

         }

         wait(.01);

     }

 }