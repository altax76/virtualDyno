#include <stdio.h>
#include <math.h> // For sine wave calculation
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h" // For esp_timer_get_time()

// LEDC configuration
#define LEDC_TIMER              LEDC_TIMER_0      // Use timer 0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE // Low-speed mode for LED control
#define LEDC_OUTPUT_GPIO        4                 // GPIO pin for PWM output
#define LEDC_CHANNEL            LEDC_CHANNEL_0    // Use channel 0
#define LEDC_DUTY_RESOLUTION    LEDC_TIMER_10_BIT // 10-bit resolution (0-1023)
#define INITIAL_DUTY_CYCLE      512               // 50% duty cycle for 10-bit resolution

// Oscillation parameters
#define MIN_FREQUENCY           1000               // Minimum frequency in Hz
#define MAX_FREQUENCY           1500              // Maximum frequency in Hz
#define OSCILLATION_PERIOD_MS   5000              // Oscillation period in milliseconds



// Function to configure the LEDC timer and channel
void ledc_pwm_init(void) {
    // Configure the timer for LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,             // LEDC speed mode
        .timer_num        = LEDC_TIMER,            // Timer index
        .duty_resolution  = LEDC_DUTY_RESOLUTION,  // Duty resolution
        .freq_hz          = MIN_FREQUENCY,         // Initial PWM frequency
        .clk_cfg          = LEDC_AUTO_CLK          // Auto-select clock source
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,               // LEDC speed mode
        .channel        = LEDC_CHANNEL,            // LEDC channel index
        .timer_sel      = LEDC_TIMER,              // Timer selection
        .intr_type      = LEDC_INTR_DISABLE,       // Disable interrupts
        .gpio_num       = LEDC_OUTPUT_GPIO,        // GPIO number for PWM output
        .duty           = INITIAL_DUTY_CYCLE,      // Initial duty cycle (50%)
        .hpoint         = 0                        // PWM start point
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

// Function to update the PWM frequency dynamically
void update_pwm_frequency(uint32_t frequency) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,             // LEDC speed mode
        .timer_num        = LEDC_TIMER,            // Timer index
        .duty_resolution  = LEDC_DUTY_RESOLUTION,  // Duty resolution
        .freq_hz          = frequency,             // New PWM frequency
        .clk_cfg          = LEDC_AUTO_CLK          // Auto-select clock source
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

// Task to modulate the PWM frequency
void pwm_frequency_modulation_task(void *pvParameter) {
    const double omega = (2.0 * M_PI) / OSCILLATION_PERIOD_MS; // Angular frequency for oscillation

    while (1) {
        // Get current time in milliseconds
        int64_t time_ms = esp_timer_get_time() / 1000;

        // Calculate the current frequency based on a sine wave
        double sine_value = sin(omega * time_ms);
        uint32_t current_frequency = MIN_FREQUENCY + (uint32_t)((MAX_FREQUENCY - MIN_FREQUENCY) * (sine_value + 1) / 2);

        // Update the PWM frequency
        update_pwm_frequency(current_frequency);

        // Delay to reduce CPU usage
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    // Initialize the LEDC peripheral
    ledc_pwm_init();

    // Create the PWM frequency modulation task
    xTaskCreate(pwm_frequency_modulation_task, "pwm_frequency_modulation_task", 2048, NULL, 5, NULL);
}
