
#include "mSDL.hpp"
#include <random>
#include <ctime>
using namespace std;

mSDL::mSDL() {
    WhiteTexture.assign(10,nullptr);
    RedTexture.assign(10,nullptr);
    SudokuColor.assign(9,vector<int>(9));
    srand(time(NULL));
    for(int i=0;i<9;++i)
    {
        for(int j=0;j<9;++j)
        {
            SudokuColor[i][j] = rand() & 1;
        }
    }
}

bool mSDL::SurfaceLoad( SDL_Renderer* &Renderer, vector<SDL_Texture*> &Texture, const vector<string> &Path, int i )
{
    SDL_Surface* tSurface = IMG_Load( Path[i].c_str() );

    if( tSurface == nullptr )
    {
        SDL_Log( "Image %s error: %s\n", Path[i].c_str(), SDL_GetError() );
        return false;
    }

    Texture[i] = SDL_CreateTextureFromSurface( Renderer, tSurface );

    if( Texture[i] == nullptr )
    {
            SDL_Log( "Texture error: %s\n", SDL_GetError() );
            return false;
    }

    SDL_DestroySurface( tSurface );
    return true;
}

void mSDL::TextureDestroy()
{
    for(int i=0;i<10;++i)
    {
        SDL_DestroyTexture( WhiteTexture[i] );
        SDL_DestroyTexture( RedTexture[i] );
        WhiteTexture[i] = nullptr;
        RedTexture[i] = nullptr;
    }
}

void mSDL::render( SDL_Renderer* &Renderer, vector<vector<int>> &answer, vector<vector<SDL_FRect>> &TexturePosition, int i, int j )
{
    switch( SudokuColor[i][j] )
    {   
    case white:
        SDL_RenderTexture( Renderer, RedTexture[answer[i][j]] , nullptr, &TexturePosition[i][j] );
        break;
    case red:
        SDL_RenderTexture( Renderer, WhiteTexture[answer[i][j]] , nullptr, &TexturePosition[i][j] );
        break;
    default:
        SDL_Log("Render color error\n");
    }
    SudokuColor[i][j] = rand() & 1;
}

vector<SDL_Texture*>* mSDL::getTexture( int color ){
    switch( color )
    {
    case white:
        return &WhiteTexture;
    case red:
        return &RedTexture;
    default:
        SDL_Log("Wrong Texture!");
        return nullptr;
    }
}

bool Init( SDL_Window* &gWindow, SDL_Renderer* &Renderer )
{
    if( !SDL_InitSubSystem( SDL_INIT_VIDEO ) )
    {
        SDL_Log( "Init error: %s\n", SDL_GetError() );
    }
    else if( !SDL_CreateWindowAndRenderer( "Ghost-sudoku", ScreenWidth, ScreenHeight, 0, &gWindow, &Renderer ) )
    {
        SDL_Log( "Window error: %s\n", SDL_GetError() );
    }
    else
    {
        return true;
    }
    return false;
}

bool LoadAllPNGS( mSDL &SDL_Object, SDL_Renderer* &Renderer )
{
    bool success = true;
    for(int i=0;i<10;++i){
        success &= SDL_Object.SurfaceLoad( Renderer, *( SDL_Object.getTexture( white ) ), WhiteNumberPath, i );
        success &= SDL_Object.SurfaceLoad( Renderer, *( SDL_Object.getTexture( red ) ), RedNumberPath, i );
        if( !success )
        {
            break;
        }
    }
    return success;
}

void Close( mSDL &SDL_Object, SDL_Window* &gWindow, SDL_Renderer* &Renderer )
{
    SDL_Object.TextureDestroy();
    SDL_DestroyRenderer( Renderer );
    SDL_DestroyWindow( gWindow );
    Renderer = nullptr;
    gWindow = nullptr;
    SDL_Quit();
}