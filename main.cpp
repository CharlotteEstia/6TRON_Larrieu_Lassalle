/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

DigitalOut led1(LED1);
Thread thread1, thread2;

Mutex stdio_mutex; 

void ping_thread()
{
    stdio_mutex.lock();
    for(int i = 0; i < 100; ++i) {
        printf("Ping \n", i);
    }
    stdio_mutex.unlock();
}

void pong_thread()
{
     stdio_mutex.lock();
    for(int i = 0; i < 100; ++i) {
        printf("Pong \n", i);
    }
    stdio_mutex.unlock();
}

int main()
{
    thread1.start(ping_thread);
    thread2.start(pong_thread);

    while (true) {
        led1 = !led1;
        printf("Alive! \n");
        ThisThread::sleep_for(500);
    }
}
