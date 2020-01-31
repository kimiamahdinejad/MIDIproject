#include <stdio.h>

int note , tempo , Count;

double FigureFreq(int NoteIndex)
{
	float freq[] = { 16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,16.35,27.50,29.14,30.87,32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,51.91,55.00,58.27,61.74,65.41,69.30,73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47,130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,207.65,220.00,233.08,246.94,261.63,277.18,293.66,311.13,329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88,523.25,554.37,587.33,622.25,659.25,698.46,739.99,783.99,830.61,880.00,932.33,987.77,1046.50,1108.7,1174.6,1244.5,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53,2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07,4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13 };
	
	return freq[NoteIndex - 12];
}

int ReadDeltaTime(int ByteCount, FILE* pointer)
{
	unsigned long value;
	unsigned char c;

	if ((value = fgetc(pointer)) & 0x80)
	{
		value &= 0x7F;
		ByteCount++;
		
		do
		{
			value = (value << 7) + ((c = getc(pointer)) & 0x7F);
			ByteCount++;
		} while (c & 0x80);
	}
	
	Count = ByteCount;
	
	return (value);
}

double FigureDuration(int tempo, int division, int delta)
{
	int variable;
	double duration;

	variable = (60 * 1000000) / tempo;

	variable *= division;

	duration = 60000 / variable;

	duration *= delta;
	
	return duration;
}

int FigureEvent(int ByteCount, FILE* pointer)
{
	int i;
	unsigned char event, temp[3];
	unsigned int length;

	fread(&event, 1, 1, pointer);

	if (event >= 0x80 && event < 0x90)
	{
		fread(&note, 1, 1, pointer);
		fread(temp, 1, 1, pointer);
		
		ByteCount += 2;
		
		Count = ByteCount;

		return 89;
	}
	else if (event >= 0x90 && event < 0xA0)
	{
		fread(&note, 1, 1, pointer);
		fread(temp, 1, 1, pointer);

		ByteCount += 2;
		
		Count = ByteCount;
	}
	else if (event >= 0xA0 && event < 0xC0)
	{
		fread(temp, 2, 1, pointer);

		ByteCount += 2;
		
		Count = ByteCount;
	}
	else if (event >= 0xC0 && event < 0xE0)
	{
		fread(temp, 1, 1, pointer);

		ByteCount++;
		
		Count = ByteCount;
	}
	else if (event >= 0xE0 && event < 0xF0)
	{
		fread(temp, 2, 1, pointer);

		ByteCount += 2;
		
		Count = ByteCount;
	}
	else if (event == 0xF0 || event == 0xF7)
	{
		fread(&length, 1, 1, pointer);
		fread(temp, length, 1, pointer);

		ByteCount += 1 + length;
		
		Count = ByteCount;
	}
	else if (event == 0xFF)
	{
		fread(&event, 1, 1, pointer);

		if (event == 0x51)
		{
			fread(&length, 1, 1, pointer);

			for (i = 0; i < length; i++)
			{
				fread(temp, 1, 1, pointer);
			}
			
			tempo = temp[0] << 16 | temp[1] << 8 | temp[2];

			ByteCount += 2 + length;
			
			Count = ByteCount;
		}
		else if (event == 0x2f)
		{
			fread(temp, 1, 1, pointer);
			
			ByteCount += 2;
			
			Count = ByteCount;

			return 1;
		}
		else
		{
			fread(&length, 1, 1, pointer);
			fread(temp, length, 1, pointer);

			ByteCount += 2 + length;
			
			Count = ByteCount;
		}
	}
	else
	{
		return 0;
	}
	
	return 2;
}

int FigureNote ()
{
	return note;
}

int FigureTempo ()
{
	return tempo;
}

int ChangeEndian(int digit)
{
	return (((digit >> 24) & 0x000000ff | ((digit >> 8) & 0x0000ff00) | ((digit << 8) & 0x00ff0000) | ((digit << 24) & 0xff000000)));
}

int ReturnByteCount()
{
	return Count;
}
