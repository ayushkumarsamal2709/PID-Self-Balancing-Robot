#include "I2Cdev.h"
#include <PID_v1.h> // Ensure this is installed via the Library Manager
#include "MPU6050_6Axis_MotionApps20.h" 

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  
uint8_t mpuIntStatus;   
uint8_t devStatus;      
uint16_t packetSize;    
uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 

// orientation/motion vars
Quaternion q;           
VectorFloat gravity;    
float ypr[3];           

/*********Tune these 4 values for your BOT*********/
double setpoint = 182; // Set this when the bot is perpendicular to ground
double Kp = 15;        // Proportional gain
double Kd = 0.9;       // Derivative gain
double Ki = 140;       // Integral gain
/******End of values setting*********/

double input, output;
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

volatile bool mpuInterrupt = false;     
void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {
    Serial.begin(115200);
    
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    
    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    
    // load and configure the DMP
    devStatus = mpu.dmpInitialize();
    
    // supply your own gyro offsets here
    mpu.setXGyroOffset(-479);
    mpu.setYGyroOffset(84);
    mpu.setZGyroOffset(15);
    mpu.setZAccelOffset(1638); 

    if (devStatus == 0) {
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
        
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;
        
        packetSize = mpu.dmpGetFIFOPacketSize();
        
        // setup PID
        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(10);
        pid.SetOutputLimits(-255, 255);
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // Initialise the Motor output pins
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    
    // Turn off both motors by default
    analogWrite(6, 0);
    analogWrite(9, 0);
    analogWrite(10, 0);
    analogWrite(11, 0);
}

void loop() {
    if (!dmpReady) return;

    while (!mpuInterrupt && fifoCount < packetSize) {
        pid.Compute();
        
        Serial.print(input); Serial.print(" => "); Serial.println(output);
        
        if (input > 150 && input < 200) { 
            if (output > 0) Forward(); 
            else if (output < 0) Reverse(); 
        } else {
            Stop(); 
        }
    }

    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();

    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));
    } else if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;
        
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        
        input = ypr[1] * 180 / M_PI + 180;
    }
}

void Forward() {
    analogWrite(6, output);
    analogWrite(9, 0);
    analogWrite(10, output);
    analogWrite(11, 0);
    Serial.print("F"); 
}

void Reverse() {
    analogWrite(6, 0);
    analogWrite(9, output * -1);
    analogWrite(10, 0);
    analogWrite(11, output * -1);
    Serial.print("R");
}

void Stop() {
    analogWrite(6, 0);
    analogWrite(9, 0);
    analogWrite(10, 0);
    analogWrite(11, 0);
    Serial.print("S");
}