//
//  main.cpp
//  SnakeQuest
//
//  Created by Bailey Andrew on 11/11/2017.
//  Copyright © 2017 Alliterative Anchovies. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include <thread>
#include <vector>
#include <fstream>//read & write files
#include <dirent.h>
#include <assert.h>
#include "GraphicsLibrary.hpp"


bool init();
void run();
void close();

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    bool success = true;
    //just gonna pull my fonts from Heptarchy Cocoa instead of setting them up in this project's folders
    std::string dumstupidcurrentdirectorybs = "/Users/baileyandrew/Desktop/Heptarchy Cocoa/";
    if( !init() ) {
        printf( "Failed to initialize!\n" );
    }
    else {
        //get screen resolution:
        SCREEN_WIDTH = 400;
        SCREEN_HEIGHT = 400;
        SDL_SetWindowPosition(gWindow, 300, 300);
        SDL_SetWindowSize(gWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
        
        if( TTF_Init() == -1 ) {
            printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
            //success = false;
            throw std::runtime_error("Font renderer could not be initialized!");
        }
        //init font
        theFont24 = TTF_OpenFont((dumstupidcurrentdirectorybs+"resources/Fonts/Times New Roman.ttf").c_str(),24);
        //std::cout << SDL_GetError();
        theFont20 = TTF_OpenFont((dumstupidcurrentdirectorybs+"resources/Fonts/Times New Roman.ttf").c_str(),20);
        theFont16 = TTF_OpenFont((dumstupidcurrentdirectorybs+"resources/Fonts/Times New Roman.ttf").c_str(),16);
        if (!theFont24||!theFont20||!theFont16) {
            printf("Failed to load fonts!");
            throw std::runtime_error("Failed to load fonts! - had checked directory "+dumstupidcurrentdirectorybs);
        }
        //Create vsynced renderer for window
        if( gRenderer == NULL ) {
            printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
            success=false;
        }
        else {
            //Initialize renderer color
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);

            if (success) {
                run();
            }
        }
    }
    close();
    return 0;
}

void run() {
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xffff0000);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(10000);
}


bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "Solve Flo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
                        //SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            SDL_SetWindowBordered(gWindow, SDL_FALSE);//for some reason the header is transparent, so
                                                        //let's just not let it have a header.
            gScreenSurface = SDL_GetWindowSurface( gWindow );
            gRenderer = SDL_GetRenderer(gWindow);
        }
    }

    return success;
}

void close()
{
    //Close fonts
    TTF_CloseFont(theFont16);
    TTF_CloseFont(theFont20);
    TTF_CloseFont(theFont24);
    theFont24 = NULL;
    theFont16 = NULL;
    theFont20 = NULL;
    //Destroy window
    SDL_DestroyWindow( gWindow );
    SDL_DestroyRenderer(gRenderer);
    gWindow = NULL;
    gRenderer = NULL;
    //Close all surfaces and textures
    //are none

    //Quit SDL subsystems
    SDL_Quit();
    TTF_Quit();
    
}
