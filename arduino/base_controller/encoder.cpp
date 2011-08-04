#include <WProgram.h>
#include "encoder.h"

volatile long encoder0Count = 0L;

/* Callback for encoder on interrupt 0
   B channel = pin 2 / interrupt 0
   A channel = pin 12 / PB4
 */
void encoder0Handler() {
  // hard code the port instead of digitalRead for execution speed
  if (PINB & (1 << PB4)) {
    encoder0Count -= 1;
  } else {
    encoder0Count += 1;
  }
}

void encoder_initialize() {
  // Set pins 2 and 12 as input, no pull ups
  pinMode(2, INPUT);
  digitalWrite(2, LOW);
  pinMode(12, INPUT);
  digitalWrite(12, LOW);

  // Interrupt to count encoder ticks on int 0 (pin 2)
  attachInterrupt(0, encoder0Handler, RISING);
}

long encoder_getCount() {
  long tmpEncoderCount;

  // Atomically read encoder count
  cli();
  tmpEncoderCount = encoder0Count;
  sei();

  return tmpEncoderCount;
}