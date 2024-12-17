#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// LEDC configuration
#define LEDC_TIMER              LEDC_TIMER_0      // Use timer 0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE // Low-speed mode for LED control
#define LEDC_OUTPUT_GPIO        2                 // GPIO pin for PWM output
#define LEDC_CHANNEL            LEDC_CHANNEL_0    // Use channel 0
#define LEDC_DUTY_RESOLUTION    LEDC_TIMER_10_BIT // 10-bit resolution (0-1023)
#define LEDC_FREQUENCY          500              // Frequency in Hz (e.g., 1 kHz PWM)

// Function to configure the LEDC peripheral
void ledc_pwm_init(void) {
    // Configure the timer for LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,             // LEDC speed mode
        .timer_num        = LEDC_TIMER,            // Timer index
        .duty_resolution  = LEDC_DUTY_RESOLUTION,  // Duty resolution
        .freq_hz          = LEDC_FREQUENCY,        // PWM frequency
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
        .duty           = 0,                       // Initial duty cycle (0%)
        .hpoint         = 0                        // PWM start point
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

// Function to set PWM duty cycle
void set_pwm_duty_cycle(uint32_t duty_cycle) {
    // Set the duty cycle (duty cycle range depends on resolution: 0-1023 for 10-bit)
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void app_main(void) {
    printf("Initializing LEDC PWM...\n");

    // Initialize the LEDC peripheral
    ledc_pwm_init();

    // Set the PWM duty cycle to 50% (for 10-bit resolution: 50% = 512)
    uint32_t duty_cycle = 512; // 50% of 1023 (10-bit resolution)
    set_pwm_duty_cycle(duty_cycle);

    printf("PWM output started on GPIO %d with 50%% duty cycle.\n", LEDC_OUTPUT_GPIO);

    // Keep the application running
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Keep the task alive
    }
}
