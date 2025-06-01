
#include "DLX.hpp"
#include "mSDL.hpp"
#include <SDL3/SDL_main.h>
#include <thread>
#include <chrono>
using namespace std;

int main( int argc, char* args[0] )
{

    enum ErrorCode {
        SUCCESS,
        SDL_WINDOW_FAILED,
        SDL_AUDIO_FAILED,
        SUDOKU_GEN_FAILED,
        RESOURCE_LOAD_FAILED
    };

    vector<vector<int>> Answer( 9, vector<int>( 9 ) );
    DLX solver( &Answer );
    vector<int> NextError;
    set<pair<int,int>> Breaked;

    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    SDL_AudioDeviceID Audio = 0;
    mSDL SDL_Object;

    vector<vector<SDL_FRect>> TexturePosition( 9, vector<SDL_FRect>( 9 ) );
    vector<Mix_Music*> Musics( 2 );
    vector<Mix_Chunk*> Chunks( 5 );
    int NowMusic = 0;

    if( !InitWindow( Window, Renderer ) )
    {
        SDL_Log( "無法建立視窗!\n" );
        Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
        return SDL_WINDOW_FAILED;
    }

    if( !InitAudio( Audio ) )
    {
        SDL_Log( "無法設置音訊!\n" );
        Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
        return SDL_AUDIO_FAILED;
    }
    
    if( !solver.DLXmain() )
    {
        SDL_Log("產生數獨失敗!\n");
        Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
        return SUDOKU_GEN_FAILED;
    }
        
    if( !LoadAllPNGS( SDL_Object, Renderer ) )
    {
        SDL_Log( "請檢查圖片路徑!\n" );
        Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
        return RESOURCE_LOAD_FAILED;
    }

    if( !LoadAllAudios( Musics, Chunks ) )
    {
        SDL_Log( "請檢查音訊路徑!\n" );
        Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
        return RESOURCE_LOAD_FAILED;
    }
    

    for(int i=0;i<9;++i)
    {
        for(int j=0;j<9;++j)
        {   
            TexturePosition[i][j] = { 100.f * static_cast<float>( i ), 100.f * static_cast<float>( j ), TextureWidth, TextureHeight };
            NextError.push_back( i * 10 + j );
        }
    }

    srand( time( NULL ) );
    random_shuffle( NextError.begin(), NextError.end() );
    pair<int,int> ErrorPosition = { 0, 0 };
    int NowError = 0;
    NowMusic = rand() % 2;

    bool quit = false;
    SDL_Event e;
    SDL_zero( e );

    while( quit == false )
    {   
        if( !solver.DLXmain() )
        {
            SDL_Log("產生數獨失敗!\n");
            Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
            return SUDOKU_GEN_FAILED;
        }
        bool correct = false;

        for( pair<int,int> b : Breaked )
        {
            Answer[b.first][b.second] = 0;
        }

        if( NowError < 81 )
        {
            ErrorPosition = { NextError[ NowError ] / 10, NextError[ NowError ] % 10 };
            ++NowError;
        }

        int t = Answer[ ErrorPosition.first ][ ErrorPosition.second ];
        while( Answer[ ErrorPosition.first ][ ErrorPosition.second ] == t ){
            Answer[ ErrorPosition.first ][ ErrorPosition.second ] = rand()%9+1;
        }

        while( quit == false && correct == false )
        {
            while( SDL_PollEvent( &e ) )
            {
                switch( e.type ){
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    correct = MouseClick( TexturePosition, ErrorPosition, Answer, Breaked, Chunks );
                    break;
                }
            }

            if( Mix_PlayingMusic() == 0 ){
                Mix_PlayMusic( Musics[ NowMusic%2 ], 0 );
                ++NowMusic;
            }

            if( rand()%100 == 1 )
            {
                int Channel = rand()%3+2;
                Mix_PlayChannel( Channel, Chunks[ Channel ], 0 );
            }

            SDL_SetRenderDrawColor( Renderer, 0x00, 0x00, 0x00, 0xFF );
            SDL_RenderClear( Renderer );

            for(int i=0;i<9;++i)
            {
                for(int j=0;j<9;++j)
                {   
                    SDL_Object.render( Renderer, Answer, TexturePosition, i, j );
                }
            }

            SDL_RenderPresent( Renderer );
            std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
        }

        SDL_Object.reset( Renderer, Answer, TexturePosition );
        std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
    } 

    Close( SDL_Object, Window, Renderer, Audio, Musics, Chunks );
    return SUCCESS;
}
