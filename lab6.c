/**
 * This program reads a WAV audio file and prints statistics about the audio samples. The file name 
 * is provided using command line arguments. If the file name is not provided or the file is not readable, 
 * the program will exit and provide an error message.
 *
 * @author Bobby Wiles {@literal <pluf@wfu.edu>}
 * @date April 4, 2021
 * @assignment Lab 6  
 * @course CSC 250
 **/

#include "get_wav_args.h"
#include <math.h>  
#include <stdio.h> 
#include <stdlib.h>  
#include <string.h>  


int read_wav_header(FILE* wav_file, FILE* new_wav_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, short *num_channels_ptr);
int read_wav_data(FILE* wav_file, FILE* new_wav_file, char text_file_name[], char new_wav_file_name[], short sample_size, int num_samples, short num_channels, int bit);


int main(int argc, char *argv[]) 
{
    FILE *wav_file; 
    FILE *new_wave_file;   /* WAV file */
    short sample_size = 0;  /* size of an audio sample (bits) */
    int sample_rate = 0;    /* sample rate (samples/second) */
    int num_samples = 0;    /* number of audio samples */ 
    int wav_ok = 0;     /* 1 if the WAV file si ok, 0 otherwise */
    short num_channels = 0;
    int bit;
    char wav_file_name[256];
    char text_file_name[256];
    char new_wave_file_name[256];
    int args_ok;

    args_ok = get_wav_args(argc, argv, &bit, wav_file_name, text_file_name);

    if (!args_ok) {
        printf("Not good args.\n");
        return 1;
    }

    if (strcmp((wav_file_name + strlen(wav_file_name) - 4), ".wav")) {
        printf("wav file %s missing \".wav\" \n", wav_file_name);
        return 0;
    }

    strcpy(new_wave_file_name, wav_file_name);
    strcpy((new_wave_file_name + strlen(new_wave_file_name) - 4), "_msg.wav");

    /*printf("bit %d \n", bit);
    printf("wav %s \n", wav_file_name);
    printf("new_wav %s \n", new_wave_file_name);
    printf("text %s \n", text_file_name);*/


    wav_file = fopen(wav_file_name, "rbe"); 
    if (!wav_file) {
        printf("could not open wav file %s \n", wav_file_name);
        return 2;
    }

    new_wave_file = fopen(new_wave_file_name, "wbe");
    if (!new_wave_file) {
        printf("could not create wav file %s \n", new_wave_file_name);
        return 2;
    }

    wav_ok = read_wav_header(wav_file, new_wave_file, &sample_size, &num_samples, &sample_rate, &num_channels);
    if (!wav_ok) {
       printf("wav file %s has incompatible format \n", argv[1]);   
       return 3;
    }
    
    read_wav_data(wav_file, new_wave_file, text_file_name, new_wave_file_name, sample_size, num_samples, num_channels, bit);

    if (wav_file) {
        fclose(wav_file);
    } 
    if (new_wave_file) {
        fclose(new_wave_file);
    }

    return 0;
}


/**
 *  function reads the RIFF, fmt, and start of the data chunk. 
 */
int read_wav_header(FILE* wav_file, FILE* new_wav_file, short *sample_size_ptr, int *num_samples_ptr, int *sample_rate_ptr, short *num_channels_ptr) {
    char chunk_id[] = "    ";  /* chunk id, note initialize as a C-string */
    char data[] = "    ";      /* chunk data */
    int chunk_size = 0;        /* number of bytes remaining in chunk */
    short audio_format = 0;    /* audio format type, PCM = 1 */
    short num_channels = 0;    /* number of audio channels */ 
    int byte_rate = 0;
    short block_align_rate = 0;

    /* first chunk is the RIFF chunk, let's read that info */  
    fread(chunk_id, 4, 1, wav_file);
    fwrite(chunk_id, 4, 1, new_wav_file);
    fread(&chunk_size, 4, 1, wav_file);
    fwrite(&chunk_size, 4, 1, new_wav_file);
    fread(data, 4, 1, wav_file);
    fwrite(data, 4, 1, new_wav_file);
    /*printf("chunk: %s, size: %d \n", chunk_id, chunk_size);
    printf("  data: %s \n", data);*/

    /* let's try to read the next chunk, it always starts with an id */
    fread(chunk_id, 4, 1, wav_file);
    /* if the next chunk is not "fmt " then let's skip over it */  
    while (strcmp(chunk_id, "fmt ") != 0) {
        fread(&chunk_size, 4, 1, wav_file);
        /* skip to the end of this chunk */  
        fseek(wav_file, chunk_size, SEEK_CUR);
        /* read the id of the next chuck */  
        fread(chunk_id, 4, 1, wav_file);
    }
    fwrite(chunk_id, 4, 1, new_wav_file);

    /* if we are here, then we must have the fmt chunk, now read that data */  
    fread(&chunk_size, 4, 1, wav_file);
    fwrite(&chunk_size, 4, 1, new_wav_file);

    fread(&audio_format, sizeof(audio_format), 1, wav_file);
    fwrite(&audio_format, sizeof(audio_format), 1, new_wav_file);

    fread(&num_channels, sizeof(num_channels), 1, wav_file);
    fwrite(&num_channels, sizeof(num_channels), 1, new_wav_file);

    fread(&*sample_rate_ptr, sizeof(*sample_rate_ptr), 1, wav_file);
    fwrite(&*sample_rate_ptr, sizeof(*sample_rate_ptr), 1, new_wav_file);

    fread(&byte_rate, sizeof(byte_rate), 1, wav_file);
    fwrite(&byte_rate, sizeof(byte_rate), 1, new_wav_file);

    fread(&block_align_rate, sizeof(block_align_rate), 1, wav_file);
    fwrite(&block_align_rate, sizeof(block_align_rate), 1, new_wav_file);

    fread(&*sample_size_ptr, sizeof(*sample_size_ptr), 1, wav_file);
    fwrite(&*sample_size_ptr, sizeof(*sample_size_ptr), 1, new_wav_file);

    /* you'll need more reads here, hear? */  

    /*printf("chunk: %s, size: %d \n", chunk_id, chunk_size);
    printf("  audio_format: %d \n", audio_format);*/

    /* read the data chunk next, use another while loop (like above) */
    /* visit http://goo.gl/rxnHB1 for helpful advice */

    fread(&chunk_id, 4, 1, wav_file);

    while (strcmp(chunk_id, "data") != 0) {
        fread(&chunk_size, 4, 1, wav_file);
        fseek(wav_file, chunk_size, SEEK_CUR);
        fread(chunk_id, 4, 1, wav_file);
    }
    fwrite(chunk_id, 4, 1, new_wav_file);

    fread(&chunk_size, 4, 1, wav_file);
    fwrite(&chunk_size, 4, 1, new_wav_file);
     
    *num_samples_ptr = chunk_size;

    *num_channels_ptr = num_channels;
    return (audio_format == 1);

    fclose(wav_file);
    fclose(new_wav_file);
    exit(1);
}


/**
 *  function reads the WAV audio data (last part of the data chunk) and copy to new file with message
 */
int read_wav_data(FILE* wav_file, FILE* new_wav_file, char text_file_name[], char new_wav_file_name[], short sample_size, int num_samples, short num_channels_ptr, int bit) {
    int i = 0;
    short sample_size_bytes = (sample_size/8);
    unsigned int sample = 0;
    char ch;
    char temp_ch;
    unsigned int mask = 0;
    unsigned int ch_mask = 0;
    unsigned int mask4 = 0x0f;
    unsigned int mask2 = 0x03;
    unsigned int mask1 = 0x01;
    FILE* text_file;
    int counter = 0;
    int letterWriteCount = 0;
    int trigger_overflow = 0;
    int smileCounter = 0;

    num_samples = num_samples/(num_channels_ptr*(sample_size/8));
    if (bit == 1) {
        ch_mask = mask1;
        mask = 0x80;
    } else if (bit == 2) {
        ch_mask = mask2;
        mask = 0xc0;
    } else if (bit == 4) {
        ch_mask = mask4;
        mask = 0xf0;
    }

    text_file = fopen(text_file_name, "re");
    if (!text_file) {
        printf("text file %s not opened", text_file_name);
        return 1;
    }

    ch = fgetc(text_file);
    while (ch != EOF && !trigger_overflow && smileCounter <= 2) {
        for (i = 0; i < 8/bit; i++) {
            temp_ch = ch_mask & (ch >> (8 - bit *(i + 1)));
            fread(&sample, sample_size_bytes, 1, wav_file);
            sample = (mask & sample) | temp_ch;
            fwrite(&sample, sample_size_bytes, 1, new_wav_file);
            counter += 1;
        }
        ch = fgetc(text_file);
        if (num_samples-(8/bit) <= counter) {
            trigger_overflow = 1;
        }
        if (ch == ':' || ch == ')') {
            smileCounter += 1;
        }
        letterWriteCount += 1;
    }

    if (smileCounter != 2) {
        ch = '\n';
        for (i = 0; i < 8/bit; i++) {
            temp_ch = ch_mask & (ch >> (8 - bit *(i + 1)));
            fread(&sample, sample_size_bytes, 1, wav_file);
            sample = (mask & sample) | temp_ch;
            fwrite(&sample, sample_size_bytes, 1, new_wav_file);
            counter += 1;
        }
        letterWriteCount += 1;

        /* Add in the :) face */
        ch = ':';
        for (i = 0; i < 8/bit; i++) {
            temp_ch = ch_mask & (ch >> (8 - bit *(i + 1)));
            fread(&sample, sample_size_bytes, 1, wav_file);
            sample = (mask & sample) | temp_ch;
            fwrite(&sample, sample_size_bytes, 1, new_wav_file);
            counter += 1;
        }
        letterWriteCount += 1;
        ch = ')';
        for (i = 0; i < 8/bit; i++) {
            temp_ch = ch_mask & (ch >> (8 - bit *(i + 1)));
            fread(&sample, sample_size_bytes, 1, wav_file);
            sample = (mask & sample) | temp_ch;
            fwrite(&sample, sample_size_bytes, 1, new_wav_file);
            counter += 1;
        }
        letterWriteCount += 1;
    }


    /* Finish copying the wav file over */
    while (counter < num_samples) {
        fread(&sample, sample_size_bytes, 1, wav_file);
        fwrite(&sample, sample_size_bytes, 1, new_wav_file);
        counter += 1;
    }

    printf("%d characters written to %s\n", letterWriteCount, new_wav_file_name);
    
   return 1;
}
