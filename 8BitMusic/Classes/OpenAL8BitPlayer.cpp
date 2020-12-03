//
//  OpenAL8BitPlayer.cpp
//  8BitMusic
//
//  Created by SuXinDe on 2020/12/3.
//  Copyright © 2020 Skyprayer Studio. All rights reserved.
//

#include "OpenAL8BitPlayer.hpp"

#include <OpenAL/OpenAL.h>
#include <math.h>
#include <stdio.h>
#include <vector>

#include "DataDefs.h"

using namespace std;

int max(int a, int b) {
    return (a >= b ? a : b);
}

#define PI 3.1415926f

bool InitOpenAL() {
    ALCdevice* pDevice = alcOpenDevice(NULL);
    ALCcontext* pContext = alcCreateContext(pDevice, NULL);
    
    if (!pContext)
        return false;
    
    alcMakeContextCurrent(pContext);
    
    return true;
}

void ShutdownOpenAL() {
    ALCcontext* pContext = alcGetCurrentContext();
    ALCdevice* pDevice = alcGetContextsDevice(pContext);
    
    alcMakeContextCurrent(NULL);
    alcDestroyContext(pContext);
    alcCloseDevice(pDevice);
}


float GetBaseHz(char major, char shift, int v, int vshift) {
    int Do = 0;
    
    int Bias[7] = { 0, 2, 4, 5, 7, 9, 11 };
    
    switch (major) {
        case 'C':
            Do = Bias[0];
            break;
        case 'D':
            Do = Bias[1];
            break;
        case 'E':
            Do = Bias[2];
            break;
        case 'F':
            Do = Bias[3];
            break;
        case 'G':
            Do = Bias[4];
            break;
        case 'A':
            Do = Bias[5];
            break;
        case 'B':
            Do = Bias[6];
        default:
            break;
    }
    
    if (shift == '#') {
        Do++;
    } else if (shift == 'b') {
        Do--;
    }
    
    int id = Do + Bias[(v + 7) % 7] + 12 * (4 + (v >= 0 ? (v / 7) : (v / 7 - 1))) + vshift;
    printf("%d\n", id );
    int line = id / 12 - 4;
    int pos = id % 12;
    
    if (line >= 0) {
        return hzList[pos] * pow(2, line);
    } else {
        return hzList[pos] / pow(2, -line);
    }
}

char Major[15] = { 'C', 'G', 'D', 'A', 'E', 'B', 'F', 'C', 'G', 'D', 'A', 'E', 'B', 'F', 'C' };
char Shift[15] = { 'b', 'b', 'b', 'b', 'b', 'b', '\0', '\0', '\0', '\0', '\0', '\0', '#', '#' };
int Pos[15] = { -1, 2, -1, 2, 1, 3,1,-1, 2,-1,2,1,3,1,-1 };
bool Line[15] = { true, true, false, false, true, true, false, true, true, false, false, true, true, false, true };

int GetV(int v, int vl, int pos, int line) {
    return ((v < 0 ? v + 1 : v) * 2 + (vl ? 1 : 2)) - ((pos < 0 ? pos + 1 : pos) * 2 + (line ? 1 : 2));
}

//五线谱转化
float GetHz(int s_b, int pos, bool line, int shift) {
    if (pos == 0) {
        return 0.0f;
    }
    return GetBaseHz(Major[s_b + 7], Shift[s_b + 7], GetV(pos, line, Pos[s_b + 7], Line[s_b + 7]),  shift);
}

//简谱转化
float GetHz(char major, char shift, int v, int l, int vshift) {
    if (v <= 0 || v > 7) {
        return 0.0f;
    }
    return GetBaseHz(major, shift, v - 8 + 7 * l, vshift);
}

typedef float(*scale_down_func)(float, float, float);


float PowScaleDown(float s, float l, float t) {
    s = powf(max(1.0f - fabs(s), 0.0f), 6.0f);
    return s;
}


float PowSinScaleDown(float s, float l, float t) {
    s = powf(max(1.0f - fabs(s), 0.0f), 16.0f);
    return s * ((sin(t * PI * 2.0f * 20.0f) + 1.0f) * 0.25f + 0.5f);
}

float LinerScaleDown(float s, float l, float t) {
    return 1.0f - min(fabs(l - 0.2f) / 0.3f, 1.0f);
}


float InvPowScaleDown(float s, float l, float t) {
    s = 1.0f - min(fabs(l - 0.2f) / 0.3f, 1.0f);
    return powf(s, 0.25f);
}

float InvPowSinScaleDown(float s, float l, float t) {
    s = 1.0f - min(fabs(l - 0.4f) / 0.3f, 1.0f);
    return powf(s, 0.25f) * ((sin(t * PI * 2.0f * 7.0f) + 1.0f) * 0.5f * t + (1.0f - t));
}

float RectScaleDown(float s, float l, float n) {
    if (l < 1.0)
        return 1.0f;
    return 0.0f;
}

scale_down_func funcs[6] = {
    &PowScaleDown,
    &PowSinScaleDown,
    &LinerScaleDown,
    &InvPowScaleDown,
    &InvPowSinScaleDown,
    &RectScaleDown
};

void Build(float hz,
           float freq,
           float time,
           float speed,
           vector<short>& buffer,
           int start,
           int scaleType) {
    float max_round = hz * PI * 2.0f;
    
    ALsizei size = freq * time / (speed / 60.0f);
    
    for (int e = 0; e < (max(size, freq) * 2); e++) {
        float v = 0.0f;
        float s = (float)(e - freq / 50) / (max(size, freq) * 2);
        float l = (float)e / size;
        
        for (int h = 0; h < hm_count; h++) {
            float t = ((float)e * (h + 1)) / freq * max_round;
            v += sin(t) * hm[h];
        }
        
        v /= 2.5f;
        v *= 0.7f;
        s = funcs[scaleType](s, l, e / freq);
        v *= s;
        
        if (buffer.size() <= start + e)
            buffer.push_back(v * 0x7fff);
        else
            buffer[start + e] = buffer[start + e] / 2 + v * 0x7fff;
    }
}


void GetLine(FILE* fp, char* buf) {
    int n = 0;
    while (!feof(fp)) {
        char c = fgetc(fp);
        
        if (c == '\r' || c == '\n') {
            break;
        }
        buf[n++] = c;
    }
    buf[n] = 0;
}


char* GetSym(char* line, char* sym) {
    int len = strlen(line);
    
    int n = 0;
    char* cur = line;
    for (; *cur != 0; cur++) {
        sym[n++] = *cur;
        if (*cur == ';') {
            cur++;
            break;
        }
    }
    
    sym[n] = 0;
    return cur;
}





int OpenAL8BitPlayer::play(const char *filePath) {
    FILE* fp = fopen(filePath, "rb");
    
    if (!fp) {
        return -1;
    }
    
    int type = 0;
    char majorStr[16];
    float speed = 80.0f;
    int scaleFunc = 0;
    
    
    fscanf(fp, "%d", &type);
    fscanf(fp, "%S", &majorStr);
    fscanf(fp, "%f", &speed);
    fscanf(fp, "%d", &scaleFunc);
    
    int s_b = 0;
    if (type == 1) {
        for (int n = 0; n < strlen(majorStr); n++) {
            if (majorStr[n] == '#') {
                s_b++;
            } else if (majorStr[n] == 'b') {
                s_b--;
            }
        }
    }
    
    if (type != 0 && type != 1) {
        return -1;
    }
    bool suc = InitOpenAL();
    if (!suc)
        return 0;
    
    ALsizei freq = 8000;
    ALint loop = false;
    ALuint Source;
    ALuint Buffer;
    
    
    vector<short> buffer;
    
    int start = 0;
    
    while (!feof(fp)) {
        char line[512];
        GetLine(fp, line);
        
        char* cur = line;
        
        if (strlen(line) > 0) {
            float time = 0.0f;
            
            for (int s = 0; s < 10; s++) {
                char sym[256];
                cur = GetSym(cur, sym);
                
                int v = 0;
                int l = 0;
                float t = 0.0f;
                int shift = 0;
                float hz = 0.0f;
                
                if(type == 0){
                    sscanf(sym, "%d,%d,%f,%d", &l, &v, &t, &shift);
                    hz = GetHz(majorStr[0], majorStr[1], v, l, shift);
                }
                else if (type == 1) {
                    sscanf(sym, "%d,%d,%f,%d", &v, &l, &t, &shift);
                    hz = GetHz(s_b, v, l == 1, shift);
                }
                
                if (t <= 0.0f)
                    break;
                
                time = max(time, t);
                
                Build(hz, freq, t, speed, buffer, start, scaleFunc);
            }
            
            start += freq * time / (speed / 60.0f);
        }
    }
    
    alGenBuffers(1, &Buffer);
    alGenSources(1, &Source);
    
    alBufferData(Buffer, AL_FORMAT_MONO16, buffer.data(), buffer.size() * sizeof(short), freq);
    
    alSourcei(Source, AL_BUFFER, Buffer);
    alSourcei(Source, AL_LOOPING, loop);
    
    alSourcePlay(Source);
    
    ALint state = AL_PLAYING;
    while (state == AL_PLAYING) {
        alGetSourcei(Source, AL_SOURCE_STATE, &state);
    }
    
    alDeleteBuffers(1, &Buffer);
    alDeleteSources(1, &Source);
    
    ShutdownOpenAL();
    return 0;
}

