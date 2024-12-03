#include "mbed.h"
#include "bme280.h"

using namespace sixtron;

// Objets matériels
DigitalOut led1(LED1);
InterruptIn button(BUTTON1);
I2C i2c(I2C1_SDA, I2C1_SCL);

// Objets logiciels
Ticker ticker_temp_humidity;
Thread event_thread;
Timer led_timer;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

// Mutex pour protéger l'accès au capteur
Mutex bme_mutex;

// Initialisation du capteur BME280
BME280 bme(&i2c, BME280::I2CAddress::Address1);

// Fonction pour lire et afficher la température et l'humidité
void read_temp_humidity() {
    queue.call([] {
        bme_mutex.lock();
        float temperature = bme.temperature(); // Lecture de la température en °C
        float humidity = bme.humidity();       // Lecture de l'humidité en %
        bme_mutex.unlock();

        printf("Temperature: %.2f  C, Humidite: %.2f %%\n", temperature, humidity);
    });
}

// Fonction pour lire et afficher la pression atmosphérique
void read_pressure() {
    queue.call([] {
        bme_mutex.lock();
        float pressure = bme.pressure() / 100.0f; // Convertir en hPa
        bme_mutex.unlock();
        printf("Pression atmospherique: %.2f hPa\n", pressure);
    });
}

// Fonction pour faire clignoter la LED
void toggle_led() {
    if (led_timer.read() >= 4.0f) {       // Vérifie si 5 secondes sont passées
        led1 = 1;                          // Allumer la LED
        ThisThread::sleep_for(50ms);       // Attendre 0,5 seconde
        led1 = 0;                          // Éteindre la LED
        led_timer.reset();                  // Réinitialiser le timer
    }
}

int main() {
    // Initialiser le capteur BME280
    if (!bme.initialize()) {
        printf("Échec de l'initialisation du capteur BME280\n");
        return -1;
    }

    // Configurer l'échantillonnage du capteur
    bme.set_sampling(BME280::SensorMode::NORMAL,
                     BME280::SensorSampling::OVERSAMPLING_X1,  // Température
                     BME280::SensorSampling::OVERSAMPLING_X1,  // Humidité
                     BME280::SensorSampling::OVERSAMPLING_X1,  // Pression
                     BME280::SensorFilter::OFF,                // Pas de filtrage
                     BME280::StandbyDuration::MS_1000);        // Veille 1s

    // Lancer le thread pour gérer les événements
    event_thread.start(callback(&queue, &EventQueue::dispatch_forever));

    // Configurer les tickers pour planifier les tâches périodiques
    ticker_temp_humidity.attach(&read_temp_humidity, 2s); // Température et humidité toutes les 2s
    led_timer.start();                // LED toutes les 5s

    // Configurer une interruption pour afficher la pression lorsque le bouton est appuyé
    button.rise(&read_pressure);

    // Boucle principale vide, car tout est géré par les tickers et la queue
    while (true) {
        toggle_led();
        ThisThread::sleep_for(100ms);
    }
}
