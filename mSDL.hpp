#ifndef __m_SDL_HPP__
#define __m_SDL_HPP__

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <vector>
#include <string>
#include <set>
using namespace std;

enum color{
    white,
    red,
};

const int ScreenWidth = 900;
const int ScreenHeight = 900;
const float TextureWidth = 100.f;
const float TextureHeight = 100.f; // 修正拼寫錯誤

const vector<string> MirrorPath{
    "element/mirror.png"
};
const vector<string> WhiteNumberPath{
    "element/break.png",
    "element/white1.png",
    "element/white2.png",
    "element/white3.png",
    "element/white4.png",
    "element/white5.png",
    "element/white6.png",
    "element/white7.png",
    "element/white8.png",
    "element/white9.png",
};
const vector<string> RedNumberPath{
    "element/break.png",
    "element/red1.png",
    "element/red2.png",
    "element/red3.png",
    "element/red4.png",
    "element/red5.png",
    "element/red6.png",
    "element/red7.png",
    "element/red8.png",
    "element/red9.png",
};
const vector<string> MusicPath{
    "element/WhatHappen.mp3",
    "element/WhosThere.mp3"
};
const vector<string> ChunkPath{
    "element/putglass.wav",
    "element/breakglass.wav",
    "element/clock.wav",
    "element/door1.wav",
    "element/door2.wav"
};

class mSDL
{
    
private:

    vector<SDL_Texture*> MirrorTexture;
    vector<SDL_Texture*> WhiteTexture;
    vector<SDL_Texture*> RedTexture;
    vector<vector<int>> SudokuColor;

public:

    mSDL();
    bool SurfaceLoad( SDL_Renderer* &Renderer, vector<SDL_Texture*> &Texture, const vector<string> &Path, int i );
    void render( SDL_Renderer* &Renderer, vector<vector<int>> &answer, vector<vector<SDL_FRect>> &TexturePosition, int i, int j );
    void reset( SDL_Renderer* &Renderer, vector<vector<int>> &answer, vector<vector<SDL_FRect>> &TexturePosition );
    vector<SDL_Texture*>* getTexture( int color );
    void TextureDestroy();

};

bool InitWindow( SDL_Window* &Window, SDL_Renderer* &Renderer );
bool InitAudio( SDL_AudioDeviceID &Audio );
bool LoadAllPNGS( mSDL &SDL_Object, SDL_Renderer* &Renderer );
bool LoadAllAudios( vector<Mix_Music*> &musics, vector<Mix_Chunk*> &Chunks );
bool MouseClick( vector<vector<SDL_FRect>> &TexturePosition, pair<int,int> &ErrorPosition, vector<vector<int>> &Answer, set<pair<int,int>> &Breaked, vector<Mix_Chunk*> &Chunks );
void Close( mSDL &SDL_Object, SDL_Window* &Window, SDL_Renderer* &Renderer, SDL_AudioDeviceID &Audio, vector<Mix_Music*> &Musics, vector<Mix_Chunk*> &Chunks );

#endif