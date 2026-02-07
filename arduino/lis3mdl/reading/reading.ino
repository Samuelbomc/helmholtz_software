/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low magnetometer data registers.
They can be converted to units of gauss using the
conversion factors specified in the datasheet for your particular
device and full scale setting (gain).

Example: An LIS3MDL gives a magnetometer X axis reading of 1292 with its
default full scale setting of +/- 4 gauss. The GN specification
in the LIS3MDL datasheet (page 8) states a conversion factor of 6842
LSB/gauss (where LSB means least significant bit) at this FS setting, so the raw
reading of 1292 corresponds to 1292 / 6842 = 0.1888 gauss.
*/

#include <Wire.h>
#include <LIS3MDL.h>

LIS3MDL mag;

char report[80];

// (Max + Min) / 2
const float mag_offset_x = (0.0 + 0.0) / 2;
const float mag_offset_y = (0.0 + 0.0) / 2;
const float mag_offset_z = (0.0 + 0.0) / 2;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!mag.init())
  {
    Serial.println("Failed to detect and initialize magnetometer!");
    while (1);
  }

  mag.enableDefault();
}

void loop()
{
  mag.read();

  // Subtract the offset from the raw sensor reading.
  float corrected_x = mag.m.x - mag_offset_x;
  float corrected_y = mag.m.y - mag_offset_y;
  float corrected_z = mag.m.z - mag_offset_z;

  // Transform to Gauss, then nano Teslas
  float mag_x_nT = (corrected_x / 6842.0) * 100000.0;
  float mag_y_nT = (corrected_y / 6842.0) * 100000.0;
  float mag_z_nT = (corrected_z / 6842.0) * 100000.0;

  snprintf(report, sizeof(report), "%.2f, %.2f, %.2f",
    mag_x_nT, mag_y_nT, mag_z_nT);
  Serial.println(report);

  delay(100);
}
