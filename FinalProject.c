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
	char TrackType[5];
	int length;
};

struct MyPlaylist
{
	char PlaylistName[50];
	char address[50];
	char name[50];
	char extension[10];
	char countinue;
	char existance;
};

struct Playlist
{
	int category;
	char CM;
	int TrackNumber;
};

struct HeaderChunk HC;
struct HeaderTrack HT;
char MIDIName[200];
int TrackLengthCount;

void ReadMid();
void ReadHeaderChunk(FILE*);
void ReadTracks(FILE*);

void main()
{
	struct Playlist music;
	struct MyPlaylist MyMusic;
	int FilePoint;
	char MIDIPlaylist[50];

	printf("Choose your favorite playlist or make your own:(C/M)\n");
	scanf("\n%c", &music.CM);
	
	do
	{
		if (music.CM == 'M')
		{
			printf("Enter the name of your playlist:\n");
			scanf("\n%s" , MyMusic.PlaylistName);
				
			FILE* point = fopen("playlists.txt", "a+");
			fprintf(point, "\n%s", MyMusic.PlaylistName);
			fclose(point);

			do
			{
				printf("Enter the address of your file:\n");
				scanf("\n%s", MyMusic.address);

				printf("Enter the name of your file:\n");
				scanf("\n%s", MyMusic.name);

				printf("Enter the extension of your file:\n");
				scanf("\n%s", MyMusic.extension);

				if (strcmp(MyMusic.extension, ".mid") != 0)
				{
					printf("Invalid MIDI file!\n");
					exit(1);
				}

				strcat(MIDIName, MyMusic.address);
				strcat(MIDIName, MyMusic.name);
				
				strcat(MyMusic.PlaylistName, ".txt");
				
				FILE* file = fopen(MyMusic.PlaylistName, "a+");
				fprintf(file, "%s\n", MIDIName);
				fclose(file);

				printf("Would you like to add another?(Y/N)\n");
				scanf("\n%c", &MyMusic.countinue);
			} while (MyMusic.countinue == 'Y');
		}
		else
		{
			printf("Choose your favorite playlist:\n");
			
			FILE* list = fopen("playlists.txt", "r");
			
			FilePoint = 1;
			while (fscanf(list, "\n%s", MIDIPlaylist) != EOF)
			{
				printf("%d: %s\t", FilePoint++, MIDIPlaylist);
			}
			fclose(list);
			
			printf("\n");
			
			FILE* choose = fopen("playlists.txt", "r");
			
			scanf("%d", &music.category);

			for (FilePoint = 1; FilePoint <= music.category; FilePoint++)
			{
				fscanf(choose, "\n%s", MIDIPlaylist);
			}
			fclose(choose);

			strcat(MIDIPlaylist, ".txt");
		
			FILE* file = fopen(MIDIPlaylist, "r");

			printf("choose your favorite track:\n");

			FilePoint = 1;
			while (fscanf(file, "\n%s", MIDIName) != EOF)
			{
				printf("%d: %s\n", FilePoint++, MIDIName);
			}
			fclose(file);
			
			scanf("%d", &music.TrackNumber);

			FILE* infile = fopen(MIDIPlaylist, "r");

			for (FilePoint = 1; FilePoint <= music.TrackNumber; FilePoint++)
			{
				fscanf(infile, "\n%s", MIDIName);
			}

			fclose(infile);
		
			strcat(MIDIName , ".mid");
			
			ReadMid();
		}
		printf("Choose your favorite playlist or make your own or exit the program:(C/M/E)\n");

		scanf("\n%c", &music.CM);

	} while (music.CM != 'E');
}

void ReadMid()
{
	FILE* pointer = fopen(MIDIName, "rb");
	
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

	printf("About your choice:\nChunk Type: %s\nHeader Length: %d\nFormat: %d\tTrack: %d\tDivision: %d\n", HC.ChunkType, HC.length, HC.format, HC.track, HC.division);

	if (HC.format == 2)
	{
		printf("Invalid Format\n");
		exit(1);
	}
}

void ReadTracks(FILE* pointer)
{
	int flag, delta, note , tempo , i;
	double frequancy, duration;

	for (i = 1; i <= HC.track; i++)
	{
		
		/*reading track type*/
		fread(HT.TrackType, 4, 1, pointer);

		/*reading header track length*/
		fread(&HT.length, 4, 1, pointer);
		HT.length = ChangeEndian(HT.length);
				
		TrackLengthCount = 0;

		printf("Track %d:\nTrack Type: %s\nTrack Length: %d\n\n", i, HT.TrackType, HT.length);

		/*reading events*/
		do
		{
			delta = ReadDeltaTime(TrackLengthCount, pointer);
			TrackLengthCount = ReturnByteCount();
			
			flag = FigureEvent(TrackLengthCount, pointer);
			TrackLengthCount = ReturnByteCount();
			
			if (flag == 0)
			{
				printf("Invalid Event found!\n");
				exit(1);
			}
			if (flag == 89)
			{
				note = FigureNote();
				tempo = FigureTempo();
				
				frequancy = FigureFreq(note);
				
				if (frequancy < 100)
				{
					printf("_\n");
				}
				else if (frequancy < 200)
				{
					printf("__\n");
				}
				else if (frequancy < 300)
				{
					printf("___\n");
				}
				else if (frequancy < 400)
				{
					printf("____\n");
				}
				else if (frequancy < 500)
				{
					printf("_____\n");
				}
				else if (frequancy < 600)
				{
					printf("______\n");
				}
				else if (frequancy < 700)
				{
					printf("_______\n");
				}
				else if (frequancy < 800)
				{
					printf("________\n");
				}
				else if (frequancy < 900)
				{
					printf("_________\n");
				}
				else
				{
					printf("__________\n");
				}
				
				duration = FigureDuration(tempo , HC.division , delta);

				if (duration == 0)
				{
					printf("Duration = 0\n");

					duration += 20;
				}

				Beep(frequancy, duration * 0.001);
			}
			
			if (TrackLengthCount == HT.length)
			{
				if (flag != 1)
				{
					printf("No End of track call Found!\n");
					exit(1);
				}
			}
		} while (flag != 1);

		if (TrackLengthCount != HT.length)
		{
			printf("Invalid end of track found!\n");
			exit(1);
		}
	}
}
