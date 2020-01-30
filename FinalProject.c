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
	char category[5];
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

	printf("Choose your favorite playlist:\n1.Classic\n2.Pop\n3.Movies\n4.Hip-Hop\n5.My Playlist\n");

	scanf("\n%s", music.category);
	
	do
	{
		if (strcmp(music.category, "5") == 0)
		{
			printf("Would you like to make a playlist?(Y/N)\n");
			scanf("\n%c", &MyMusic.existance);

			if (MyMusic.existance == 'Y')
			{
				printf("Enter the name of your playlist:\n");
				scanf("\n%s" , MyMusic.PlaylistName);
				
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
					strcat(MIDIName, MyMusic.extension);

					FILE* file = fopen(MyMusic.PlaylistName, "a+");
					fprintf(file, "%s\n", MIDIName);
					fclose(file);

					printf("Would you like to add another?(Y/N)\n");
					scanf("\n%c", &MyMusic.countinue);
				} while (MyMusic.countinue == 'Y');
			}
			
			printf("Would you like to play your playlist?(Y/N)\n");
			scanf("\n%c" , &MyMusic.countinue);
			
			if (MyMusic.countinue == 'Y')
			{
				strcat(MyMusic.PlaylistName , ".txt");
		
				FILE* file = fopen(MyMusic.PlaylistName, "r");

				printf("choose your favorite track:\n");

				FilePoint = 1;
				while(fscanf(file, "\n%s", MIDIName) != EOF)
				{
					printf("%d: %s\n", FilePoint++, MIDIName);
				}
				fclose(file);

				scanf("%d", &music.TrackNumber);

				FILE* infile = fopen(music.category, "r");

				for (FilePoint = 1; FilePoint <= music.TrackNumber; FilePoint++)
				{
					fscanf(infile, "\n%s", MIDIName);
				}

				fclose(infile);
			}
		}
		else
		{
			strcat(music.category , ".txt");
		
			FILE* file = fopen(music.category, "r");

			printf("choose your favorite track:\n");

			FilePoint = 1;
			while (fscanf(file, "\n%s", MIDIName) != EOF)
			{
				printf("%d: %s\n", FilePoint, MIDIName);
			}
			fclose(file);

			scanf("%d", &music.TrackNumber);

			FILE* infile = fopen(music.category, "r");

			for (FilePoint = 1; FilePoint <= music.TrackNumber; FilePoint++)
			{
				fscanf(infile, "\n%s", MIDIName);
			}

			fclose(infile);
		
			strcat(MIDIName , ".mid");
		}
		ReadMid();

		printf("Choose your favorite playlist:\n1.Classic\n2.Pop\n3.Movies\n4.Hip-Hop\n5.My Playlist\n6.Exit\n");
		scanf("\n%c", &music.category);

	} while (strcmp(music.category , "6"));
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

	for (i = 0; i < HC.track; i++)
	{
		TrackLengthCount = 0;

		/*reading track type*/
		fread(HT.TrackType, 4, 1, pointer);

		/*reading header track length*/
		fread(&HT.length, 4, 1, pointer);
		HT.length = ChangeEndian(HT.length);

		printf("Track %d:\nTrack Type: %s\nTrack Length: %d\n", i, HT.TrackType, HT.length);

		/*reading events*/
		do
		{
			delta = ReadDeltaTime(TrackLengthCount, pointer);
			flag = FigureEvent(TrackLengthCount, pointer);

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

				if (duration == 0)
				{
					printf("Duration = 0\n");

					duration += 20;
				}

				Beep(frequancy, duration * 0.5);
			}
			if (ReturnByteCount() == HT.length)
			{
				if (flag != 1)
				{
					printf("No End of track call Found!\n");
					exit(1);
				}
			}
		} while (flag != 1);

		if (ReturnByteCount() != HT.length)
		{
			printf("Invalid End of track call!\n");
			exit(1);
		}
	}
}

