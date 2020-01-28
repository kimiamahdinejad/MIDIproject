#include<stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "MIDIHelp.h"

struct HeaderChunk
{
	char ChunkType[5];
	int length;
	int format;
	int track;
	int division;
};

struct HeaderTrack
{
	char TrachType[5];
	int length;
};

struct HeaderChunk HC;
struct HeaderTrack HT;

void ReadHeaderChunk(FILE*);
void ReadTracks(FILE*);
int ChangeEndian(int);

int main()
{
	FILE* pointer = fopen("forelise.mid", "rb");
	/*can change the MIDI pointer name if needed*/
	
	/*reading header chunck which is uniqe*/
	ReadHeaderChunk(pointer);

	/*reading every track contains header and events*/
	ReadTracks(pointer);
}

void ReadHeaderChunk(FILE* pointer)
{
	unsigned char Buffer[2];

	/*reading chunk type*/
	fread(HC.ChunkType, 4, 1, pointer);

	/*reading header chunk length*/
	fread(&HC.length, 4, 1, pointer);
	HC.length = ChangeEndian(HC.length);

	/*reading format*/
	fread(Buffer, 2, 1, pointer);
	HC.format = Buffer[1] | Buffer[0] << 8;

	/*reading track*/
	fread(Buffer, 2, 1, pointer);
	HC.track = Buffer[1] | Buffer[0] << 8;

	/*reading division*/
	fread(Buffer, 2, 1, pointer);
	HC.division = Buffer[1] | Buffer[0] << 8;
}

void ReadTracks(FILE* pointer)
{
	int flag, delta, note , tempo , i;
	double frequancy, duration;

	for (i = 0; i < HC.track; i++)
	{
		/*reading track type*/
		fread(HT.TrachType, 4, 1, pointer);

		/*reading header track length*/
		fread(&HT.length, 4, 1, pointer);
		HT.length = ChangeEndian(HT.length);

		/*reading events*/
		do
		{
			delta = ReadDeltaTime(pointer);
			flag = FigureEvent(pointer);

			if (flag == 0)
			{
				exit(1);
			}
			if (flag == 89)
			{
				note = FigureNote();
				tempo = FigureTempo();
				
				frequancy = FigureFreq(note);

				duration = FigureDuration(tempo , HC.division , delta);

				Beep(frequancy, duration * 0.6);
			}
		} while (flag != 1);
	}
}
