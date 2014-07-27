#ifndef _MIDI_H
#define _MIDI_H

typedef enum {
	NoteOff       = 0x8,
	NoteOn        = 0x9,
	PolyPressure  = 0xa,
	CC            = 0xb,
	ProgramChange = 0xc,
	Aftertouch    = 0xd,
	PitchBend     = 0xe
} midi_event_t;


typedef union {
	struct {
		u32 ALL;
	};
	struct {
		u8 cin_cable;
		u8 evnt0;
		u8 evnt1;
		u8 evnt2;
	};
	struct {
		u8 type:4;
		u8 cable:4;
		u8 chn:4; 
		u8 event:4; 
		u8 value1;
		u8 value2;
	};
	struct {
		u8 cin:4;
		u8 dummy1_cable:4;
		u8 dummy1_chn:4;  
		u8 dummy1_event:4; 
		u8 note:8;
		u8 velocity:8;
	};
	struct {
		u8 dummy2_cin:4;
		u8 dummy2_cable:4;
		u8 dummy2_chn:4; 
		u8 dummy2_event:4; 
		u8 cc_number:8;
		u8 value:8;
	};
} midi_package_t;

#endif /* _MIOS32_MIDI_H */

