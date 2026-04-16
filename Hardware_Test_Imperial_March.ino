#include <driver/i2s.h>
#include <math.h>

// Your I2S Pin Definitions
#define I2S_BCLK       15  // Bit Clock (BCLK)
#define I2S_LRC        16  // Left/Right Clock (LRC / WS)
#define I2S_DOUT       17  // Data Out (DIN)

// I2S Configuration
#define I2S_PORT       I2S_NUM_0
#define SAMPLE_RATE    44100
#define AMPLITUDE      500 // Safe volume level

// Musical Notes (Frequencies in Hz)
#define NOTE_G4  392.00
#define NOTE_EB4 311.13
#define NOTE_BB4 466.16
#define NOTE_D5  587.33
#define NOTE_EB5 622.25
#define NOTE_FS4 369.99
#define NOTE_G5  783.99
#define REST     0.0

// Star Wars Imperial March Melody
float melody[] = {
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_EB4, NOTE_BB4,
  NOTE_G4, NOTE_EB4, NOTE_BB4, NOTE_G4, REST,
  NOTE_D5, NOTE_D5, NOTE_D5, NOTE_EB5, NOTE_BB4,
  NOTE_FS4, NOTE_EB4, NOTE_BB4, NOTE_G4, REST
};

// Note Durations (in milliseconds)
int durations[] = {
  500, 500, 500, 350, 150,
  500, 350, 150, 1000, 500,
  500, 500, 500, 350, 150,
  500, 350, 150, 1000, 2000
};

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing I2S DAC Melody test...");

  // 1. Configure the I2S driver (with our fixed buffers)
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, 
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, 
    .dma_buf_count = 16,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = true 
  };

  // 2. Map the I2S pins to your ESP32-S3
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE 
  };

  // 3. Install and start the I2S driver
  if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println("Failed to install I2S driver!");
    return;
  }
  i2s_set_pin(I2S_PORT, &pin_config);

  Serial.println("I2S running! Playing Melody...");
}

// Function to play a specific frequency for a set time
void playTone(float frequency, int duration_ms) {
  size_t bytes_written;
  int total_samples = (duration_ms / 1000.0) * SAMPLE_RATE;
  int16_t sample_buffer[2];

  for (int i = 0; i < total_samples; i++) {
    if (frequency == 0.0) {
      // If it's a REST, play silence
      sample_buffer[0] = 0;
      sample_buffer[1] = 0;
    } else {
      // Calculate the sine wave for the tone
      float time = (float)i / SAMPLE_RATE;
      int16_t sample_val = (int16_t)(AMPLITUDE * sin(TWO_PI * frequency * time));
      sample_buffer[0] = sample_val;
      sample_buffer[1] = sample_val;
    }
    // Write audio data to the DAC
    i2s_write(I2S_PORT, &sample_buffer, sizeof(sample_buffer), &bytes_written, portMAX_DELAY);
  }

  // Add a tiny 20ms silent gap between notes so they don't blur together
  sample_buffer[0] = 0; sample_buffer[1] = 0;
  int gap_samples = (20 / 1000.0) * SAMPLE_RATE;
  for (int i = 0; i < gap_samples; i++) {
    i2s_write(I2S_PORT, &sample_buffer, sizeof(sample_buffer), &bytes_written, portMAX_DELAY);
  }
}

void loop() {
  int num_notes = sizeof(melody) / sizeof(melody[0]);

  // Loop through the arrays and play each note
  for (int i = 0; i < num_notes; i++) {
    playTone(melody[i], durations[i]);
  }
  
  Serial.println("Melody finished. Restarting...");
}
