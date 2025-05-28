#ifndef __m_SDL_HPP__
#define __m_SDL_HPP__

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
using namespace std;

enum color{
    white,
    red,
};

const int ScreenWidth = 900;
const int ScreenHeight = 900;
const float TextureWidth = 100.f;
const float TextureHeight = 100.f; // 修正拼寫錯誤

const vector<string> WhiteNumberPath{
    "ghost-and-mirror/break.png",
    "ghost-and-mirror/white1.png",
    "ghost-and-mirror/white2.png",
    "ghost-and-mirror/white3.png",
    "ghost-and-mirror/white4.png",
    "ghost-and-mirror/white5.png",
    "ghost-and-mirror/white6.png",
    "ghost-and-mirror/white7.png",
    "ghost-and-mirror/white8.png",
    "ghost-and-mirror/white9.png"
};
const vector<string> RedNumberPath{
    "ghost-and-mirror/break.png",
    "ghost-and-mirror/red1.png",
    "ghost-and-mirror/red2.png",
    "ghost-and-mirror/red3.png",
    "ghost-and-mirror/red4.png",
    "ghost-and-mirror/red5.png",
    "ghost-and-mirror/red6.png",
    "ghost-and-mirror/red7.png",
    "ghost-and-mirror/red8.png",
    "ghost-and-mirror/red9.png"
};

/* Class Prototypes */
class mSDL
{
    
private:

    vector<SDL_Texture*> WhiteTexture;
    vector<SDL_Texture*> RedTexture;
    vector<vector<int>> SudokuColor;

public:

    mSDL();
    bool SurfaceLoad( SDL_Renderer* &Renderer, vector<SDL_Texture*> &Texture, const vector<string> &Path, int i );
    void render( SDL_Renderer* &Renderer, vector<vector<int>> &answer, vector<vector<SDL_FRect>> &TexturePosition, int i, int j );
    vector<SDL_Texture*>* getTexture( int color );
    void TextureDestroy();

};

bool Init( SDL_Window* &gWindow, SDL_Renderer* &Renderer );
bool LoadAllPNGS( mSDL &SDL_Object, SDL_Renderer* &Renderer );
void Close( mSDL &SDL_Object, SDL_Window* &gWindow, SDL_Renderer* &Renderer );

#endif