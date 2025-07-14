/*
 * Control loop example program
 * This demonstrates embedded control logic
 */

int sensor_value;
int motor_speed;

void control_loop() {
    while (1) {
        if (sensor_value > 100) {
            motor_speed = 50;
        } else {
            motor_speed = 0;
        }
    }
}

int main() {
    sensor_value = 150;
    control_loop();
    return motor_speed;
}