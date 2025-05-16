#include "config.h"

float retunLDRIntensity() {
  int rawValue = analogRead(LDR_AO); // ADC value: 32–4063
  int min = 32;
  int max = 4063;

  // Normalized and inverted value
  float intensity = 1.00000f - (float)(rawValue - min) / (float)(max - min);

  // Clamp between 0 and 1
  if (intensity < 0) intensity = 0;
  if (intensity > 1) intensity = 1;

  return intensity;
}