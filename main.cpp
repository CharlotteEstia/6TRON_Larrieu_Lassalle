/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

DigitalOut led1(LED1);
InterruptIn button(BUTTON1);

using namespace std::chrono;
Timer t;

namespace {
#define PERIOD_MS 2000ms
}

bool A = 0;
uint64_t elapsed_time = 0; 

void on_button_press()
{
    led1 = 1;       
    t.start(); 
    A=0;          
}

void on_button_release()
{
    led1 = 0;       
    t.stop();   
    elapsed_time = duration_cast<milliseconds>(t.elapsed_time()).count();    
    t.reset();   
    A=1;        
}

int main()
{
    button.rise(&on_button_press); 
    button.fall(&on_button_release);

    while (true) {

         if (A == 1) {
            printf("The time taken was %llu milliseconds\n", elapsed_time);
           
            A = 0; 
        }
        
        ThisThread::sleep_for(50ms); 
    }
}
