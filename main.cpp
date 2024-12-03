#include "mbed.h"

DigitalOut led1(LED1);
Thread thread1, thread2;

Mutex stdio_mutex; 

// Thread ping
void ping_thread()
{
    stdio_mutex.lock();
    for(int i = 0; i < 100; ++i) {
        printf("Ping \n");
    }
    stdio_mutex.unlock();
}

// Thread pong
void pong_thread()
{
    stdio_mutex.lock();
    for(int i = 0; i < 100; ++i) {
        printf("Pong \n");
    }
    stdio_mutex.unlock();
}

int main()
{
    // Lancer les threads
    thread1.start(ping_thread);
    thread2.start(pong_thread);

    thread1.set_priority(osPriorityLow);
    thread2.set_priority(osPriorityHigh);

    // Boucle principale pour faire clignoter la LED
    while (true) {
        led1 = !led1;  
        printf("Alive! \n");  
        ThisThread::sleep_for(500ms);  
    }
}
