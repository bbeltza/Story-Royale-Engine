#include <Core/Audio.h>
#include <Base/AudioChunk.h>
#include <Entry.h>

#include <utils/math.h>

struct
{
    int t;
} data = {
    .t = 0
};

double sawtooth(double x)
{
    double mod = fmod(x, UT_PI * 2);
    return 1 - mod / UT_PI; 
}

int audio_callback(void* userdata, sre_u8* _samples, size_t size)
{
    size >>= 1;
    sre_s16* samples = (sre_s16*)_samples;
    int* t = userdata;

    static const int speed = 9000;
    static const int freqs = 5;

    static int freq1 = -2;
    static int freq2 = 1;
    static int freq3 = 5;
    static int freq4 = 8;
    static int freq5 = 12;

    if (freq1 < 20)
    {
        freq1 = (int)(pow(2, freq1/12.0)*440) - 1;
        freq2 = (int)(pow(2, freq2/12.0)*440) + 1;
        freq3 = (int)(pow(2, freq3/12.0)*440);
        freq4 = (int)(pow(2, freq4/12.0)*440);
        freq5 = (int)(pow(2, freq5/12.0)*440);
    }

    for (size_t i = 0; i < size; i += 2)
    {
        const double tt = sre_audiofreqratio(*t);
        double v;
        v = 1 - *t / (double)speed * tt;
        v /= freqs;

        samples[i] = (sre_s16)(sawtooth(tt * UT_PI * freq1) * 0x7FFF * v);
        samples[i] += (sre_s16)(sawtooth(tt * UT_PI * freq2) * 0x7FFF * v);
        samples[i] += (sre_s16)(sawtooth(tt * UT_PI * freq3) * 0x7FFF * v);
        samples[i] += (sre_s16)(sawtooth(tt * UT_PI * freq4) * 0x7FFF * v);
        samples[i] += (sre_s16)(sawtooth(tt * UT_PI * freq5) * 0x7FFF * v);
        samples[i + 1] = samples[i];

        (*t)++;
        *t = *t % speed;
    }

    return 128;
}

void sre_initialize()
{
    sre_audioconfigure(false, 9000); // what? This app makes a bunch of noises but it wasn't the case the last time I ran it...
    sre_audiocallbackqueue(audio_callback, &data);
}