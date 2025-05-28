
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
        SDL_INIT_FAILED,
        SUDOKU_GEN_FAILED,
        RESOURCE_LOAD_FAILED
    };

    vector<vector<int>> answer(9,vector<int>(9));
    DLX solver(&answer);

    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    mSDL SDL_Object;

    vector<vector<SDL_FRect>> TexturePosition(9,vector<SDL_FRect>(9));

    if( !Init( Window, Renderer ) )
    {
        SDL_Log( "無法建立視窗!\n" );
        Close( SDL_Object, Window, Renderer );
        return SDL_INIT_FAILED;
    }
    
    if(!solver.DLXmain()){
        SDL_Log("產生數獨失敗!\n");
        Close( SDL_Object, Window, Renderer );
        return SUDOKU_GEN_FAILED;
    }
        
    if( !LoadAllPNGS( SDL_Object, Renderer ) )
    {
        SDL_Log( "請檢查圖片路徑!\n" );
        Close( SDL_Object, Window, Renderer );
        return RESOURCE_LOAD_FAILED;
    }

    for(int i=0;i<9;++i)
    {
        for(int j=0;j<9;++j)
        {   
            TexturePosition[i][j] = { 100.f * static_cast<float>(i), 100.f * static_cast<float>(j), TextureWidth, TextureHeight };
        }
    }

    bool quit = false;
    SDL_Event e;
    SDL_zero( e );
    
    while( quit == false )
    {

        while( SDL_PollEvent( &e ) )
        {
            quit = ( e.type == SDL_EVENT_QUIT );
        }

        SDL_SetRenderDrawColor( Renderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderClear( Renderer );

        for(int i=0;i<9;++i)
        {
            for(int j=0;j<9;++j)
            {   
                SDL_Object.render( Renderer, answer, TexturePosition, i, j );
            }
        }

        SDL_RenderPresent( Renderer );
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } 

    Close( SDL_Object, Window, Renderer );
    return SUCCESS;
}
