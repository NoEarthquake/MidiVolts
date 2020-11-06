/*
  MidiVolts.h - Library for converting Midi to CV.
*/

#ifndef MidiVolts_h
#define MidiVolts_h

#include "Arduino.h"
#include <Wire.h>

#define UNSET -1

class MidiVolts
{
  public:
    MidiVolts(int gatePin, int tcaSlot);
    void NoteOn(int midiNum); // assigns voltage for DAC, and turns gate pin on
    void NoteOff(); // turns assigned gate pin off
    void VelocityOn(byte amount); // assigns velocity for DAC
    int VelocityPin = UNSET; // assign velocity pin to control DAC
    void Bend(int amount, int semitonesUp, int semitimesDown); // pitch bends notes based on number of semitones
    void CC(byte amount); // assigns value for specified CC
    void SendToDAC(uint16_t resolution, uint8_t slot); // sends data to specified DAC
    bool noteState = false; // if true then note on
    int MidiNum; // tracks current Midi Number
    double RefVoltage = 5; //Voltage for calculating resolution

    double PitchVoltage; // tracks PitchVoltage

    double Offset = 0;
    double Gain = 1;

  private:
    const int _lowestMIDINote = 36; // adjusts MIDI range

    int _gatePin;
    int _tcaSlot;

    //uint16_t _resolution; // tracks resolution
    int16_t _bendedResolution; // tracks amount of pitch bend

    double _fo; // frequency original - auto calculated on startup based on _lowestMIDINote
    double GetFrequency(int midiNum);
    int16_t GetResolution(double frequency);
};

#endif
