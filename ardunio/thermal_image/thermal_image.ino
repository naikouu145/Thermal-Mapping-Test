#include <Wire.h>
#include <Adafruit_MLX90640.h>

// ESP32-S3 CAM I2C pins - adjust these to match your wiring
#define I2C_SDA 1   // Common GPIO for SDA on ESP32-S3 CAM
#define I2C_SCL 2   // Common GPIO for SCL on ESP32-S3 CAM

Adafruit_MLX90640 mlx;
float frame[32 * 24];  // buffer for full frame of temperatures

void scanI2C() {
  Serial.println("DEBUG: Scanning I2C bus...");
  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("DEBUG: Found device at 0x");
      Serial.println(addr, HEX);
      count++;
    }
  }
  Serial.print("DEBUG: Found ");
  Serial.print(count);
  Serial.println(" device(s)");
}

void setup() {
  Serial.begin(115200);
  
  // Wait longer for serial to be ready
  delay(3000);  // ESP32 needs more time
  
  Serial.println();
  Serial.println("========================");
  Serial.println("DEBUG: Serial initialized");
  Serial.println("DEBUG: MLX90640 Thermal Camera");
  Serial.println("DEBUG: Running on ESP32-S3");
  Serial.println("========================");
  
  // Print ESP32 chip info
  Serial.print("DEBUG: ESP32 Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("DEBUG: CPU Freq: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("DEBUG: Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  Serial.println("DEBUG: Initializing I2C...");
  Serial.print("DEBUG: SDA Pin: ");
  Serial.println(I2C_SDA);
  Serial.print("DEBUG: SCL Pin: ");
  Serial.println(I2C_SCL);
  
  // Initialize I2C with specific pins for ESP32
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);  // 400kHz I2C clock
  delay(100);
  
  // Scan for I2C devices
  scanI2C();
  
  Serial.println("DEBUG: Attempting to initialize MLX90640...");
  Serial.print("DEBUG: Using I2C address 0x");
  Serial.println(MLX90640_I2CADDR_DEFAULT, HEX);
  
  if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("ERROR:MLX90640_NOT_FOUND");
    Serial.println("DEBUG: Check wiring - SDA/SCL connections");
    Serial.println("DEBUG: Check power - sensor needs 3.3V");
    Serial.println("DEBUG: Try different GPIO pins if needed");
    while (1) {
      Serial.println("DEBUG: Halted - sensor not found");
      delay(5000);
    }
  }
  
  Serial.println("DEBUG: MLX90640 found!");
  Serial.print("DEBUG: Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  Serial.println("DEBUG: Setting mode to CHESS...");
  mlx.setMode(MLX90640_CHESS);
  
  Serial.println("DEBUG: Setting resolution to 18-bit...");
  mlx.setResolution(MLX90640_ADC_18BIT);
  
  Serial.println("DEBUG: Setting refresh rate to 4Hz...");
  mlx.setRefreshRate(MLX90640_4_HZ);
  
  Serial.println("DEBUG: Setup complete!");
  Serial.println("READY");
}

void loop() {
  static unsigned long frameCount = 0;
  static unsigned long lastDebugTime = 0;
  
  delay(250);
  
  Serial.println("DEBUG: Requesting frame...");
  int status = mlx.getFrame(frame);
  
  if (status != 0) {
    Serial.print("ERROR:FRAME_FAILED (code=");
    Serial.print(status);
    Serial.println(")");
    return;
  }
  
  frameCount++;
  
  // Print debug info every 10 frames
  if (millis() - lastDebugTime > 5000) {
    Serial.print("DEBUG: Frame #");
    Serial.print(frameCount);
    Serial.print(" - Min: ");
    float minT = frame[0], maxT = frame[0];
    for (int i = 1; i < 768; i++) {
      if (frame[i] < minT) minT = frame[i];
      if (frame[i] > maxT) maxT = frame[i];
    }
    Serial.print(minT, 1);
    Serial.print("C, Max: ");
    Serial.print(maxT, 1);
    Serial.println("C");
    lastDebugTime = millis();
  }
  
  // Output format: START,temp1,temp2,...,temp768,END
  Serial.print("START,");
  for (int i = 0; i < 32 * 24; i++) {
    Serial.print(frame[i], 2);
    if (i < 32 * 24 - 1) {
      Serial.print(",");
    }
  }
  Serial.println(",END");
}