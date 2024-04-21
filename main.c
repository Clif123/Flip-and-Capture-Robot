#include "project.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

//////////////////// Change the colour of the robot here

int Red_Robot = 0;           //0 means on
int Blue_Robot = 1;

////////////////////

char str [128];
char str1 [128];
char str2 [128];
int rdy;
int x;
int y;
int i_straight, i_reverse;
int base_x,back;
int up,down,left,right;
int a;
int gripping, reverse_grip;
int release_grip = 0;
int grip_2 = 0;
int val,val2;
int y_initial = 0; 
int some_var = 0;
int reversing = 0;
int temp_xx = 0;
// x reverse is for NEAR
int x_reverse = 0;  //changed in 3 places
//int y_start_val = 200;
// ori 55
int y_start_val = 133;   //129 //104 for 1st row    //123

int compare_val = 0;
//int temp_coor = 0;
int coor_val = 0;
int temp_comp = 0;
int diff = 0;
int x_puck_val;
int flip_up = 0;

uint16 freq = 0;

int color = 0;
int ir_done = 0;
int position = 0;
int reverse_temp = 0;
int puck_drop = 0;
int puck_drop2 = 0;
int x_drop_val = 0;
int puck_done = 0;
int green_dly;
int compare,count,count2,count3,error,slave,state;
int distance_measured;
int zone = 1;

int kp = 55; //35
int temp=0;
int temp_ir = 0;
int count_echo = 0;
int echo_select = 0;
int obstacle = 0;
int double_puck = 0;
int pwm = 32767; // full 65535 
int temp_x = 0;
int puck_compare = 0;
int x_start = 0;

//int count = 0;
int distance_arr[6] = {0,0,0,0,0,0};
int temp_distance;
int final_dist = -10;

//int temp_reverse = 0;
char *RGB[] = { "RED", "BLUE", "GREEN" } ;
int col_arr[3];
int fin_col,location;
int size = 3;
#define straight 1
#define stop 2
#define turn 3
#define reverse 4
#define puck 5
#define base 6
#define gripper 7
#define turn_right 8


#define RED_COLOUR   0u
#define BLUE_COLOUR  1u
#define GREEN_COLOUR 2u

#define up 9
#define down 10
#define left 11
#define right 12

//function declaration
void motor(int state);
int color_sense(int color);
void measure(void);
int freq_output(int color);


CY_ISR(Echo_ISR_Handler)
{
    //UART_1_PutString(str1);
    Timer_1_ReadStatusRegister();
    count3 = Timer_1_ReadCounter();
    
    distance_measured = (65535 - count3)/58; //distance measured in cm  //each 58us is equal to 1cm
   
    
    if((count_echo<6) && (distance_measured >= 3))  //6
    {   
        distance_arr[count_echo] = distance_measured;
        count_echo++;
//         sprintf(str2, "dist measured:%d\n ", distance_arr[count_echo]);
//         UART_1_PutString(str2) ;
    }
    
//     sprintf(str2, " %d   %d\n ", distance_measured, count_echo) ;
//     UART_1_PutString(str2) ;
   if (count_echo==6)  //6
    {
        for(int i=0;i<5;i++)   //5
        {
            for(int j=0;j<5-i-1;j++)  //6-i-1
            {
                if(distance_arr[j] > distance_arr[j+1])
                {
                    temp_distance       = distance_arr[j];
                    distance_arr[j]   = distance_arr[j+1];
                    distance_arr[j+1] = temp_distance;
                }
            }
        }
        
        final_dist = (distance_arr[2] + distance_arr[3])/2;  //3&2
//        sprintf(str2, "final dist:%d\n ", final_dist);
//        UART_1_PutString(str2) ;
        memset(distance_arr,0,sizeof(distance_arr));
        count_echo = 0;
    }
   
    
//    if (echo_select == 1)
//    {
//        echo_select = 0;
//    }
//    else
//    {
//        echo_select++;
//    }
    
    
    
    //sprintf(str1, "distance from ultra:  %d\n", distance_measured);
    //UART_1_PutString(str1);
    
}

CY_ISR(counter_ISR)
{
    Counter_1_Stop();
    Timer_4_Stop();
    
    LED_coloursensor_Write(0);
    Timer_4_ReadStatusRegister();
    counter_ISR_ClearPending();
    
    freq = Counter_1_ReadCounter();
    
    rdy = 1;
}

CY_ISR(Trigger_ISR_Handler)
{
    Timer_2_ReadStatusRegister();
////   // UART_1_PutString("i am TRIGGERED !!! \n");
    
    if(Echo_1_Read()==0)
    {
     //UART_1_PutString("i am here \n");
        Trigger_1_Write(1);
        CyDelayUs(10);
        Trigger_1_Write(0);
    }
    
}


CY_ISR(State_ISR_Handler)
{
    Timer_3_ReadStatusRegister();
    
    switch (state)
    {
        case straight:
        
        motor(stop);
        temp_comp = 0;
        coor_val = 0;
        diff = 0;
        compare_val = 0;
        Green_Write(0);
        
        if  (reverse_temp == 1)
        {
            if (position == up)
            {
                position = down;
                reverse_temp = 0;
            }
            else if (position == right)
            {
                position = left;
                reverse_temp = 0;
            }
            else if (position == down)
            {
                position = up;
                reverse_temp = 0;
            }
            else if (position == left)
            {
                position = right;
                reverse_temp = 0;
            }
        }
        
        i_straight = 1;
        
        while(i_straight == 1)
        {
            motor(state);
            
//            sprintf(str2, "y: %d/n ", y) ;
//            UART_1_PutString(str2) ;
            CyDelay(100);
            
            compare_val = compare_val + compare;
            diff = compare_val - temp_comp;
            
            if (diff >= 5000)
            {
                temp_comp = compare_val;
                
                coor_val = round(diff/5000);
               
                if (position == up)
                {
                    y = y + coor_val;
                }
                else if (position == left)
                {
                    x = x - coor_val;
                }
                else if (position == down)
                {
                    y = y - coor_val;
                }
                else if (position == right)
                {
                    x = x + coor_val;
                }

                // sprintf(str, "x=%d        y=%d       position=%d     x_drop_val=%d    \n", x,y,position,x_drop_val);
                // UART_1_PutString(str);
            }
            
            if(x_start==1)
            {
                
//                if(temp_x>=compare)  //puck_compare >= compare
//                {
//                    puck_compare = puck_compare + temp_x;
//                }
                
                //temp_x = compare;
                
                puck_compare = (some_var*32000) + compare;
                
                
                if (temp_x>=compare)
                {
                    some_var = some_var + 1;
                }
                
                temp_x = compare;
            }
            
            
            if ((y >= y_start_val) && (y_initial == 0) && (zone == 1))
            {
                
                state = turn_right;
                x_start = 1;
                y_initial = 1;
                i_straight = 0;
                //y_start_val = y_start_val + 30;
                //y_start_val = y + 50;
                x_reverse = 0;
                puck_compare = 0;
            }
            
            //if ((puck_drop == 1) && (x <= x_drop_val - 10))
	        //if ((puck_drop == 1) && (y >= 75))
            if ((puck_drop == 1) && (final_dist <= 11) && (y >= 140))
            {
                //state = turn;
                
		        state = turn_right;
                puck_drop = 0;
                puck_drop2 = 1;
		        zone = 2;
                i_straight = 0;
            }
            
            //else if ((zone == 2) && (x >= 48))
            else if ((zone == 2) && (final_dist <= 14) && (x >= 65))
//            else if ((zone == 2) && (final_dist <= 14))
    	    {           
        		state = turn_right;
        		zone = 3;
        		i_straight = 0;
    	    }
            
            //else if ((zone == 3) && (y <= 4))
            else if ((zone == 3) && (final_dist <= 15) && (y <= 8))
            {
                motor(stop);
                state = gripper;
                i_straight = 0;
            }
            
    	    //else if ((zone == 4) && (x <= -5))
//            else if ((zone == 4) && (final_dist <= 23) && (x <= 35))  //x<=28
//    	    {
//                y_initial = 0;
//        		state = turn_right;
//        		//zone = 1; 
//        		i_straight = 0;
//    	    }
            
            else if (zone == 4)  //&& (final_dist <= 23))  //x<=28
    	    {
                PWM_B_WriteCompare(48495);     //old12:70%:45874
                CyDelay(3000);
                
                while (final_dist >= 18) //24
                {
                    count = QuadDec_1_GetCounter(); // master 
                    count2= QuadDec_2_GetCounter(); // slave 
                    error = count - count2; // master minus slave 
                    
                    if (error <= 0)
                    {
                        error = 0;
                    }
                    
                    slave = (kp * error)+ pwm;
                    
                    PWM_B_WriteCompare(slave);
                    compare = (count2+count)/2;
                }
                state = turn_right;
                 y_initial = 0;
            		
            	//zone = 1; 
            	i_straight = 0;
    	    }
                        
            //ultrasonic detection
//            if ((distance_measured >0) && (distance_measured < 13))
//            {
//                motor(stop);
//                state = reverse;
//                obstacle = 1;
//                i_straight = 0;
//                
//            }
            
            //IR detection
            if ((IR_sensor_Read()==0) && (double_puck == 0) && (zone == 1))
            {
                if (green_dly == 1)
                {
                    CyDelay(300);
                    green_dly = 0;
                    //x_reverse = -42;
                }
                else
                {
                    double_puck = 1;
                    motor(stop);
                   // UART_1_PutString("I am here");
                    //temp_ir = 1;
                    state = puck;
                    i_straight = 0;
                }
            }
            
            //change compare value later or use xy coordinates
            //This is for going straight to grip the puck after reversing
            if ((grip_2 == 1) && (x >= x_puck_val - 26))
            {
             
                motor(stop);
                //x_reverse = x;
                x_reverse = puck_compare;
                some_var = 0;
                state = gripper;
                i_straight = 0;
                
            }
        }
        
        //compare_val = 0;
        
        break;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case turn:
        
        motor(stop);
        //reseting 
        //reverse_temp = 0;
        
        if (reverse_temp == 1)
        {
            if (position == up)
            {
                position = right;
            }
            else if (position == left)
            {
                position = up;
            }
            else if (position == right)
            {
                position = down;
            }
            else if (position == down)
            {
                position = left;
            }
            reverse_temp = 0;
        }
        else
        {
            if (position == up)
            {
                position = left;
            }
            else if (position == left)
            {
                position = down;
            }
            else if (position == right)
            {
                position = up;
            }
            else if (position == down)
            {
                position = right;
            }
        }
        
        
        motor(state);
        state = straight;
        
        break;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case turn_right:
        
        if (reverse_temp == 1)
        {
            if (position == up)
            {
                position = left;
            }
            else if (position == left)
            {
                position = down;
            }
            else if (position == right)
            {
                position = up;
            }
            else if (position == down)
            {
                position = right;
            }
            reverse_temp = 0;
        }
        else
        {
            if (position == up)
            {
                position = right;
            }
            else if (position == left)
            {
                position = up;
            }
            else if (position == right)
            {
                position = down;
            }
            else if (position == down)
            {
                position = left;
            }
        }
        
        motor(stop);
        //reseting 
        reverse_temp = 0;
        
        //temp = state;
        
        
        motor(state);
        
        if ((zone == 3) || ((zone == 4) && (y_initial == 0)))
        {
            state = reverse;
        }
        else
        {
            state = straight;
        }
        
        break;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case puck:
        
        memset(col_arr,0,sizeof(col_arr));
        location = 0;
        
        //colour sensor function
        motor(stop);
//        UART_1_PutString("In puck case \n");
        LED_coloursensor_Write(1);
        
        for (int i=0; i<3 ; i++)
        {
            for (color = 0 ; color < 3 ; color++ ) 
            {
                // UART_1_PutString("IN LOOP\n");
                freq = freq_output(color) ;
//                sprintf(str2, " RGB Colour: %5s Freq: %5u Hz\n", RGB[color],freq) ;
//                UART_1_PutString(str2) ;
//                UART_1_PutString("WORKS\n");
                col_arr[color] = col_arr[color] + freq;
            }
        }
        
        fin_col = col_arr[0];
        
        for (int i = 1; i < size; i++)
        {
            if(col_arr[i] > fin_col)
            {
                fin_col = col_arr[i];
                location = i+1;
            }
        }
        
//        sprintf(str2, "final colour: %5s/n ", RGB[location-1] ) ;
//        UART_1_PutString(str2) ;
        
        if(location != 0)
        {
            location = location - 1;
        }
        
//        sprintf(str2, "location: %d/n ", location ) ;
//        UART_1_PutString(str2) ;
        
        if(location == 0)
        {
            Red_Write(1);
           // CyDelay(3000);
           // Red_Write(0);
        }
        else if(location == 1)
        {
           Blue_Write(1);
          
           // CyDelay(3000);
           //Blue_Write(0);
        }
        else if(location == 2)
        {
            Green_Write(1);
           // CyDelay(3000);
           // Green_Write(0);
            double_puck = 0;
            green_dly = 1;
        }
        
        //if puck is red or blue, reverse in prep to grip puck, else, go straight and ignore green wan
        if ((location == 1) || (location == 0))
        {
            gripping = 1;
            x_puck_val = x;
            motor(stop);
            state = reverse;
        }
        else
        {
            state = straight;
        }
        break;
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case reverse:
        
        motor(stop);
        //motor(state);
        reverse_temp = 1;
        //UART_1_PutString("In reverse case \n");
        temp_comp = 0;
        coor_val = 0;
        diff = 0;
        compare_val = 0;
        
        
        if (position == down)
        {
            position = up;
        }
        else if (position == left)
        {
            position = right;
        }
        else if (position == up)
        {
            position = down;
        }
        else if (position == right)
        {
            position = left;
        }
        
        //CyDelay(300);
        i_reverse = 1;
        
        while(i_reverse == 1)
        {
//            sprintf(str, "y_test = %d\n", y);
//            UART_1_PutString(str);
            
            motor(state);
            CyDelay(100);
            
            compare_val = compare_val + compare;
            diff = compare_val - temp_comp;
            
//            sprintf(str, "compare= %d    \n", compare);
//            UART_1_PutString(str);
           
            if (diff >= 5000) 
            {
                temp_comp = compare_val;
                
                coor_val = round(diff/5000);
            
                if (position == up)
                {
                    y = y + coor_val;
                }
                else if (position == left)
                {
                    x = x - coor_val;
                }
                else if (position == down)
                {
                    y = y - coor_val;
                }
                else if (position == right)
                {
                    x = x + coor_val;
                }
                //temp_comp = compare;
                
               // sprintf(str, "x=%d        y=%d       position=%d     x_puck_val=%d    \n", x,y,position,x_puck_val);
               //   UART_1_PutString(str);
            }
        
            if ((x <= x_puck_val - 35) && (gripping == 1) && (reverse_grip == 0))//30
            {
                reverse_grip = 1;
                i_reverse = 0;
                x_start = 0;
            }
            
            if ((reverse_grip == 1) && (gripping == 1))
            {
                 motor(stop);
                //CyDelay(1000);
                // reset first
                reverse_grip = 0;
                gripping = 0;
                state = gripper;
                i_reverse = 0;
            }
            
            if ((Contact_Switch1_Read() == 1) && (Contact_Switch2_Read() == 1))
            {  
                if (zone == 4) 
                {
                    zone = 1;
                    //resetting xy coordinates
                    x = 0;
                    y = 0;
                 
                }
                some_var = 0;
                state = straight;
                i_reverse = 0;
            }
           // if ((compare >= x_reverse) && (puck_drop == 1))
	        //if ((x <= x_puck_val - 10) && (puck_drop == 1) && (obstacle == 0))
            if (puck_drop == 1)
            {                
                 reversing = (some_var*32000) + compare;
                
                
                if (temp_xx>=compare)
                {
                    some_var = some_var + 1;
                }
                
                temp_xx = compare;
                
                if (reversing >= puck_compare - 7000)
                {
                    state = turn;
                    i_reverse = 0;
                }
                //x_reverse = 0;
       
            }
            
            //if ((puck_done == 1) && (y >= y_start_val))
            //if ((puck_done == 1) && (y >= 0) && (y_initial == 1))
            if ((puck_done == 1) && (final_dist >= 18) && (y_initial == 1)) //20 final dist
            {
                //y_start_val = y_start_val + 50;
                i_reverse = 0;
                zone = 4; 
                state = gripper;
                //puck_done = 0;
                double_puck = 0;
		              
            }
        }
        
        // manually trying to stop the reversing in order to reverse and grip the puck after color sense
        // change this compare value laterr
        
        break;
    
        case gripper:
            
       // UART_1_PutString("in gripper state") ;
         Red_Write(0);
         Blue_Write(0);
 
        
        if ((grip_2 == 0) && (puck_drop2 == 0))
        {
            Control_Reg_2_Write(0);
            
            val= 20000-2320;
            PWM_Servo_WriteCompare(val);   //flipping down   
            CyDelay(230);
            
            Control_Reg_3_Write(0);
            
            val2= 20000-800;
            PWM_Servo_Gripping_WriteCompare(val2);    //ungripping
            
            grip_2 = 1;
            state = straight; 
        }
        
        else if ((grip_2 == 1) && (puck_drop2 == 0))  //gripping the puck after reversing and flipping
        {
            val2= 20000-470;
            PWM_Servo_Gripping_WriteCompare(val2);     //gripping
            //trying
            CyDelay(300);
            val= 20000-450;
            PWM_Servo_WriteCompare(val);            //flipping up
            CyDelay(300);
           

            puck_drop = 1;
            grip_2 = 0;
            // for now fix later 
            //motor(stop);
            Control_Reg_2_Write(1);     //trying
            state = reverse;
      
        }
        //need to add condition (blue/ red robot)
        if ((puck_drop2 == 1) && (puck_done == 0))
        {   
            if (((location == 1) && (Red_Robot == 0)) || ((location == 0) && (Blue_Robot == 0)))
            {
            
                for (int i = 450; i < 700; i= i+1)//i+35
                {
                    val2= 20000 - i;
                    PWM_Servo_Gripping_WriteCompare(val2);  //ungripping
                    //CyDelay(500);
                }

            }
            
            else if (((location == 0) && (Red_Robot == 0)) || ((location == 1) && (Blue_Robot == 0)))
            {
                Control_Reg_2_Write(0);
                val= 20000-2320;
                PWM_Servo_WriteCompare(val);   //flipping down   
                CyDelay(280);    //280
                
                val2= 20000-800;
                PWM_Servo_Gripping_WriteCompare(val2);    //ungripping
                CyDelay(100);       //100
                
                flip_up = 1;
            }
            
            puck_done = 1;
            state = reverse; 
        }
            
        else if (zone == 4)
        {
            PWM_Servo_Gripping_WriteCompare(20000-430);  //gripping
            CyDelay(500);
            
            if (flip_up == 1)
            {
                val= 20000-450;
                PWM_Servo_WriteCompare(val);            //flipping up 
                flip_up = 0;
                CyDelay(500);
            }
            
            Control_Reg_2_Write(1);
            Control_Reg_3_Write(1);
            
            puck_drop2 = 0;
            puck_done = 0;
            state = turn_right;   
        }
               
        break;
    }     
}

int main(void)
{
    //Initializing modules
    Timer_1_Start();
    Timer_2_Start();
    Timer_3_Start();
    QuadDec_1_Start();
    QuadDec_2_Start();
    UART_1_Start();
    PWM_A_Start();
    PWM_B_Start();
    PWM_Servo_Start();
    PWM_Servo_Gripping_Start();
    
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    isr_trigger_StartEx(Trigger_ISR_Handler);
    isr_echo_StartEx(Echo_ISR_Handler);
    isr_state_StartEx(State_ISR_Handler);
    counter_ISR_StartEx(counter_ISR) ;

    // Initial motor state for holder
    Control_Reg_2_Write(0);
    val= 20000-450;
    PWM_Servo_WriteCompare(val);
    
    // Initial motor state for gripper
    Control_Reg_3_Write(0);
    val2= 20000-450; //430
    PWM_Servo_Gripping_WriteCompare(val2);
    
    CyDelay(500);
    Control_Reg_2_Write(1);
    Control_Reg_3_Write(1);
    
    state = straight;
    //up = 1;
    position = up;
        
    for(;;)
    {
      
    }
}

void motor(state)
{
    if (state == straight)
    {
        //UART_1_PutString("STRAIGHT STATE\n");
        Pin_Motor_A2_Write(1);
        Pin_Motor_A1_Write(0);
        
        Pin_Motor_B2_Write(1);
        Pin_Motor_B1_Write(0);
        
        count = QuadDec_1_GetCounter(); // master 
        count2= QuadDec_2_GetCounter(); // slave 
        error = count - count2; // master minus slave 
        
        if (error <= 0)
        {
            error = 0;
        }
        
//        if (count >= 31000)
//        {
//           QuadDec_1_SetCounter(0);
//           QuadDec_2_SetCounter(0);
//        }
        
        
        slave = (kp * error)+ pwm;
        
        PWM_B_WriteCompare(slave);
        compare = (count2+count)/2;
                
//        sprintf(str, "STRAIGHT %d        %d           %d           %d    %d\n", count, count2, compare, error, slave);
//        UART_1_PutString(str);
    }
    
    else if (state == stop || state == base)
    {
        Pin_Motor_A2_Write(0);
        Pin_Motor_A1_Write(0);

        Pin_Motor_B2_Write(0);
        Pin_Motor_B1_Write(0);
        // trying initial=500
        CyDelay(100);
        
        QuadDec_1_SetCounter(0);
        QuadDec_2_SetCounter(0);
        compare = 0;
    }
    
    else if (state == turn)
    {
        //UART_1_PutString("TURN LEFT STATE\n");
        
        if(zone == 3)
        {
            a = 16000;    
        }
        else
        {
            a = 14000;
        }
        
        while (compare <= a)
        {   
            //kp= 25;
            // master forward
            Pin_Motor_A2_Write(0);
            Pin_Motor_A1_Write(1);
            
            // slave backward
            Pin_Motor_B2_Write(1);
            Pin_Motor_B1_Write(0);
            
            count = QuadDec_1_GetCounter(); // master 
            count2= QuadDec_2_GetCounter(); // slave 
            error = -count - count2; // master minus slave 
  
            slave = (kp * error)+ pwm;
            PWM_B_WriteCompare(slave);
            compare = (count2-count)/2;
            
//            sprintf(str, "%d        %d           %d           %d      %d\n", count, count2, compare, error, slave);
//            
//            UART_1_PutString(str);
        } 
    }

    else if (state == turn_right)
    {
        //UART_1_PutString("TURN RIGHT STATE\n");

        //compare = 7900
        if(zone == 3)
        {
            a = 16950;  //16600
        }
        else if (zone == 2)
        {
            a = 12050;  //old12:12200
        }
        //puck capture zone
        else if ((zone == 4) && (y_initial == 1))
        {
            a = 11300;  //11500 //12600
        }
        //reversing to base
        else if ((y_initial == 0) && (zone == 4))
        {
            a = 18500;    //18900  //19200
        }
        else
        {
            a = 14300; //14400 full batt
        }
        while (compare <= a)
        {   
            //kp= 25;
            // master forward
            Pin_Motor_A2_Write(1);
            Pin_Motor_A1_Write(0);
            
            // slave backward
            Pin_Motor_B2_Write(0);
            Pin_Motor_B1_Write(1);
            
            count = QuadDec_1_GetCounter(); // master 
            count2= QuadDec_2_GetCounter(); // slave 
            error = count + count2; // master minus slave 
            
            if (error <= 0)
            {
                error = 0;
            }
            slave = (kp * error)+ pwm;
            PWM_B_WriteCompare(slave);
            compare = (count-count2)/2;
            
//            sprintf(str, "%d        %d           %d           %d      %d\n", count, count2, compare, error, slave);
//            
//            UART_1_PutString(str);
        }      
    }
    
    
    else if(state == reverse)
    { 
        // master forward
        Pin_Motor_A2_Write(0);
        Pin_Motor_A1_Write(1);
            
        // slave backward
        Pin_Motor_B2_Write(0);
        Pin_Motor_B1_Write(1);
            
        count = QuadDec_1_GetCounter(); // master 
        count2= QuadDec_2_GetCounter(); // slave 
        error = -(count - count2); // master minus slave 
        
        if (error <= 0)
        {
            error = 0;
        }
        
        //If either contact switch is actuated, no error correction
        if ((Contact_Switch1_Read() == 0) && (Contact_Switch2_Read() == 0))
        {
            slave = (kp * error)+ pwm;
                
            PWM_B_WriteCompare(slave);
            compare = -(count2+count)/2; 
        }
        else if (Contact_Switch2_Read() == 1)
        {
            while (Contact_Switch1_Read() == 0)
            { 
                slave = pwm;
                PWM_B_WriteCompare(slave);
            }
        }
        else if (Contact_Switch1_Read() == 1)
        {
            while (Contact_Switch2_Read() == 0)
            {
                PWM_A_WriteCompare(40000);
                PWM_B_WriteCompare(32767);
            }
            PWM_A_WriteCompare(32767);
        }
        
        
       
//        sprintf(str, "%d        %d           %d           %d    %d\n", count, count2, compare, error, slave);
//        UART_1_PutString(str);
        
        
    }

    
}

void measure(void)
{
    LED_coloursensor_Write(1) ;
    Control_Reg_1_Write(1) ;
    CyDelay(1) ;
    Control_Reg_1_Write(0) ;
    rdy = 0 ;
    Counter_1_Start() ;

    Timer_4_Start() ;
}

int freq_output(int color)
{
    color_sense(color) ;
    S0_Write(1) ; 
    S1_Write(1) ;
    measure() ;
    //UART_1_PutString("HELLO");
    while(!rdy) {
        ;
    }
    return(freq) ;
}

int color_sense(int color)
{
    int result = 0 ;
    switch(color) {
    case RED_COLOUR:
        S2_Write(0) ; 
        S3_Write(0) ; 
        break ;
    case BLUE_COLOUR:
        S2_Write(0) ; 
        S3_Write(1) ; 
        break ;
    case GREEN_COLOUR:
        S2_Write(1) ;
        S3_Write(1) ; 
        break ;
    default:
//        sprintf(str, "err %d\n", color) ;
//        UART_1_PutString(str) ;
        result = -1 ;
        break ;
    }
    return result; 
}



