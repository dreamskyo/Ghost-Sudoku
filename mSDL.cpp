
#include "mSDL.hpp"
#include <random>
#include <ctime>
using namespace std;

mSDL::mSDL()
{
    MirrorTexture.assign(1,nullptr);
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

void mSDL::render( SDL_Renderer* &Renderer, vector<vector<int>> &Answer, vector<vector<SDL_FRect>> &TexturePosition, int i, int j )
{
    switch( SudokuColor[i][j] )
    {   
    case white:
        SDL_RenderTexture( Renderer, RedTexture[ Answer[i][j] ] , nullptr, &TexturePosition[i][j] );
        break;
    case red:
        SDL_RenderTexture( Renderer, WhiteTexture[ Answer[i][j] ] , nullptr, &TexturePosition[i][j] );
        break;
    default:
        SDL_Log("Render color error\n");
    }
    SudokuColor[i][j] = rand() & 1;
}

void mSDL::reset( SDL_Renderer* &Renderer, vector<vector<int>> &Answer, vector<vector<SDL_FRect>> &TexturePosition )
{
    for(int i=0;i<9;++i)
    {
        for(int j=0;j<9;++j)
        {   
            if( Answer[i][j] != 0 )
            {
                SDL_RenderTexture( Renderer, MirrorTexture[ 0 ] , nullptr, &TexturePosition[i][j] );
            }
        }
    }
}

vector<SDL_Texture*>* mSDL::getTexture( int color ){
    switch( color )
    {
    case white:
        return &WhiteTexture;
    case red:
        return &RedTexture;
    default:
        return &MirrorTexture;
    }
}

bool InitWindow( SDL_Window* &Window, SDL_Renderer* &Renderer )
{
    if( !SDL_InitSubSystem( SDL_INIT_VIDEO ) )
    {
        SDL_Log( "System error: %s\n", SDL_GetError() );
    }
    else if( !SDL_CreateWindowAndRenderer( "Ghost-sudoku", ScreenWidth, ScreenHeight, 0, &Window, &Renderer ) )
    {
        SDL_Log( "Window error: %s\n", SDL_GetError() );
    }
    else
    {
        return true;
    }
    return false;
}

bool InitAudio( SDL_AudioDeviceID &Audio )
{
    if( !SDL_InitSubSystem( SDL_INIT_AUDIO ) )
    {
        SDL_Log( "System error: %s\n", SDL_GetError() );
        return false;
    }

    SDL_AudioSpec setting;
    SDL_zero( setting );
    setting.format = SDL_AUDIO_F32;
    setting.channels = 2;
    setting.freq = 44100;

    Audio = SDL_OpenAudioDevice( SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &setting );
    if( Audio == 0 )
    {
        SDL_Log( "Audio error: %s\n", SDL_GetError() );
        return false;
    }
    else if( !Mix_OpenAudio( Audio, nullptr ) )
    {
        printf( "Mixer error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
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

    success &= SDL_Object.SurfaceLoad( Renderer, *( SDL_Object.getTexture( -1 ) ), MirrorPath, 0 );

    return success;
}

bool LoadAllAudios( vector<Mix_Music*> &Musics, vector<Mix_Chunk*> &Chunks )
{
    for(int i=0;i<2;++i)
    {
        Musics[i] = Mix_LoadMUS( MusicPath[i].c_str() );
        if( Musics[i] == nullptr )
        {
            return false;
        }
    }

    for(int i=0;i<5;++i)
    {
        Chunks[i] = Mix_LoadWAV( ChunkPath[i].c_str() );
        if( Chunks[i] == nullptr )
        {
            return false;
        }
    }

    return true;
}

bool MouseClick( vector<vector<SDL_FRect>> &TexturePosition, pair<int,int> &ErrorPosition, vector<vector<int>> &Answer, set<pair<int,int>> &Breaked, vector<Mix_Chunk*> &Chunks )
{
    float x = -1.f, y = -1.f;
    SDL_GetMouseState( &x, &y );
    if( x < 0 || y < 0 || x > ScreenWidth || y > ScreenHeight )
    {
        SDL_Log("Out of screen!\n");
        return false;
    }

    if( x > TexturePosition[ ErrorPosition.first ][ ErrorPosition.second ].x &&
        x < TexturePosition[ ErrorPosition.first ][ ErrorPosition.second ].x + TextureWidth &&
        y > TexturePosition[ ErrorPosition.first ][ ErrorPosition.second ].y &&
        y < TexturePosition[ ErrorPosition.first ][ ErrorPosition.second ].y + TextureHeight 
    )
    {
        if( Breaked.count( ErrorPosition ) )
        {
            SDL_Log("Break same position!\n");
            return false;
        }
        Answer[ ErrorPosition.first ][ ErrorPosition.second ] = 0;
        Breaked.insert( ErrorPosition );
        Mix_PlayChannel( 1, Chunks[1], 0 );
        return true;
    }
    else
    {
        Mix_PlayChannel( 0, Chunks[0], 0 );
        return false;
    }
}

void Close( mSDL &SDL_Object, SDL_Window* &Window, SDL_Renderer* &Renderer, SDL_AudioDeviceID &Audio, vector<Mix_Music*> &Musics, vector<Mix_Chunk*> &Chunks )
{
    for(int i=0;i<2;++i)
    {
        Mix_FreeMusic( Musics[i] );
        Musics[i] = nullptr;
    }

    for(int i=0;i<5;++i)
    {
        Mix_FreeChunk( Chunks[i] );
        Chunks[i] = nullptr;
    }

    SDL_Object.TextureDestroy();
    SDL_DestroyRenderer( Renderer );
    SDL_DestroyWindow( Window );
    SDL_CloseAudioDevice( Audio );
    Renderer = nullptr;
    Window = nullptr;
    Audio = 0;
    Mix_Quit();
    SDL_Quit();
}