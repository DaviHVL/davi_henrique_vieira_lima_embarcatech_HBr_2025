#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <math.h>
#include "motor.h" // baixar em github.com/BitDogLab/BitDogLab-C -> Basic_peripherals -> ponteH_tb6612fng
#include "mpu6050_i2c.h" // baixar em github.com/BitDogLab/BitDogLab-C -> Basic_peripherals -> sensor_acelerometro_giroscopio

#define ANGLE_SETPOINT -7.8f
#define KP 15000.0f
#define KD 220.0f 
#define KI 1000.0f  
#define INTEGRAL_LIMIT 10.0f
#define GYRO_SENSITIVITY 131.0f
#define PWM_DEAD_ZONE 6000 

#define COMPLEMENTARY_FILTER_ALPHA 0.98f

const uint16_t PWM_WRAP = 65535;

float gyro_y_offset;

float integral_error; 

void calibrate_gyro() {
    long gyro_y_sum = 0;
    int16_t accel_raw[3], gyro_raw[3], temp;
    int num_readings = 1000;

    for (int i = 0; i < num_readings; i++) {
        mpu6050_read_raw(accel_raw, gyro_raw, &temp);
        gyro_y_sum += gyro_raw[1];
        sleep_ms(2);
    }

    gyro_y_offset = (float)gyro_y_sum / num_readings;
}

int main() {
    stdio_init_all();
    mpu6050_setup_i2c();
    mpu6050_reset();
    motor_setup();

    calibrate_gyro();

    motor_enable();

    // Variables for sensor data
    int16_t accel_raw[3], gyro_raw[3], temp;
    float gyro[3];

    mpu6050_read_raw(accel_raw, gyro_raw, &temp);
    float filtered_angle = atan2f((float)accel_raw[2], (float)accel_raw[0]) * 180.0f / M_PI;

    uint32_t last_update_time_us = time_us_32();

    while (true) {
        // Leitura dos dados do MPU6050
        mpu6050_read_raw(accel_raw, gyro_raw, &temp);

        // Cálculo do tempo decorrido
        uint32_t now_us = time_us_32();
        float dt = (now_us - last_update_time_us) / 1000000.0f;
        last_update_time_us = now_us;

        // Cálculo do ângulo a partir dos dados do acelerômetro
        float raw_angle_acc = atan2f((float)accel_raw[2], (float)accel_raw[0]) * 180.0f / M_PI;

        // Cálculo da derivada do erro usando a taxa de giro do giroscópio
        float gyro_y_dps = ((float)gyro_raw[1] - gyro_y_offset) / GYRO_SENSITIVITY;
        float derivative_error = gyro_y_dps;

        // Filtro complementar para estimar o ângulo correto
        filtered_angle = COMPLEMENTARY_FILTER_ALPHA * (filtered_angle + gyro_y_dps * dt) +
                (1.0f - COMPLEMENTARY_FILTER_ALPHA) * raw_angle_acc;


        // Cálculo do erro entre o ângulo medido e o ponto de equilíbrio
        float error = filtered_angle - ANGLE_SETPOINT;

        // Cálculo do erro integral acumulado
        integral_error += error * dt;
        if (integral_error > INTEGRAL_LIMIT){
            integral_error = INTEGRAL_LIMIT;
        } 
        else if (integral_error < -INTEGRAL_LIMIT){
            integral_error = -INTEGRAL_LIMIT;
        } 

        // Cálculo da saída do controlador PID
        float p_output = (KP * error) + (KI * integral_error) + (KD * derivative_error);

        // Definição da direção do motor
        bool direction = !(p_output > 0);

        // Conversão para valor absoluto 
        uint16_t power = (uint16_t)fabsf(p_output);

        // Limitação da potência máxima
        if (power > PWM_WRAP) {
            power = PWM_WRAP;
        }

        // Implementação da zona morta
        if (power < PWM_DEAD_ZONE) {
            power = 0;
        }

        //printf("%f\n", filtered_angle);

        // Acionamento dos motores
        motor_set_both_level(power, direction);

        sleep_ms(12); // Delay entre leituras
    }
}