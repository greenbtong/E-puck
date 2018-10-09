#include <p30F6014A.h>
#include <motor_led/e_epuck_ports.h>
#include <motor_led/e_init_port.h>
#include <a_d/advance_ad_scan/e_prox.h>
#include <a_d/advance_ad_scan/e_ad_conv.h>
#include <uart/e_uart_char.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


// functions
void Wait(long);
int GetSelector(void);
void Movement(int, int);
void LED_ON(unsigned int);
void LED_OFF(unsigned int);
void BODY_ON();
void BODY_OFF();

// constants
char Buffer[100];
int Buff_Len;
unsigned int Off = 0; // dont need
unsigned int On = 1; // dont need
int SpeedF = 1000;
int SpeedR = -1000;
int SpeedT = 1000; 
int Stop = 0;
int Sec = 1000000;
int Close = 1000;
int S[] = {0,0,0,0,0,0,0,0};
int amount = 200;

int main()
{
    // Initialisation. 
    e_init_port(); // Initialises the ports.
    e_init_ad_scan(ALL_ADC); // Initialises the analogue to digital converters.
    e_led_clear(); // Turn off all the LEDs in the initialisation stage.
    e_start_agendas_processing(); // Allows the robot to execute multiple tasks.
    e_init_uart1(); // Initialises the UART. Uncomment when you use Bluetooth communication.
    e_calibrate_ir(); // Uncomment when you use Proximity Sensors.
    e_init_motors(); // Initialises the motors. Uncomment when you use the motors.
    
    e_led_clear(); // clear ring LED
    e_set_front_led(0); // Turn the front LED off
    e_set_body_led(0); // Turn the body LED off
    
   
    int i, j;
    int r1 = 0; // random variable
    int r2 = 0; // random variable
    srand((unsigned)time(NULL));
    int timer = 0;
 
    // Main Loop.
    while(1)
    {
        //e_set_front_led(1); // Turn the front LED on
        //e_set_body_led(1); // Turn the body LED on
        BODY_OFF();
        
        // Send information to bluetooth 
        // Buff_Len = sprintf(Buffer, "Selector: %d\r\n", GetSelector());
        // e_send_uart1_char(Buffer, Buff_Len);
        
        // LED and determining which sensor is close
        for(i = 0; i < 8; i++){
            if (e_get_prox(i) > Close){
                S[i] = 1;
                if(i <= 3){
                    LED_ON(i);
                } else {
                    if (i == 7){
                        LED_ON(0);
                    } else {
                        LED_ON(i+1);
                    }
                }
            } else {
                S[i] = 0;
                if(i <= 3){
                    LED_OFF(i);
                } else {
                    if (i == 7){
                        LED_OFF(0);
                    } else {
                        LED_OFF(i+1);
                    }
                }
            }
        }
        
        // back light
        if (S[3] == 1 && S[4] ==1){
            LED_ON(4);
        } else {
            LED_OFF(4);
        }
        
        // movements
        if (timer == amount){
            r1 = e_get_prox(4) % 10;
            r2 = e_get_prox(3) % 100;
//            r1 = rand() % 2; // determine if c or cc
//            r2 = rand() % 2; // determine how long to turn
            if (r1 > 5 && S[2] == 0 && S[1] == 0){            
                BODY_ON();
                if (r2 > 50){                    
                    timer = 0;
                } else {
                    timer = amount;
                }
                for(j = 0; i <Sec/2 ;i++){
                    Movement(SpeedT,-SpeedT); // turn c 
                }
            } else if(r1 <=5 && S[1] == 0 S[2] == 0) {                            
                BODY_ON();
                if (r2 > 50){
                    timer = 0;
                } else {
                    timer = amount;
                }
                for(j = 0; i <Sec/2 ;i++){
                    Movement(-SpeedT,SpeedT); // turn cc  
                }
            } else {
                timer = 0;
            }
        } else{
            if (S[0] == 1 && S[1] == 1 && S[2] == 1 && S[3] == 1 && S[4] == 1 && S[5] == 1 && S[6] == 1 && S[7] == 1){
                Movement(Stop, Stop);
            } else if (S[0] == 1 || S[7] == 1 || S[1] == 1 || S[6] == 1){
                BODY_ON();
                if (S[0] == 1 && S[7] == 1 && S[1] == 1 && S[6] == 1 && S[4] == 0 && S[3] == 0){
                    Movement(SpeedR, SpeedR);
                    timer = amount;
                } else if (S[2] == 1){
                    Movement(-SpeedT,SpeedT); // turn cc
                    timer = 0;
                } else if (S[1] == 1){
                    Movement(-SpeedT,SpeedT); // turn cc
                    timer = 0;
                } else if (S[0] == 1){
                    Movement(-SpeedT,SpeedT); // turn cc
                    timer = 0;
                } else if (S[5] == 1){
                    Movement(SpeedT,-SpeedT); // turn c
                    timer = 0;
                } else if (S[6] == 1){
                    Movement(SpeedT,-SpeedT); // turn c
                    timer = 0;
                } else if (S[7] == 1){
                    Movement(SpeedT,-SpeedT); // turn c
                    timer = 0;
                }
            } else {
                Movement(SpeedF, SpeedF);
                timer++;
            } 
        }
    }
    return 0;
}

// wait function
void Wait(long Duration) 
{
    long i;
    for(i=0;i<Duration;i++);
}

// get selectors?
int GetSelector() {
    return SELECTOR0 + 2*SELECTOR1 + 4*SELECTOR2 + 8*SELECTOR3;
}

// move wheels of epuck
void Movement(int left, int right) {
    e_set_speed_left(left);
    e_set_speed_right(right);
}

// turn on led 
void LED_ON(unsigned int LED_number){
    e_set_led(LED_number, 1);
}

// turn off led
void LED_OFF(unsigned int LED_number){
    e_set_led(LED_number, 0);
}

void BODY_ON(){
    e_set_body_led(On); // Turn the body LED on
}

void BODY_OFF(){
    e_set_body_led(Off); // Turn the body LED on
}
