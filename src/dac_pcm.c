
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <EGB.h>
#include <SND.h>

// 8bit * 320000hz * mono * 25 sec = 800000
#define PCM_SIZE 800000

char *ework;
char *swork;

void initialize_TBIOS_and_DAC(void)
{
	// EGB(Graphics) initialize
	ework = (char *)malloc(sizeof(char) * 1536);
	EGB_init(ework, 1536);
	EGB_resolution(ework, 1, 5);
	EGB_resolution(ework, 0, 5);
	EGB_writePage(ework, 1);
	EGB_displayStart(ework, 0, 64, 0);
	EGB_displayStart(ework, 2, 2, 2);
	EGB_displayStart(ework, 3, 256, 240);
	EGB_writePage(ework, 0);
	EGB_displayStart(ework, 0, 64, 0);
	EGB_displayStart(ework, 2, 2, 2);
	EGB_displayStart(ework, 3, 256, 240);
	EGB_color(ework, 0, 0x7fff);
	EGB_displayPage(ework, 0, 3);

	// SND(Sound) initialize
	swork = (char *)malloc(sizeof(char) * 16384);
	SND_init(swork);
	SND_elevol_mute(0xff);
	SND_fm_timer_b_set(0, 0); // Stop Timer B

	// Enable YM2612 DAC(FM 6ch OFF)
	SND_fm_write_data(0, 0x2b, 0x80);
}

void end_TBIOS_and_DAC(void)
{
	EGB_init(ework, 1536);
	free(ework);

	// Disable YM2612 DAC(FM 6ch ON)
	SND_fm_write_data(0, 0x2b, 0);

	SND_end();
	free(swork);
}

void main(void)
{
	unsigned char *pcm_data;
	unsigned int pcm_point;
	int count;

	char para[64];

	initialize_TBIOS_and_DAC();

	pcm_data = (unsigned char *)malloc(sizeof(unsigned char) * PCM_SIZE);
	if(pcm_data == NULL)
	{
		end_TBIOS_and_DAC();
		return;
	}
	memset(pcm_data, 0x7f, PCM_SIZE);

	sprintf(para, "      Playing 32KHz PCM on FM TOWNS");
	WORD(para+0) = 8;
	WORD(para+2) = 32;
	WORD(para+4) = 29;
	EGB_sjisString(ework, para);

	sprintf(para, "      with DAC of YM2612");
	WORD(para+0) = 52;
	WORD(para+2) = 52;
	WORD(para+4) = 18;
	EGB_sjisString(ework, para);

	// Load PCM data.
	// like YM2612 DAC data is unsigned 8bit (-1.0=0x0, 0=0x7f ,+1.0=0xff).
	FILE *fp;
	fp = fopen("dac.pcm", "rb");
	if(fp == NULL)
	{
		// Saw Wave 400Hz
		unsigned char data;
		pcm_point = 0;
		for(int i = 0;i < 10000;i++)
		{
			data = 47;
			for(int j = 0;j < 80; j++)
			{
				pcm_data[pcm_point++] = data;
				data += 2;
			}
		}
	}
	else
	{
		fseek(fp, 0, 0);
		fread(pcm_data, sizeof(unsigned char), PCM_SIZE, fp);
		fclose(fp);
	}

	// Disable interrupt
	_disable();

	pcm_point = 0;
	count = 0;

	while(0 != (_inb(0x4d8) & 0x80));
	_outb(0x4d8, 0x2a);

	while(1)
	{
		if(count < 1)
		{
			// Output DAC
			while(0 != (_inb(0x4d8) & 0x80));
			_outb(0x4da, pcm_data[pcm_point]);

			pcm_point++;
			// Loop
			if(pcm_point >= PCM_SIZE )
			{
				pcm_point = 0;
			}

			//// Press the mouse button to EXIT (but heavy for 386 TOWNS)
			/*if( (((_inb(0x04d0)) & (_inb(0x04d2))) & 0x30) != 0x30)
			{
				break;
			}*/

			count++;
		}
		else
		{
			// for Wait
			while(0 != (_inb(0x4d8) & 0x80));
			_outb(0x4d8, 0x2a);

			count = 0;
		}
	}

	// Enable interrupt
	_enable();

	end_TBIOS_and_DAC();
	free(pcm_data);
}