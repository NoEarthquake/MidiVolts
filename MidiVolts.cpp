/*
  MidiVolts.h - Library for converting Midi to CV.
*/

#include "Arduino.h"
#include <Wire.h>
#include "MidiVolts.h"

#define tcaAddress 0x70
#define dacAddress 0x62

MidiVolts::MidiVolts(int gatePin, int tcaSlot)
{
  pinMode(gatePin, OUTPUT);
  _gatePin = gatePin;
  _tcaSlot = tcaSlot;

  //auto calculating frequency original
  double a = pow(2, (double)1 / (double)12);
  _fo = (double)8.175798916 * pow(a, (double)_lowestMIDINote);
}

void MidiVolts::NoteOn(int midiNum)
{
  MidiNum = midiNum;

  if (midiNum >= _lowestMIDINote) // Checks if midi note is in range
  {
    double freq = GetFrequency(midiNum);
    int16_t resolution = GetResolution(freq) + _bendedResolution; // gets resolution plus any pitch bend

    if (resolution > 4095) // verifies resolution is within min max constraints 0 - 4095
    {
      resolution = 4095;
    }
    else if (resolution < 0)
    {
      resolution = 0;
    }

    SendToDAC(resolution, _tcaSlot);

    if (midiNum >= _lowestMIDINote && midiNum <= (_lowestMIDINote + 60))
    {
      digitalWrite(_gatePin, HIGH);
      noteState = true;
    }
  }
  else
  {
    digitalWrite(_gatePin, LOW);
    noteState = false;
  }
}

void MidiVolts::NoteOff()
{
  digitalWrite(_gatePin, LOW);
  noteState = false;
}

void MidiVolts::VelocityOn(byte amount)
{
  if (VelocityPin != UNSET)
  {
    uint16_t resolution = (double)amount * (double)4095 / (double)127;
    SendToDAC(resolution, VelocityPin);
  }
}

void MidiVolts::Bend(int amount, int semitonesUp, int semitonesDown)
{
  // 8192 is center, factor by 8191 for non neg numbers

  if (amount > 8192)
  {
    _bendedResolution = round((((double)amount - (double)8192) / (double)8191) * ((double)semitonesUp * (double)68.25));
  }
  else if (amount < 8192)
  {
    _bendedResolution = round((((double)amount - (double)8192) / (double)8192) * ((double)semitonesDown * (double)68.25));
  }
  else
  {
    _bendedResolution = 0;
  }

  double freq = GetFrequency(MidiNum);
  int16_t resolution = GetResolution(freq) + _bendedResolution;

  if (resolution > 4095) // verifies resolution is within min max constraints 0 - 4095
  {
    resolution = 4095;
  }
  else if (resolution < 0)
  {
    resolution = 0;
  }

  SendToDAC(resolution, _tcaSlot);
}

void MidiVolts::CC(byte amount)
{
  uint16_t resolution = (double)amount * (double)4095 / (double)127;
  SendToDAC(resolution, _tcaSlot);
}

double MidiVolts::GetFrequency(int midiNum)
{
  int n = midiNum - _lowestMIDINote;
  double a = pow(2, (double)1 / (double)12);
  double freq = _fo * pow(a, n);

  return freq;
}

int16_t MidiVolts::GetResolution(double frequency)
{
  PitchVoltage = (log( frequency / _fo)) / log(2);

  double volts = Offset + PitchVoltage * Gain; // compensates for gain and offset

  int16_t resolution = (int)round((volts / RefVoltage) * (double)4095); // gets resolution

  return resolution;
}

void MidiVolts::SendToDAC(uint16_t resolution, uint8_t slot)
{
  //Assigns slot for Multiplexer
  Wire.beginTransmission(tcaAddress);
  Wire.write(1 << (uint8_t)slot);
  Wire.endTransmission();

  //Sends Data to DAC
  Wire.beginTransmission(dacAddress);
  Wire.write(0x40);
  Wire.write(resolution / 16);
  Wire.write((resolution % 16) << 4);
  Wire.endTransmission();
}
