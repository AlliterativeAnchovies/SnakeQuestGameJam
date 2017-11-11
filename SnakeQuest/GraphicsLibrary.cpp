//
//  AlitAn_lib.cpp
//  Solve Flo
//
//  Created by Bailey Andrew on 10/11/2017.
//  Copyright © 2017 Alliterative Anchovies. All rights reserved.
//

#include "GraphicsLibrary.hpp"

//Annoying extern stuff
SDL_Renderer* gRenderer;//The window renderer
SDL_Surface* gScreenSurface;//The surface contained by the window
SDL_Window* gWindow;//The window we'll be rendering to
SDL_Texture* mapToDraw;
SDL_Rect stretchRect;
TTF_Font* theFont24;
TTF_Font* theFont20;
TTF_Font* theFont16;
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//Graphics definitions
Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    /*if (pixels==NULL) {
        return 0x00000000;
    }*/
    //Get the requested pixel
    return pixels[ ( y * surface->w ) + x ];
}

void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    
    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}

void drawCircleOnSurface(SDL_Surface* theSurface,int centx,int centy,float radius,Uint32 color) {
    for (int wi = 0;wi<theSurface->w;wi++) {
        for (int hi = 0;hi<theSurface->h;hi++) {
            if (sqrt((wi-centx)*(wi-centx)+(hi-centy)*(hi-centy))<=radius) {
                put_pixel32( theSurface, wi, hi, color );
            }
        }
    }
}
void drawHollowCircleOnSurface(SDL_Surface* theSurface,int centx,int centy,float radius,Uint32 color) {
    int oldPosX = centx+radius;
    int oldPosY = centy;
    for (int i = 0;i<360;i++) {
        float angle = i/180.0*M_PI;
        float sin_theta;
        float cos_theta;
        fastSineCosine_float(&sin_theta, &cos_theta, angle);
        sin_theta*=radius;
        cos_theta*=radius;
        drawLineOnSurface(theSurface, oldPosX, oldPosY, centx+cos_theta, centy+sin_theta, color);
        oldPosX = centx+cos_theta;
        oldPosY = centy+sin_theta;
    }
}
void drawRect(int px,int py,int wid,int hei, Uint32 color) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    Uint8 r = (color<<8)>>24;
    Uint8 g = (color<<16)>>24;
    Uint8 b = (color<<24)>>24;
    Uint8 a = color>>24;
    SDL_SetRenderDrawColor(gRenderer,r,g,b,a);
    SDL_RenderFillRect(gRenderer,&stretchRect);
}

SDL_Surface* shadeSurface(SDL_Surface* toShade,Uint32 colorOver) {
    //Does not modify toShade
    SDL_Surface* temp = createBlankSurfaceWithSize(toShade->w, toShade->h);
    SDL_Rect toDraw;
    toDraw.x = 0;
    toDraw.y = 0;
    toDraw.w = toShade->w;
    toDraw.h = toShade->h;
    SDL_FillRect(temp, &toDraw, colorOver);
    SDL_Surface* toReturn = SDL_ConvertSurface(toShade, gScreenSurface->format, NULL);
    SDL_BlitSurface(temp,NULL,toReturn,NULL);
    SDL_FreeSurface(temp);
    return toReturn;
}

Uint32 blend(Uint32 c1, Uint32 c2) {
    Uint32 result = 0x00000000;
    double a1 = getA(c1) / 255.0;
    double a2 = getA(c2) / 255.0;

    result += ((int) (a1 * getR(c1) + a2 * (1 - a1) * getR(c2)))*0x00010000;
    result += ((int) (a1 * getG(c1) + a2 * (1 - a1) * getG(c2)))*0x00000100;
    result += ((int) (a1 * getB(c1) + a2 * (1 - a1) * getB(c2)))*0x00000001;
    result += ((int) (255 * (a1 + a2 * (1 - a1))))*0x01000000;
    return result;
}

void fillAreaOnSurface(SDL_Surface* theSurface,Uint32 fillColor,int centerx,int centery) {
    put_pixel32(theSurface, centerx, centery, fillColor);
    for (int looper = 0;looper<100;looper++) {
        for (int wi = 0;wi<theSurface->w;wi++) {
            for (int hi = 0;hi<theSurface->h;hi++) {
                if (get_pixel32(theSurface, wi, hi)==fillColor) {//make the ones around it the skin color as well
                    Uint32 north=0xFF000000;
                    if (hi>0) {
                        north = get_pixel32(theSurface, wi, hi-1);
                        if (north!=0xFF000000) {
                            put_pixel32(theSurface, wi, hi-1, fillColor);
                        }
                    }
                    Uint32 south=0xFF000000;
                    if (hi<(theSurface->h)-1) {
                        south = get_pixel32(theSurface, wi, hi+1);
                        if (south!=0xFF000000) {
                            put_pixel32(theSurface, wi, hi+1, fillColor);
                        }
                    }
                    Uint32 west=0xFF000000;
                    if (wi>0) {
                        west = get_pixel32(theSurface, wi-1, hi);
                        if (west!=0xFF000000) {
                            put_pixel32(theSurface, wi-1, hi, fillColor);
                        }
                    }
                    Uint32 east=0xFF000000;
                    if (wi<(theSurface->w)-1) {
                        east = get_pixel32(theSurface, wi+1, hi);
                        if (east!=0xFF000000) {
                            put_pixel32(theSurface, wi+1, hi, fillColor);
                        }
                    }
                }
            }
        }
    }
}

void drawLineOnSurface(SDL_Surface* theSurface,double x1,double y1,double x2,double y2,Uint32 color) {
    double x = x2 - x1;
    double y = y2 - y1;
    double length = sqrt( x*x + y*y );

    double addx = x / length;
    double addy = y / length;

    x = x1;
    y = y1;
    for(double i = 0; i < length; i += 1) {
        if (y>=0&&y<theSurface->h&&x>=0&&x<theSurface->w) {
            put_pixel32( theSurface, x, y, color );
        }
        x += addx;
        y += addy;
    }
}

void drawLineOnSurfaceSafe(SDL_Surface* theSurface,double x1,double y1,double x2,double y2,Uint32 color) {
    //Un-safe lines may look like this:
    //         *********
    //*********
    //
    //Which is not a problem (in fact, fixing it can be a problem!) unless you're drawing lines next to eachother:
    //         *********
    //********* ********
    //**********
    //
    //To get rid of that hole, we make every line-segment look like:
    //        **********
    //**********
    //There's no guarantee that the problem is solved, but it's impossible to guarantee because to do so this function
    //would need to know more about the lines around it.
    double x = x2 - x1;
    double y = y2 - y1;
    double length = sqrt( x*x + y*y );

    double addx = x / length;
    double addy = y / length;

    x = x1;
    y = y1;
    
    double prevX = x;
    double prevY = y;
    bool isVerticalish = true;//if the line is more vertical than it is horizontal
    if (addx>addy) {
        isVerticalish = false;
    }
    
    for(double i = 0; i < length; i += 1) {
        if (y>=0&&y<theSurface->h&&x>=0&&x<theSurface->w) {
            put_pixel32( theSurface, x, y, color );
        }
        if (isVerticalish) {
            //component lines are vertical, every change in x we need to fill in the corners
            if (floor(prevX)!=floor(x)) {
                //one such change has occured!
                put_pixel32(theSurface, prevX, y, color);
                put_pixel32(theSurface, x, prevY, color);
            }
        }
        else {
            //component lines are horizontal, every change in y we need to fill in the corners
            if (floor(prevY)!=floor(y)) {
                //one such change has occured!
                put_pixel32(theSurface, x, prevY, color);
                put_pixel32(theSurface, prevX, y, color);
            }
        }
        prevX = x;
        prevY = y;
        x += addx;
        y += addy;
    }
}

bool pointIsOnSegment(Point<int> segstart,Point<int> segend,Point<int> tocheck) {
    //Imagine two vectors, one defined as segstart->segend and one as segstart->tocheck
    int seg_x = segend.x-segstart.x;
    int seg_y = segend.y-segstart.y;
    int check_x = tocheck.x-segstart.x;
    int check_y = tocheck.y-segstart.y;
    //Check if they are parallel vectors (true if determinant is zero)
    if  (seg_x*check_y-seg_y*check_x!=0) {
        return false;//not parallel
    }
    //are parallel, this means that if tocheck is between segstart and segend then it is on the line
    if (segstart.x<segend.x) {
        //points in correct order
        //check if between them
        return tocheck.x>=segstart.x&&tocheck.x<=segend.x;
    }
    else if (segstart.x>segend.x) {
        //points in opposite order
        //check if between them
        return tocheck.x<=segstart.x&&tocheck.x>=segend.x;
    }
    else {
        //vertical line:
        if (segstart.y<segend.y) {
            //correct order
            return tocheck.y>=segstart.y&&tocheck.y<=segend.y;
        }
        else {
            //opposite order (or on horizontal line which would imply being on singular point which still counts and correct answer given)
            return tocheck.y<=segstart.y&&tocheck.y>=segend.y;
        }
    }
}

void fillPolygonOnSurface(SDL_Surface* theSurface,Uint32 fillColor,std::vector<float> x,std::vector<float> y) {
    //handle degenerate cases
    if (x.size()<3) {//degenerate case, throw error
        #ifdef DEBUGMODE
            throw std::runtime_error("Called fillPolygonOnSurface but gave "+std::to_string(x.size())+" points instead of mandatory >=3 points");
        #else
            return;
        #endif
    }
    if (x.size()==3) {
        fillTriangleOnSurface(theSurface, x[0], y[0], x[1], y[1], x[2], y[2], fillColor);
        return;
    }
    //done handling degen cases.  Now, draw polygon
    //we do this by counting number of edges it has passes - an odd number is inside polygon, otherwise outside
    
    
    //  public-domain code by Darel Rex Finley, 2007
    int  nodes, nodeX[x.size()], pixelX, pixelY, i, j, swap ;
    //  Loop through the rows of the image.
    for (pixelY=0; pixelY<theSurface->h; pixelY++) {
        //  Build a list of nodes.
        nodes=0; j=(int)x.size()-1;
        for (i=0; i<x.size(); i++) {
            if ((y[i]<(double) pixelY && y[j]>=(double) pixelY) ||  (y[j]<(double) pixelY && y[i]>=(double) pixelY)) {
                nodeX[nodes++]=(int) (x[i]+(pixelY-y[i])/(y[j]-y[i])*(x[j]-x[i]));
            }
            j=i;
        }
        //  Sort the nodes, via a simple “Bubble” sort.
        i=0;
        while (i<nodes-1) {
            if (nodeX[i]>nodeX[i+1]) {
                swap=nodeX[i];
                nodeX[i]=nodeX[i+1];
                nodeX[i+1]=swap;
                if (i>0) {
                    i--;
                }
            }
            else {
                i++;
            }
        }
        //  Fill the pixels between node pairs.
        for (i=0; i<nodes; i+=2) {
            if   (nodeX[i  ]>=theSurface->w) {
                break;
            }
            if   (nodeX[i+1]> 0 ) {
                if (nodeX[i  ]< 0 ) {
                    nodeX[i  ]=0 ;
                }
                if (nodeX[i+1]> theSurface->w) {
                    nodeX[i+1]=theSurface->w;
                }
                for (pixelX=nodeX[i]+1; pixelX<nodeX[i+1]; pixelX++) {//nodeX[i]+1 to prevent 'floating pixels'
                    //fillPixel(pixelX,pixelY);
                    put_pixel32(theSurface, pixelX, pixelY, fillColor);
                }
            }
        }
    }
}

void fillPolygonOnSurface_old(SDL_Surface* theSurface,Uint32 fillColor,int centerx,int centery,std::vector<float> x,std::vector<float> y) {
    //Basically we just fill a bunch of triangles
    if (x.size()<3) {//degenerate case, throw error
        throw std::runtime_error("Called fillPolygonOnSurface but gave "+std::to_string(x.size())+" points instead of mandatory >=3 points");
    }
    if (x.size()==3) {
        fillTriangleOnSurface(theSurface, x[0], y[0], x[1], y[1], x[2], y[2], fillColor);
        return;
    }
    for (int i = 0;i<x.size()-1;i++) {
        fillTriangleOnSurface(theSurface, x[i], y[i], x[i+1], y[i+1], centerx, centery, fillColor);
        drawLineOnSurfaceSafe(theSurface, centerx, centery, x[i], y[i], fillColor);
    }
    fillTriangleOnSurface(theSurface, x[x.size()-1], y[x.size()-1], x[0], y[0], centerx, centery, fillColor);
    drawLineOnSurfaceSafe(theSurface, centerx, centery, x[x.size()-1], y[x.size()-1], fillColor);
}

void fillTriangleOnSurface(SDL_Surface* toDraw,int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color) {
    //first, draw borders, to try avoiding rounding error shenanigans:
    //drawLineOnSurfaceSafe(toDraw, x1, y1, x2, y2, color);
    //drawLineOnSurfaceSafe(toDraw, x1, y1, x3, y3, color);
    //drawLineOnSurfaceSafe(toDraw, x2, y2, x3, y3, color);
    //get points in order of height
    int top_x =     (y1>=y2)?                //if y1 is larger than y2
                        ((y1>=y3)?x1:x3):    //then check if it is larger than y3 - if so, it is largest, otherwise y3 is largest
                        ((y2>=y3)?x2:x3);    //if y2>=y1, then check if y2 is greater than y3 - if so, y2 is largest, otherwise y3 is largest
    int top_y =     (y1>=y2)?                //same as above ^
                        ((y1>=y3)?y1:y3):
                        ((y2>=y3)?y2:y3);
    int bottom_x =  (y1<=y2)?                //if y1 is smaller than y2
                        ((y1<=y3)?x1:x3):    //then check if it is smaller than y3, if so, it is the smallest, otherwise y3 is the smallest
                        ((y2<=y3)?x2:x3);    //if y2<=y1, then check if y2 is less than y3 - is so, y2 is smallest, otherwise, y3 is smallest
    int bottom_y =  (y1<=y2)?                //same as above ^
                        ((y1<=y3)?y1:y3):
                        ((y2<=y3)?y2:y3);
    int middle_x =  (y1==top_y)?                    //if y1 is largest
                        ((y2==bottom_y)?x3:x2):     //check if y2 is smallest - if so, y3 is middle, otherwise y3 must be smallest so y2 is middle
                        ((y2==top_y)?               //if y1 is not largest, check if y2 is largest
                            ((y1==bottom_y)?x3:x1): //if y2 is largest, check if y1 is smallest - if so, y3 is middle, otherwise y3 must be smallest so y1 is middle
                            ((y1==bottom_y)?x2:x1)  //if y2 is not largest, y3 must be largest.  If y1 is smallest, then y2 must be middle, otherwise y1 is middle
                        );
    int middle_y =  (y1==top_y)?                    //same as above ^
                        ((y2==bottom_y)?y3:y2):
                        ((y2==top_y)?
                            ((y1==bottom_y)?y3:y1):
                            ((y1==bottom_y)?y2:y1)
                        );
    //find deltas
    int del_tb_x = bottom_x-top_x;
    int del_tb_y = bottom_y-top_y;
    int del_tm_x = middle_x-top_x;
    int del_tm_y = middle_y-top_y;
    int del_mb_x = bottom_x-middle_x;
    int del_mb_y = bottom_y-middle_y;
    //now draw the lines
    for (int y_pos = top_y;y_pos>=bottom_y;y_pos--) {
        //need to figure out the two x points, along each line:
        //slope = del_tb_y/del_tb_x
        //slope = (b_y - y_pos)/(b_x - x_pos)
        //del_tb_y/del_tb_x = (b_y - y_pos)/(b_x - x_pos)
        //b_x-(del_tb_x/del_tb_y)*(b_y-y_pos) = x_pos
        //x point along tb
        int x_pos_1;
        if (del_tb_y!=0) {
            x_pos_1 = bottom_x-(1.0*del_tb_x/del_tb_y)*(bottom_y-y_pos);
        }
        else {//avoid divide-by-zero
            if (bottom_y-y_pos>0) {
                x_pos_1 = 0;
            }
            else if (bottom_y-y_pos<0) {
                x_pos_1 = toDraw->w-1;
            }
            else {
                x_pos_1 = bottom_x;
            }
        }
        int x_pos_2;
        if (y_pos>middle_y) {//x point along tm
            if (del_tm_y!=0) {
                x_pos_2 = middle_x-(1.0*del_tm_x/del_tm_y)*(middle_y-y_pos);
            }
            else {//avoid divide-by-zero
                if (middle_y-y_pos>0) {
                    x_pos_2 = 0;
                }
                else if (middle_y-y_pos<0) {
                    x_pos_2 = toDraw->w-1;
                }
                else {
                    x_pos_2 = middle_x;
                }
            }
        }
        else {//x point along mb
            if (del_mb_y!=0) {
                x_pos_2 = bottom_x-(1.0*del_mb_x/del_mb_y)*(bottom_y-y_pos);
            }
            else {//avoid divide-by-zero
                if (bottom_y-y_pos>0) {
                    x_pos_2 = 0;
                }
                else if (bottom_y-y_pos<0) {
                    x_pos_2 = toDraw->w-1;
                }
                else {
                    x_pos_2 = bottom_x;
                }
            }
        }
        drawLineOnSurface(toDraw, x_pos_1, y_pos, x_pos_2, y_pos, color);
    }
}

Uint32 convertToRGBA(Uint32 ARGB) {
    if (ARGB<0xFF000000) {
        //std::cout <<"ARGB passed, not 100% opaque?: "<<ARGB<<"\n";
        return 0x00000000;
    }
    ARGB-=0xFF000000;
    ARGB=ARGB<<8;
    ARGB+=0x000000FF;
    return ARGB;
}

void drawGraphic (int px,int py,int wid,int hei,SDL_Surface* graph) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    //SDL_RenderCopy(gRenderer,graph,NULL,&stretchRect);
    SDL_BlitSurface(graph,NULL,gScreenSurface,&stretchRect);

}
void drawRectBorder(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(gRenderer,r,g,b,a);
    SDL_RenderDrawRect(gRenderer,&stretchRect);
}

bool pointInBounds(int px,int py,int bx,int bxf,int by,int byf) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    //px = 1.0*px*(newSizex)/SCREEN_WIDTH;
    //py = 1.0*py*(newSizey)/SCREEN_HEIGHT;
    bx = bx*(1.0*(newSizex)/SCREEN_WIDTH);
    by = by*(1.0*(newSizey)/SCREEN_HEIGHT);
    bxf = bxf*(1.0*(newSizex)/SCREEN_WIDTH);
    byf = byf*(1.0*(newSizey)/SCREEN_HEIGHT);
    if (px>=bx&&px<=bxf&&py>=by&&py<=byf) {
        return true;
    }
    return false;
}

bool pointInCircle(int px,int py,int cx,int cy,int r) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    //px = px*(1.0*newSizex)/SCREEN_WIDTH;
    //py = py*(1.0*newSizey)/SCREEN_HEIGHT;
    cx = cx*(1.0*newSizex)/SCREEN_WIDTH;
    cy = cy*(1.0*newSizey)/SCREEN_HEIGHT;
    r  =  r*(1.0*newSizex)/SCREEN_WIDTH;
    if (sqrt((px-cx)*(px-cx)+(py-cy)*(py-cy))<=r) {
        return true;
    }
    return false;
}
void drawRectWithBorderGraphics(int px,int py,int wid,int hei,SDL_Surface* fill,SDL_Surface* border,SDL_Surface* corner) {
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, wid, hei, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    float breakdownWidth = (1.0*wid)/border->w-1;
    float breakdownHeight = (1.0*hei)/border->h-1;
    bool quitOutX = false;
    //border is top row
    //fill is middle
    //corner is top left
    SDL_Surface* borderLeft = rotateSurfaceNinetyDegreesCounterClockwise(border);
    SDL_Surface* borderRight = flipSurfaceOverYAxis(borderLeft);
    SDL_Surface* borderBottom = flipSurfaceOverXAxis(border);
    SDL_Surface* corner_0_bH = rotateSurfaceNinetyDegreesCounterClockwise(corner);
    SDL_Surface* corner_bW_bH = rotateSurfaceNinetyDegreesCounterClockwise(corner_0_bH);
    SDL_Surface* corner_0_bW = rotateSurfaceNinetyDegreesCounterClockwise(corner_bW_bH);
    
    
    for (float x = 0;true;x++) {
        bool quitOutY = false;
        for (float y = 0;true;y++) {
            if (x>breakdownWidth) {
                x=breakdownWidth;
                quitOutX=true;
            }
            if (y>breakdownHeight) {
                y=breakdownHeight;
                quitOutY=true;
            }
            SDL_Rect destRect;
            destRect.x = x*border->w;//Place border position every 'border->w'-th space
            destRect.y = y*border->h;
            destRect.w = border->w;
            destRect.h = border->h;
            SDL_Surface* toDraw = NULL;
            if (y==0) {
                //Draw top border
                toDraw = border;
            }
            else if (y==breakdownHeight) {
                //Draw bottom border, flip over x axis
                toDraw = borderBottom;
            }
            else if (x==0) {
                //Draw left border, rotate ninety degrees
                toDraw = borderLeft;
            }
            else if (x==breakdownWidth) {
                //Draw right border, rotate ninety degrees and flip over y axis
                toDraw = borderRight;
            }
            if (y==x&&x==0) {
                toDraw = corner;
            }
            if (y==0&&x==breakdownWidth) {
                toDraw = corner_0_bW;
            }
            if (x==0&&y==breakdownHeight) {
                toDraw = corner_0_bH;
            }
            if (x==breakdownWidth&&y==breakdownHeight) {
                toDraw = corner_bW_bH;
            }
            if (toDraw==NULL) {
                toDraw = fill;
            }
            SDL_BlitSurface(toDraw,NULL,surfaceToDraw,&destRect);
            //surfacesToFree.push_back(toDraw);
            //toDraw=NULL;
            if (quitOutY) {
                break;
            }
        }
        if (quitOutX) {
            break;
        }
    }
    SDL_Texture* textureToDraw = SDL_CreateTextureFromSurface(gRenderer, surfaceToDraw);
    drawGraphic(px, py, surfaceToDraw->w, surfaceToDraw->h, textureToDraw);
    SDL_FreeSurface(surfaceToDraw);
    SDL_DestroyTexture(textureToDraw);
    SDL_FreeSurface(borderLeft);
    SDL_FreeSurface(borderBottom);
    SDL_FreeSurface(borderRight);
    SDL_FreeSurface(corner_bW_bH);
    SDL_FreeSurface(corner_0_bH);
    SDL_FreeSurface(corner_0_bW);
}
/*void drawRectWithBorderGraphics(int px,int py,int wid,int hei,SDL_Surface* fill,SDL_Surface* border,SDL_Surface* corner) {
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, wid, hei, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    float breakdownWidth = (1.0*wid)/border->w-1;
    float breakdownHeight = (1.0*hei)/border->h-1;
    std::vector<SDL_Surface*> surfacesToFree = {};
    //for (int x = 0;x<breakdownWidth;x++) {
    bool quitOutX = false;
    for (float x = 0;true;x++) {
        //for (int y = 0;y<breakdownHeight;y++) {
        bool quitOutY = false;
        for (float y = 0;true;y++) {
            if (x>breakdownWidth) {
                x=breakdownWidth;
                quitOutX=true;
            }
            if (y>breakdownHeight) {
                y=breakdownHeight;
                quitOutY=true;
            }
            SDL_Rect destRect;
            destRect.x = x*border->w;//Place border position every 'border->w'-th space
            destRect.y = y*border->h;
            destRect.w = border->w;
            destRect.h = border->h;
            SDL_Surface* toDraw = NULL;
            if (y==0) {
                //Draw top border
                toDraw = SDL_ConvertSurface(border, gScreenSurface->format, NULL);
            }
            //else if (y+1>=breakdownHeight) {
            else if (y==breakdownHeight) {
                //Draw bottom border, flip over x axis
                toDraw= flipSurfaceOverXAxis(border);
            }
            else if (x==0) {
                //Draw left border, rotate ninety degrees
                toDraw= rotateSurfaceNinetyDegreesCounterClockwise(border);
            }
            //else if (x+1>=breakdownWidth) {
            else if (x==breakdownWidth) {
                //Draw right border, rotate ninety degrees and flip over y axis
                SDL_Surface* temp = rotateSurfaceNinetyDegreesCounterClockwise(border);
                toDraw= flipSurfaceOverYAxis(temp);
                SDL_FreeSurface(temp);
                temp=NULL;
            }
            if (y==x&&x==0) {
                SDL_FreeSurface(toDraw);
                toDraw = SDL_ConvertSurface(corner, gScreenSurface->format, NULL);
            }
            //if (y==0&&x+1>=breakdownWidth) {
            if (y==0&&x==breakdownWidth) {
                //toDraw = SDL_ConvertSurface(corner, gScreenSurface->format, NULL);
                SDL_FreeSurface(toDraw);
                SDL_Surface* temp = rotateSurfaceNinetyDegreesCounterClockwise(corner);
                SDL_Surface* temp2 = rotateSurfaceNinetyDegreesCounterClockwise(temp);
                toDraw = rotateSurfaceNinetyDegreesCounterClockwise(temp2);
                SDL_FreeSurface(temp);
                SDL_FreeSurface(temp2);
                temp=NULL;
                temp2=NULL;
            }
            //if (x==0&&y+1>=breakdownHeight) {
            if (x==0&&y==breakdownHeight) {
                SDL_FreeSurface(toDraw);
                toDraw = rotateSurfaceNinetyDegreesCounterClockwise(corner);
            }
            //if (x+1>=breakdownWidth&&y+1>=breakdownHeight) {
            if (x==breakdownWidth&&y==breakdownHeight) {
                SDL_FreeSurface(toDraw);
                SDL_Surface* temp = rotateSurfaceNinetyDegreesCounterClockwise(corner);
                toDraw = rotateSurfaceNinetyDegreesCounterClockwise(temp);
                SDL_FreeSurface(temp);
                temp=NULL;
            }
            if (toDraw==NULL) {
                toDraw = SDL_ConvertSurface(fill, gScreenSurface->format, NULL);
            }
            SDL_BlitSurface(toDraw,NULL,surfaceToDraw,&destRect);
            SDL_FreeSurface(toDraw);
            //surfacesToFree.push_back(toDraw);
            //toDraw=NULL;
            if (quitOutY) {
                break;
            }
        }
        if (quitOutX) {
            break;
        }
    }
    SDL_Texture* textureToDraw = SDL_CreateTextureFromSurface(gRenderer, surfaceToDraw);
    drawGraphic(px, py, surfaceToDraw->w, surfaceToDraw->h, textureToDraw);
    SDL_FreeSurface(surfaceToDraw);
    surfaceToDraw = NULL;
    SDL_DestroyTexture(textureToDraw);
    textureToDraw = NULL;
    for (int i = 0;i<surfacesToFree.size();i++) {
        if (surfacesToFree[i]!=NULL) {
            SDL_FreeSurface(surfacesToFree[i]);
            surfacesToFree[i]=NULL;
        }
    }
}*/

void drawBorderedRect(int px,int py,int wid,int hei,Uint32 fillColor,Uint32 borderColor) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex,&newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(gRenderer,getR(fillColor),getG(fillColor),getB(fillColor),getA(fillColor));
    SDL_RenderFillRect(gRenderer,&stretchRect);
    SDL_SetRenderDrawColor(gRenderer,getR(borderColor),getG(borderColor),getB(borderColor),getA(borderColor));
    SDL_RenderDrawRect(gRenderer,&stretchRect);
}


void drawBorderedRect(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex,&newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(gRenderer,r,g,b,a);
    SDL_RenderFillRect(gRenderer,&stretchRect);
    SDL_SetRenderDrawColor(gRenderer,0x00,0x00,0x00,a);
    SDL_RenderDrawRect(gRenderer,&stretchRect);
}
void drawRect(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(gRenderer,r,g,b,a);
    SDL_RenderFillRect(gRenderer,&stretchRect);
}

SDL_Surface* flipSurfaceOverXAxis(SDL_Surface* toFlip) {
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, toFlip->w, toFlip->h, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_LockSurface(surfaceToDraw);
    for (int x = 0;x<toFlip->w;x++) {
        for (int y = 0;y<toFlip->h;y++) {
            put_pixel32(surfaceToDraw, x, toFlip->h-y-1, get_pixel32(toFlip, x, y));//If height 30 & at position 0, we want it to be 29 since it
                                                                                    //only has range 0-29, likewise 30@1 -> 28, etc.  Hence the -1
        }
    }
    SDL_UnlockSurface(surfaceToDraw);
    return surfaceToDraw;
}

SDL_Surface* flipSurfaceOverYAxis(SDL_Surface* toFlip) {
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, toFlip->w, toFlip->h, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_LockSurface(surfaceToDraw);
    for (int x = 0;x<toFlip->w;x++) {
        for (int y = 0;y<toFlip->h;y++) {
            put_pixel32(surfaceToDraw, toFlip->w-x-1, y, get_pixel32(toFlip, x, y));//If width 30 & at position 0, we want it to be 29 since it
                                                                                    //only has range 0-29, likewise 30@1 -> 28, etc.  Hence the -1
        }
    }
    SDL_UnlockSurface(surfaceToDraw);
    return surfaceToDraw;
}

/*void drawLine(int x,int y,int x2,int y2,Uint32 color) {
    int l_x = (x2>x)?x2:x;
    int l_y = (y2>y)?y2:y;
    int s_x = (x2>x)?x:x2;
    int s_y = (y2>y)?y:y2;
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, l_x-s_x, l_y-s_y, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    drawLineOnSurface(surfaceToDraw, 0, 0, l_x, l_y, color);
    SDL_Texture* toDraw = SDL_CreateTextureFromSurface(gRenderer, surfaceToDraw);
    SDL_Rect dstrect;
    dstrect.x = x-s_x;
    dstrect.y = y-s_y;
    dstrect.w = l_x-s_x;
    dstrect.h = l_y-s_y;
    //dstrect.x = s_x;
    //dstrect.y = s_y;
    //dstrect.w = l_x-s_x;
    //dstrect.h = l_y-s_y;
    SDL_RenderCopy(gRenderer, toDraw, NULL, &dstrect);
    SDL_FreeSurface(surfaceToDraw);
    SDL_DestroyTexture(toDraw);
}*/

/*Uint32 invertColor(Uint32 color) {        //commented out because it turns out I already wrote this function XD
    Uint8 r = (color&0x00ff0000)/0x00010000;
    Uint8 g = (color&0x0000ff00)/0x00000100;
    Uint8 b = (color&0x000000ff);//0x00000001;
    r = 0xff-r;
    g = 0xff-g;
    b = 0xff-b;
    return 0xff000000+r*0x00010000+g*0x00000100+b;// *0x00000001
}*/

SDL_Surface* rotateSurfaceByAngleAboutCenter_radians(SDL_Surface* inSurf,float angle,int* resizeOffsetX,int* resizeOffsetY) {
    //first two arguments are self-explanatory, surface to be rotated and the angle to rotate by
    //the second two are because the image necessarily gets resized by the rotation (or else it wouldn't fit!)
    //so those are pointers to the values that, when added to the surface's old display position, produces the correct position
    //(adding the offsets is translating the new surface's center to overlap the old surface's center)


    float cos_theta;// = cosf(angle);
    float sin_theta;// = sinf(angle);
    fastSineCosine_float(&sin_theta, &cos_theta, angle);
    int offx = 0;
    int offy = 0;
    float centerX = inSurf->w/2;
    float centerY = inSurf->h/2;

    int topleft_prime_x = cos_theta*(0-centerX)-sin_theta*(0-centerY)+centerX;
    int topleft_prime_y = sin_theta*(0-centerX)+cos_theta*(0-centerY)+centerY;
    int topright_prime_x = cos_theta*(inSurf->w-centerX)-sin_theta*(0-centerY)+centerX;
    int topright_prime_y = sin_theta*(inSurf->w-centerX)+cos_theta*(0-centerY)+centerY;
    int bottomleft_prime_x = cos_theta*(0-centerX)-sin_theta*(inSurf->h-centerY)+centerX;
    int bottomleft_prime_y = sin_theta*(0-centerX)+cos_theta*(inSurf->h-centerY)+centerY;
    int bottomright_prime_x = cos_theta*(inSurf->w-centerX)-sin_theta*(inSurf->h-centerY)+centerX;
    int bottomright_prime_y = sin_theta*(inSurf->w-centerX)+cos_theta*(inSurf->h-centerY)+centerY;
    int leftmost = topleft_prime_x;
    int rightmost = topleft_prime_x;
    int topmost = topleft_prime_y;
    int bottommost = topleft_prime_y;
    leftmost = (topright_prime_x<leftmost)?topright_prime_x:leftmost;
    leftmost = (bottomleft_prime_x<leftmost)?bottomleft_prime_x:leftmost;
    leftmost = (bottomright_prime_x<leftmost)?bottomright_prime_x:leftmost;
    rightmost = (topright_prime_x>rightmost)?topright_prime_x:rightmost;
    rightmost = (bottomleft_prime_x>rightmost)?bottomleft_prime_x:rightmost;
    rightmost = (bottomright_prime_x>rightmost)?bottomright_prime_x:rightmost;
    topmost = (topright_prime_y<topmost)?topright_prime_y:topmost;
    topmost = (bottomleft_prime_y<topmost)?bottomleft_prime_y:topmost;
    topmost = (bottomright_prime_y<topmost)?bottomright_prime_y:topmost;
    bottommost = (topright_prime_y>bottommost)?topright_prime_y:bottommost;
    bottommost = (bottomleft_prime_y>bottommost)?bottomleft_prime_y:bottommost;
    bottommost = (bottomright_prime_y>bottommost)?bottomright_prime_y:bottommost;
    //offx = leftmost+centerX;
    //offy = topmost+centerY;
    int newsizex = rightmost-leftmost;
    int newsizey = bottommost-topmost;
    
    SDL_Surface* toReturn = SDL_CreateRGBSurface(NULL, newsizex, newsizey, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    SDL_LockSurface(inSurf);
    SDL_LockSurface(toReturn);
    float newCenterX = toReturn->w/2;
    float newCenterY = toReturn->h/2;
    
    
    //vvv works but automatically centers the text, which we don't want
    offx = leftmost-centerX;
    offy = topmost-centerY;
    //To move to its correct position, consider that if it was not rotated we'd move it py (newsizex/2,newsizey/2)
    //However, it has been rotated!  By angle - how do we move it?
    //Well, let's call the old movement vector V : ||V|| is how much to move it, and sin_theta and cos_theta give us
    //the direction
    float mag_V = sqrtf(newsizex*newsizex/4.0+newsizey*newsizey/4.0);
    offx += mag_V*cos_theta;
    offy += mag_V*sin_theta;
    //(wow, that worked first try, go me!)
    //since putting an image at its center can be useful, maybe add in an optional boolean paramater defaulting
    //to false which is something like offsetToCenter or something.
    
    
    
    //cos_theta = cosf(-angle);//compute things backwards to fill every pixel
    //sin_theta = sinf(-angle);
    fastSineCosine_float(&sin_theta, &cos_theta, -angle);//compute things backwards to fill every pixel
    for (int x = 0;x<toReturn->w;x++) {
        for (int y = 0;y<toReturn->h;y++) {
            //What's going on here?:
            //Normal formula is cos_theta*x-cos_theta*y = x_prime
            //But that's when rotating about the origin.  We want to rotate about the center
            //We can rotate about the center by translating it to the origin.
            //Since we're doing this backwards (computing where a potentially rotated point was before
            //the rotation & then grabbing it's old position's color), the 'center' we are translating
            //to the origin is (newCenterX,newCenterY), not (centerX,centerY).
            //This gives us the formula: cos_theta*(x-newCenterX)-sin_theta*(y-centerY)=x_prime
            //But now we have a shape centered at the origin - we don't want that, it should
            //be centered at its actual center.  So, after the rotations, we should
            //translate it back - however, we're no longer talking about our old image,
            //but our new image, which has the center (centerX,centerY),
            //which gives us cos_theta*(x-newCenterX)-sin_theta*(y-newCenterY)+centerX=x_prime
            //Exact same logic derives the formula for y_prime
            int x_prime = cos_theta*(x-newCenterX)-sin_theta*(y-newCenterY)+centerX;
            int y_prime = sin_theta*(x-newCenterX)+cos_theta*(y-newCenterY)+centerY;
            if (pointInBounds(x_prime, y_prime, 0, inSurf->w-1, 0, inSurf->h-1)) {
                put_pixel32(toReturn, x, y, get_pixel32(inSurf, x_prime, y_prime));
            }
            else {
                //For debugging, to see size of new object:
                //put_pixel32(toReturn, x, y, 0xffffffff);
            }
        }
    }

    SDL_UnlockSurface(inSurf);
    SDL_UnlockSurface(toReturn);
    *resizeOffsetX = offx;
    *resizeOffsetY = offy;
    return toReturn;
}

SDL_Surface* rotateSurfaceNinetyDegreesCounterClockwise(SDL_Surface* toFlip) {
    /*
        1 1 0    0 1 1
        0 1 1 -> 1 1 1
        0 1 1    1 0 0
        Basically, row n becomes column n in reverse order
        Or basically, column n becomes row size-n-1 if starting at 0
     
        1 2 3 4     4 8 C
        5 6 7 8 ->  3 7 B
        9 A B C     2 6 A
                    1 5 9
        Top left is 0,0:
        0,0 -> 0,3 AKA 0,size-1
        0,1 -> 1,3 AKA 1,size-1
        1,0 -> 0,2 AKA 0,size-2
     
        DEDUCTION: point x,y -> y,newsizey-1-x
        newsizey = oldsizex
        (x,y) -> (y,oldsizex-1-x)
    */
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, toFlip->h, toFlip->w, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    //drawGraphic(100, 100, toFlip->w, toFlip->h, SDL_CreateTextureFromSurface(gRenderer, toFlip));//debug-only
    SDL_LockSurface(surfaceToDraw);
    for (int x = 0;x<toFlip->w;x++) {
        for (int y = 0;y<toFlip->h;y++) {
            put_pixel32(surfaceToDraw, y, toFlip->w-1-x, get_pixel32(toFlip, x, y));//If height 30 & at position 0, we want it to be 29 since it
                                                                                    //only has range 0-29, likewise 30@1 -> 28, etc.  Hence the -1
        }
    }
    SDL_UnlockSurface(surfaceToDraw);
    return surfaceToDraw;
    /*
    SDL_Surface* surfaceToDraw = SDL_CreateRGBSurface(NULL, toFlip->w, toFlip->h, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_LockSurface(surfaceToDraw);
    for (int x = 0;x<toFlip->w;x++) {
        for (int y = 0;y<toFlip->h;y++) {
            put_pixel32(surfaceToDraw, y, toFlip->h-x-1, get_pixel32(toFlip, x, y));//If height 30 & at position 0, we want it to be 29 since it
                                                                                    //only has range 0-29, likewise 30@1 -> 28, etc.  Hence the -1
        }
    }
    SDL_UnlockSurface(surfaceToDraw);
    return surfaceToDraw;
    */
}


void drawGraphic (int px,int py,int wid,int hei,SDL_Texture* graph) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_RenderCopy(gRenderer,graph,NULL,&stretchRect);
    //SDL_RenderCopyEx(gRenderer,graph,NULL,&stretchRect,0,NULL,SDL_FLIP_NONE);
}

void drawGraphicFlipped (int px,int py,int wid,int hei,SDL_Texture* graph,SDL_RendererFlip flipflag) {
    int newSizex;
    int newSizey;
    SDL_GetWindowSize(gWindow, &newSizex, &newSizey);
    stretchRect.x = px*(newSizex)/SCREEN_WIDTH;
    stretchRect.y = py*(newSizey)/SCREEN_HEIGHT;
    stretchRect.w = wid*(newSizex)/SCREEN_WIDTH;
    stretchRect.h = hei*(newSizey)/SCREEN_HEIGHT;
    SDL_RenderCopyEx(gRenderer,graph,NULL,&stretchRect,0,NULL,flipflag);

}

int getWidthTakenByDrawingText(std::string text,int text_size) {
    if (text=="") {
        return 0;//Not drawing anything
    }
    SDL_Color textColor = {0,0,0};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
    return text_width;
}

int getHeightTakenByDrawingTextInBorder(std::string text,int text_size,int w,int h) {
    if (text=="") {
        return 0;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {0,0,0};
    std::vector<std::string> words = split(text, ' ');
    std::vector<SDL_Surface*> surfaceMessages;
    for (int iterator = 0;iterator < words.size();iterator++) {
        std::string toDraw = words[iterator];
        if (iterator<words.size()-1) {
            toDraw = toDraw+" ";
        }
        if (text_size==16) {
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont16,toDraw.c_str(),textColor));
        }
        else if (text_size==20) {
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont20,toDraw.c_str(),textColor));
        }
        else {//Default 24
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont24,toDraw.c_str(),textColor));
        }
        if(surfaceMessages[surfaceMessages.size()-1] == NULL) {
            printf("Unable to render text surface: %s\n",TTF_GetError());
        }
    }
    SDL_Surface* actualMessage = createBlankSurfaceWithSize(w, h);
    int widthTaken = 0;
    int heightTaken = 0;
    for (int iterator = 0;iterator<surfaceMessages.size();iterator++) {
        int text_width = surfaceMessages[iterator]->w;
        int text_height = surfaceMessages[iterator]->h;
        if (widthTaken+text_width < w) {//Can fit
            SDL_Rect destrect;
            destrect.x = widthTaken;
            destrect.y = heightTaken;
            destrect.w = text_width;
            destrect.h = text_height;
            SDL_BlitSurface(surfaceMessages[iterator],NULL,actualMessage,&destrect);
            widthTaken+=text_width;
        }
        else if (widthTaken==0) { //Always place word if width too small for it to ever fit, but cut it off
            SDL_Rect destrect;
            destrect.x = widthTaken;
            destrect.y = heightTaken;
            destrect.w = w;
            destrect.h = text_height;
            SDL_BlitSurface(surfaceMessages[iterator],NULL,actualMessage,&destrect);
            widthTaken=0;
            heightTaken+=text_height;
        }
        else {//Can't fit, increment height and reset widthTaken
            widthTaken = 0;
            heightTaken+=text_height;
            iterator-=1;//Step backwards so we can try this again
        }
    }

    for (int iterator = 0;iterator<surfaceMessages.size();iterator++) {
        SDL_FreeSurface(surfaceMessages[iterator]);
        surfaceMessages[iterator]=NULL;
    }
    return heightTaken+text_size;//the +text_size to start a new line after the text;
}
int drawTextInBorderSmartWrap(std::string text,int text_size,int x,int y, int w,int h,Uint8 r,Uint8 g,Uint8 b) {
    if (text=="") {
        return 0;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {r,g,b};
    std::vector<std::string> words = split(text, ' ');
    std::vector<SDL_Surface*> surfaceMessages;
    for (int iterator = 0;iterator < words.size();iterator++) {
        std::string toDraw = words[iterator];
        if (iterator<words.size()-1) {
            toDraw = toDraw+" ";
        }
        if (text_size==16) {
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont16,toDraw.c_str(),textColor));
        }
        else if (text_size==20) {
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont20,toDraw.c_str(),textColor));
        }
        else {//Default 24
            surfaceMessages.push_back(TTF_RenderUTF8_Solid(theFont24,toDraw.c_str(),textColor));
        }
        if(surfaceMessages[surfaceMessages.size()-1] == NULL) {
            printf("Unable to render text surface: %s\n",TTF_GetError());
        }
    }
    SDL_Surface* actualMessage = createBlankSurfaceWithSize(w, h);
    int widthTaken = 0;
    int heightTaken = 0;
    for (int iterator = 0;iterator<surfaceMessages.size();iterator++) {
        int text_width = surfaceMessages[iterator]->w;
        int text_height = surfaceMessages[iterator]->h;
        if (widthTaken+text_width < w) {//Can fit
            SDL_Rect destrect;
            destrect.x = widthTaken;
            destrect.y = heightTaken;
            destrect.w = text_width;
            destrect.h = text_height;
            SDL_BlitSurface(surfaceMessages[iterator],NULL,actualMessage,&destrect);
            widthTaken+=text_width;
        }
        else if (widthTaken==0) { //Always place word if width too small for it to ever fit, but cut it off
            SDL_Rect destrect;
            destrect.x = widthTaken;
            destrect.y = heightTaken;
            destrect.w = w;
            destrect.h = text_height;
            SDL_BlitSurface(surfaceMessages[iterator],NULL,actualMessage,&destrect);
            widthTaken=0;
            heightTaken+=text_height;
        }
        else {//Can't fit, increment height and reset widthTaken
            widthTaken = 0;
            heightTaken+=text_height;
            iterator-=1;//Step backwards so we can try this again
        }
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,actualMessage);
    drawGraphic(x, y, w, h, message);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(actualMessage);
    message=NULL;
    for (int iterator = 0;iterator<surfaceMessages.size();iterator++) {
        SDL_FreeSurface(surfaceMessages[iterator]);
        surfaceMessages[iterator]=NULL;
    }
    return heightTaken+text_size;//the +text_size to start a new line after the text
}

void drawTextWithBackground(std::string text,int text_size,int x,int y, Uint32 txtColor, Uint32 bgColor, Uint32 edgeColor) {
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {getR(txtColor),getG(txtColor),getB(txtColor)};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        //surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    drawBorderedRect(x, y, text_width, text_height, bgColor, edgeColor);
    drawGraphic(x, y, text_width, text_height, message);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

void drawTextInBorder(std::string text,int text_size,int x,int y, int w,int h,Uint8 r,Uint8 g,Uint8 b) {
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {r,g,b};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    SDL_Surface* actualMessage = createBlankSurfaceWithSize(w, h);
    for (int iterator = 0;text_width>w&&iterator*text_height<h;iterator++) {
        SDL_Rect dstrect;
        dstrect.x = 0;
        dstrect.y = iterator*text_height;
        dstrect.w = w;
        dstrect.h = text_height;
        SDL_Rect srcrect;
        srcrect.x = iterator*w;
        srcrect.y = 0;
        srcrect.w = w;
        srcrect.h = text_height;
        SDL_BlitSurface(surfaceMessage,&srcrect,actualMessage,&dstrect);
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,actualMessage);
    drawGraphic(x, y, w, h, message);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

SDL_Surface* createBlankSurfaceWithSize(int sx,int sy) {
    return SDL_CreateRGBSurface(NULL, sx, sy, 32,0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
}

void drawTextAtAngle(std::string text,int text_size,int x,int y, Uint32 color,int angle) {
    if ((angle)%90!=0) {
        throw std::runtime_error("Error: drawTextAtAngle only accepts degrees, and multiples of 90! Sorry");
    }
    angle%=360;
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {getR(color),getG(color),getB(color)};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        //surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    //SDL_Surface* toDraw = SDL_ConvertSurface(surfaceMessage, gScreenSurface->format, NULL);
    //SDL_ConvertPixels(surfaceMessage->w, surfaceMessage->h, surfaceMessage->format->format, surfaceMessage->pixels, surfaceMessage->pitch, gScreenSurface->format->format, toDraw->pixels, toDraw->pitch);
    SDL_Surface* toDraw = SDL_ConvertSurfaceFormat(surfaceMessage, gScreenSurface->format->format, NULL);
    //SDL_Surface* toDraw = SDL_ConvertSurface(surfaceMessage, gScreenSurface->format, NULL);
    //drawGraphic(300, 300, toDraw->w, toDraw->h, SDL_CreateTextureFromSurface(gRenderer, toDraw));//debug
    if (angle>=90) {
        //SDL_FreeSurface(toDraw);
        SDL_Surface* temp = toDraw;//SDL_ConvertSurface(toDraw, gScreenSurface->format, NULL);
        toDraw = rotateSurfaceNinetyDegreesCounterClockwise(temp);
        SDL_FreeSurface(temp);
        //drawGraphic(300, 300, toDraw->w, toDraw->h, SDL_CreateTextureFromSurface(gRenderer, toDraw));//debug
        if (angle>=180) {
            temp = toDraw;
            toDraw = rotateSurfaceNinetyDegreesCounterClockwise(temp);
            SDL_FreeSurface(temp);
            if (angle>=270) {
                temp = toDraw;
                toDraw = rotateSurfaceNinetyDegreesCounterClockwise(temp);
                SDL_FreeSurface(temp);
            }
        }
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,toDraw);
    int text_width = toDraw->w;
    int text_height = toDraw->h;
    drawGraphic(x, y, text_width, text_height, message);
    //drawGraphic(x, y, text_height, text_width, SDL_CreateTextureFromSurface(gRenderer,surfaceMessage));//DEBUG
    SDL_FreeSurface(toDraw);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

SDL_Texture* getTextAsTexture(std::string text,int text_size,Uint32 color,float angle,int* offsetX,int* offsetY) {
    SDL_Surface* temp = getTextAsSurface(text, text_size, color, angle, offsetX, offsetY);
    SDL_Texture* toReturn = SDL_CreateTextureFromSurface(gRenderer, temp);
    SDL_FreeSurface(temp);
    return toReturn;
}

SDL_Surface* getTextAsSurface(std::string text,int text_size,Uint32 color,float angle,int* offsetX,int* offsetY) {
    if (text=="") {
        return createBlankSurfaceWithSize(1, 1);
    }
    SDL_Color textColor = {getR(color),getG(color),getB(color)};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        //surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    SDL_Surface* toReturn = SDL_ConvertSurfaceFormat(surfaceMessage, gScreenSurface->format->format, NULL);
    if (angle!=0) {
        toReturn = rotateSurfaceByAngleAboutCenter_radians(toReturn, angle, offsetX, offsetY);
    }
    SDL_FreeSurface(surfaceMessage);
    return toReturn;
}

void drawText(std::string text,int text_size,int x,int y, Uint32 color) {
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {getR(color),getG(color),getB(color)};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        //surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
        throw std::runtime_error("Error unable to render text surface!");
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    drawGraphic(x, y, text_width, text_height, message);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

void drawCenteredText(std::string text,int text_size,int x,int y,int w,int h,Uint32 color) {
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {getR(color),getG(color),getB(color)};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        //surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    //SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    SDL_Surface* toCenterOn = createBlankSurfaceWithSize(w, h);
    SDL_Rect destrect;
    destrect.x = w/2-text_width/2;
    destrect.y = h/2-text_height/2;
    destrect.w = text_width;
    destrect.h = text_height;
    SDL_BlitSurface(surfaceMessage,NULL,toCenterOn,&destrect);
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer, toCenterOn);
    drawGraphic(x, y, w, h, message);
    SDL_FreeSurface(surfaceMessage);
    SDL_FreeSurface(toCenterOn);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

void drawText(std::string text,int text_size,int x,int y, Uint8 r,Uint8 g,Uint8 b) {
    if (text=="") {
        return;//Why draw if not drawing anything?
    }
    SDL_Color textColor = {r,g,b};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    drawGraphic(x, y, text_width, text_height, message);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    message=NULL;
    surfaceMessage=NULL;
}

void drawTextToSurface(std::string text,int text_size,int x,int y, Uint8 r,Uint8 g,Uint8 b,SDL_Surface* surf) {
    SDL_Color textColor = {r,g,b};
    SDL_Surface* surfaceMessage;
    if (text_size==16) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont16,text.c_str(),textColor);
    }
    else if (text_size==20) {
        surfaceMessage = TTF_RenderUTF8_Solid(theFont20,text.c_str(),textColor);
    }
    else {//Default 24
        surfaceMessage = TTF_RenderUTF8_Solid(theFont24,text.c_str(),textColor);
    }
    if(surfaceMessage == NULL) {
        printf("Unable to render text surface: %s\n",TTF_GetError());
    }
    //SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer,surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    SDL_Rect textRect{x,y,text_width,text_height};
    //std::cout<<x<<"\n";
    //SDL_RenderCopy(gRenderer,message,NULL,&textRect);
    SDL_BlitSurface(surfaceMessage,NULL,surf,&textRect);
    SDL_FreeSurface(surfaceMessage);
    //SDL_DestroyTexture(message);
    //message=NULL;
    surfaceMessage=NULL;
}

void quadraticBezier(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float cx,float cy,Uint32 color,int thicknessRadius,float increment) {
    //Puts quadratic bezier curve on toDraw
    //from (x1,y1) to (x3,y3) with control (cx,cy)
    //Formula for Quadratic Bezier: B(t) = (1-t)^2*P_1+2(1-t)t*P_C+t^2*P_2
    //Formula for Derivative of Quadratic Bezier: B'(t) = 2(1-t)(P_C-P_1)+2t(P_2-P_C)
    //Negative resiprocal is line perpendicular, so let ∆ = -1/B'(t)
    //Normalize ∆ so that each step is 1 distance away
    //At any point on the curve, add (+/-)thicknessRadius*∆ to make it thicker
    float t = 0;//t goes from 0 to 1
    while (t<=1) {
        //Calculate the location of a point on the curve
        int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
        int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
        //Put the point down
        if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
            put_pixel32(toDraw, x_to_put, y_to_put, color);
        }
        if (thicknessRadius>1) {
            //Calculate the derivative
            float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
            float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
            //head perpendicular to it
            float direc_x = -1.0/deriv_x;
            float direc_y = -1.0/deriv_y;
            //normalize the gradient
            direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
            direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
            //draw it
            for (int i = 1;i<thicknessRadius;i++) {
                put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
            }
        }
        t+= increment;
    }
}

void quadraticBezierWithEdges(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float cx,float cy,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment) {
    //Puts quadratic bezier curve on toDraw
    //from (x1,y1) to (x3,y3) with control (cx,cy)
    //with the edges of the curve a different color
    //than the center
    //Formula for Quadratic Bezier: B(t) = (1-t)^2*P_1+2(1-t)t*P_C+t^2*P_2
    //Formula for Derivative of Quadratic Bezier: B'(t) = 2(1-t)(P_C-P_1)+2t(P_2-P_C)
    //Negative resiprocal is line perpendicular, so let ∆ = -1/B'(t)
    //Normalize ∆ so that each step is 1 distance away
    //At any point on the curve, add (+/-)thicknessRadius*∆ to make it thicker
    float t = 0;//t goes from 0 to 1
    while (t<=1) {
        //Calculate the location of a point on the curve
        int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
        int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
        //Put the point down
        if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
            put_pixel32(toDraw, x_to_put, y_to_put, color);
        }
        if (thicknessRadius>1) {
            //Calculate the derivative
            float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
            float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
            //head perpendicular to it
            float direc_x = -1.0/deriv_x;
            float direc_y = -1.0/deriv_y;
            //normalize the gradient
            direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
            direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
            //draw it
            for (int i = 1;i<=thicknessRadius;i++) {
                if (i>thicknessRadius-edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
            for (int i = -1;i>=-thicknessRadius;i--) {
                if (i<-thicknessRadius+edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
        }
        t+= increment;
    }
}

void cubicBezier(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4,Uint32 color,int thicknessRadius,float increment) {
    //Puts cubic bezier curve on toDraw
    //from (x1,y1) to (x3,y3)
    //Formula for Bezier Curve: B(t)=(1-t)^3*P_1+3(1-t)^2*t*P_2+3(1-t)t^2*P_3+t^3*P_4
    //Formula for Derivative of Bezier Curve: B'(t)=3(1-t)^2(P_2-P_1)+6(1-t)t(P_3-P_2)+3t^2(P_4-P_3)
    //Negative resiprocal is line perpendicular, so let ∆ = -1/B'(t)
    //Normalize ∆ so that each step is 1 distance away
    //At any point on the curve, add (+/-)thicknessRadius*∆ to make it thicker
    float t = 0;//t goes from 0 to 1
    while (t<=1) {
        int x_to_put = pow(1-t,3)*x1+3*pow(1-t,2)*t*x2+3*(1-t)*pow(t,2)*x3+pow(t,3)*x4;
        int y_to_put = pow(1-t,3)*y1+3*pow(1-t,2)*t*y2+3*(1-t)*pow(t,2)*y3+pow(t,3)*y4;
        if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
            put_pixel32(toDraw, x_to_put, y_to_put, color);
        }
        if (thicknessRadius>1) {
            //calculate derivative
            float deriv_x = 3*pow(1-t,2)*(x2-x1)+6*(1-t)*t*(x3-x2)+3*pow(t, 2)*(x4-x3);
            float deriv_y = 3*pow(1-t,2)*(y2-y1)+6*(1-t)*t*(y3-y2)+3*pow(t, 2)*(y4-y3);
            //head perpendicular to it
            float direc_x = -1.0/deriv_x;
            float direc_y = -1.0/deriv_y;
            //normalize the gradient
            float tdirx = direc_x;
            float tdiry = direc_y;
            direc_x = tdirx/sqrt(tdirx*tdirx+tdiry*tdiry);
            direc_y = tdiry/sqrt(tdirx*tdirx+tdiry*tdiry);
            //draw it
            for (int i = 1;i<=thicknessRadius;i++) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
            }
            for (int i = -1;i>=-thicknessRadius;i--) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
            }
        }
        t+=increment;
    }
}

void funkyCurveWithEdges(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,float firstControlX,float firstControlY,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment) {
    //Draws composite quadratic bezier curve on toDraw
    //It combines the curves using the fact that the
    //second derivative of a bezier curve is 2(P_2-2*P_1+P_0)
    //where P_0 is the start, P_2 is the end, and P_1 is the control
    //So if we want a "smooth" piecewise function  of 2 beziers,
    //we set their first derivatives equal and solve for control
    //note: this gets funky because the control will then be made in terms of time
    //cross your fingers that this works!
    float t = 0;//goes from 0 to 1 per curve
    while (t<=1) {//Draw the first curve
        //Calculate the location of a point on the curve
        float x1 = pointsToPassThroughX[0];
        float x2 = pointsToPassThroughX[1];
        float y1 = pointsToPassThroughY[0];
        float y2 = pointsToPassThroughY[1];
        float cx = firstControlX;
        float cy = firstControlY;
        int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
        int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
        //Put the point down
        if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
            put_pixel32(toDraw, x_to_put, y_to_put, color);
        }
        if (thicknessRadius>1) {
            //Calculate the derivative
            float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
            float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
            //head perpendicular to it
            float direc_x = -1.0/deriv_x;
            float direc_y = -1.0/deriv_y;
            //normalize the gradient
            direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
            direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
            //draw it
            for (int i = 1;i<=thicknessRadius;i++) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                if (i>thicknessRadius-edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
            for (int i = -1;i>=-thicknessRadius;i--) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                if (i<-thicknessRadius+edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
        }
        t+= increment;
    }
    float prevControlX = firstControlX;
    float prevControlY = firstControlY;
    for (int j = 1;j<pointsToPassThroughX.size()-1;j++) {//Now deal with the other curves
        t = 0;//goes from 0 to 1 per curve
        //Calculate the location of a point on the curve
        float x1 = pointsToPassThroughX[j];
        float x2 = pointsToPassThroughX[j+1];
        float y1 = pointsToPassThroughY[j];
        float y2 = pointsToPassThroughY[j+1];
        //float cx = prevControlX-0.5*pointsToPassThroughX[j-1]+0.5*pointsToPassThroughX[j+1];
        //float cy = prevControlY-0.5*pointsToPassThroughY[j-1]+0.5*pointsToPassThroughY[j+1];
        //prevControlX = cx;
        //prevControlY = cy;
        float cx=0;
        float cy=0;
        while (t<=1) {//Draw the first curve
            cx = ((2*t-2)*x1+(2-2*t)*x2)/(2-4*t)+prevControlX;
            cy = ((2*t-2)*y1+(2-2*t)*y2)/(2-4*t)+prevControlY;
            int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
            int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
            //Put the point down
            if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
                put_pixel32(toDraw, x_to_put, y_to_put, color);
            }
            if (thicknessRadius>1) {
                //Calculate the derivative
                float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
                float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
                //head perpendicular to it
                float direc_x = -1.0/deriv_x;
                float direc_y = -1.0/deriv_y;
                //normalize the gradient
                direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
                direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
                //draw it
                for (int i = 1;i<=thicknessRadius;i++) {
                    if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                        continue;//is out of bounds
                    }
                    if (i>thicknessRadius-edgeSize) {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                    }
                    else {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                    }
                }
                for (int i = -1;i>=-thicknessRadius;i--) {
                    if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                        continue;//is out of bounds
                    }
                    if (i<-thicknessRadius+edgeSize) {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                    }
                    else {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                    }
                }
            }
           t+= increment;
        }
        prevControlX = cx;
        prevControlY = cy;
    }
}


void compositeQuadraticBezierWithEdges(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,float firstControlX,float firstControlY,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment) {
    //Draws composite quadratic bezier curve on toDraw
    //It combines the curves using the fact that the
    //second derivative of a bezier curve is 2(P_2-2*P_1+P_0)
    //where P_0 is the start, P_2 is the end, and P_1 is the control
    //So if we want a "smooth" piecewise function  of 2 beziers,
    //we set their second derivatives equal to eachother
    //So if pointsToPassThrough is abbreviated ppt:
    //and we denote the control for point i as cnt[i]
    //2(ppt_x[i+1]-2*cnt_x[i]+ppt_x[i])=2(ppt_x[i]-2*cnt_x[i-1]+ppt_x[i-1])
    //and a similar equation for y values
    //then one can solve for the new control point:
    //cnt_x[i]=cnt_x[i-1]-0.5*ppt_x[i-1]+0.5*ppt_x[i+1]
    float t = 0;//goes from 0 to 1 per curve
    while (t<=1) {//Draw the first curve
        //Calculate the location of a point on the curve
        float x1 = pointsToPassThroughX[0];
        float x2 = pointsToPassThroughX[1];
        float y1 = pointsToPassThroughY[0];
        float y2 = pointsToPassThroughY[1];
        float cx = firstControlX;
        float cy = firstControlY;
        int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
        int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
        //Put the point down
        if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
            put_pixel32(toDraw, x_to_put, y_to_put, color);
        }
        if (thicknessRadius>1) {
            //Calculate the derivative
            float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
            float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
            //head perpendicular to it
            float direc_x = -1.0/deriv_x;
            float direc_y = -1.0/deriv_y;
            //normalize the gradient
            direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
            direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
            //draw it
            for (int i = 1;i<=thicknessRadius;i++) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                if (i>thicknessRadius-edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
            for (int i = -1;i>=-thicknessRadius;i--) {
                if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                    continue;//is out of bounds
                }
                if (i<-thicknessRadius+edgeSize) {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                }
                else {
                    put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                }
            }
        }
        t+= increment;
    }
    float prevControlX = firstControlX;
    float prevControlY = firstControlY;
    for (int j = 1;j<pointsToPassThroughX.size()-1;j++) {//Now deal with the other curves
        t = 0;//goes from 0 to 1 per curve
        //Calculate the location of a point on the curve
        float x1 = pointsToPassThroughX[j];
        float x2 = pointsToPassThroughX[j+1];
        float y1 = pointsToPassThroughY[j];
        float y2 = pointsToPassThroughY[j+1];
        //cnt_x[i]=cnt_x[i-1]-0.5*ppt_x[i-1]+0.5*ppt_x[i+1]
        float cx = prevControlX-0.5*pointsToPassThroughX[j-1]+0.5*pointsToPassThroughX[j+1];
        float cy = prevControlY-0.5*pointsToPassThroughY[j-1]+0.5*pointsToPassThroughY[j+1];
        prevControlX = cx;
        prevControlY = cy;
        while (t<=1) {//Draw the first curve
            int x_to_put = pow(1-t,2)*x1+2*(1-t)*t*cx+pow(t,2)*x2;
            int y_to_put = pow(1-t,2)*y1+2*(1-t)*t*cy+pow(t,2)*y2;
            //Put the point down
            if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
                put_pixel32(toDraw, x_to_put, y_to_put, color);
            }
            if (thicknessRadius>1) {
                //Calculate the derivative
                float deriv_x = 2*(1-t)*(cx-x1)+2*t*(x2-cx);
                float deriv_y = 2*(1-t)*(cy-y1)+2*t*(y2-cy);
                //head perpendicular to it
                float direc_x = -1.0/deriv_x;
                float direc_y = -1.0/deriv_y;
                //normalize the gradient
                direc_x /= sqrt(direc_x*direc_x+direc_y*direc_y);
                direc_y /= sqrt(direc_x*direc_x+direc_y*direc_y);
                //draw it
                for (int i = 1;i<=thicknessRadius;i++) {
                    if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                        continue;//is out of bounds
                    }
                    if (i>thicknessRadius-edgeSize) {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                    }
                    else {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                    }
                }
                for (int i = -1;i>=-thicknessRadius;i--) {
                    if (!(x_to_put+direc_x*i>=0&&x_to_put+direc_x*i<toDraw->w&&y_to_put+direc_y*i>=0&&y_to_put+direc_y*i<toDraw->h)) {
                        continue;//is out of bounds
                    }
                    if (i<-thicknessRadius+edgeSize) {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, edgeColor);
                    }
                    else {
                        put_pixel32(toDraw, x_to_put+direc_x*i, y_to_put+direc_y*i, color);
                    }
                }
            }
           t+= increment;
        }
    }
}

void drawLinesFromCollectionOfPoints(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,Uint32 color) {
    for (int j = 0;j<pointsToPassThroughX.size()-1;j++) {
        int startx = pointsToPassThroughX[j];
        int starty = pointsToPassThroughY[j];
        int endx = pointsToPassThroughX[j+1];
        int endy = pointsToPassThroughY[j+1];
        float delx = (endx-startx)/sqrt(pow(endx-startx,2)+pow(endy-starty, 2));
        float dely = (endy-starty)/sqrt(pow(endx-startx,2)+pow(endy-starty, 2));
        float numIterations = (endx-startx)/delx;
        if ((endy-starty)/dely>numIterations) {
            numIterations = (endy-starty)/dely;
        }
        for (int k = 0;k<ceil(numIterations);k++) {
            float x_to_put = startx+delx*k;
            float y_to_put = starty+dely*k;
            if (x_to_put>=0&&x_to_put<toDraw->w&&y_to_put>=0&&y_to_put<toDraw->h) {
                put_pixel32(toDraw, x_to_put, y_to_put, color);
            }
        }
    }
}

void drawRoundedLinesetFromCollectionOfPoints(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,Uint32 color,Uint32 edgeColor,int thickness) {
    //Draws a curve on toDraw that passes near every point in the pTPT vectors with color color.
    //This curve is simply lines drawn through the points in order, but at each junction the edge is rounded by a circle
    //The circle will cut the smallest line off at its midpoint
    if (pointsToPassThroughX.size()<2) {
        return;//don't do anything, not enough points
    }
    if (pointsToPassThroughX.size()==2) {
        double slope = (pointsToPassThroughY[1]-pointsToPassThroughY[0])/(pointsToPassThroughX[1]-pointsToPassThroughX[0]);
        double normal_x = slope/sqrt(1+slope*slope);
        double normal_y = -1/sqrt(1+slope*slope);
        for (double j = -thickness+1;j<thickness;j+=0.05) {
            Uint32 colorToUse = color;
            if (j+0.05>=thickness) {
                colorToUse=edgeColor;
                drawLineOnSurface(toDraw, pointsToPassThroughX[0]+normal_x*(-thickness+1), pointsToPassThroughY[0]+normal_y*(-thickness+1), pointsToPassThroughX[1]+normal_x*(-thickness+1), pointsToPassThroughY[1]+normal_y*(-thickness+1), colorToUse);
                //^^^ Draw the first line over again so it doesn't get covered up!
            }
            drawLineOnSurface(toDraw, pointsToPassThroughX[0]+normal_x*j, pointsToPassThroughY[0]+normal_y*j, pointsToPassThroughX[1]+normal_x*j, pointsToPassThroughY[1]+normal_y*j, colorToUse);
        }
        return;//only draw 1 line if only 2 points
    }
    double start_x = pointsToPassThroughX[0];
    double start_y = pointsToPassThroughY[0];
    for (int i = 0;i<pointsToPassThroughX.size()-2;i++) {
        //First, assign the stuff we're dealing with to some local variables
        double A_x = pointsToPassThroughX[i];
        double A_y = pointsToPassThroughY[i];
        double B_x = pointsToPassThroughX[i+1];
        double B_y = pointsToPassThroughY[i+1];
        double C_x = pointsToPassThroughX[i+2];
        double C_y = pointsToPassThroughY[i+2];
        //Express A->B and B->C as vectors
        double vec_A_B_x = B_x-A_x;
        double vec_A_B_y = B_y-A_y;
        double vec_B_C_x = C_x-B_x;
        double vec_B_C_y = C_y-B_y;
        //Find slopes of these vectors
        double slope_A_B = vec_A_B_y/vec_A_B_x;
        double slope_B_C = vec_B_C_y/vec_B_C_x;
        //Now, find smallest line, A->B or B->C
        double length_A_B = sqrt(pow(vec_A_B_x,2)+pow(vec_A_B_y,2));
        double length_B_C = sqrt(pow(vec_B_C_x,2)+pow(vec_B_C_y,2));
        double smallestLength = 0;
        if (length_A_B<length_B_C) {
            smallestLength = length_A_B;
        }
        else {
            smallestLength = length_B_C;
        }
        //Calculate points of intersection of our rounding circle and our lines
        double A_end_x = B_x-(0.5*smallestLength/length_A_B)*vec_A_B_x;
        double A_end_y = B_y-(0.5*smallestLength/length_A_B)*vec_A_B_y;
        double B_start_x = B_x+(0.5*smallestLength/length_B_C)*vec_B_C_x;
        double B_start_y = B_y+(0.5*smallestLength/length_B_C)*vec_B_C_y;
        //Draw the line A->B: (but cut off for the circle & make it thick)
        //Normal line proportional to [slope,-1] b/c normal is -1/slope
        double slope = (A_end_y-start_y)/(A_end_x-start_x);
        //normalize it as well
        double normal_x = slope/sqrt(1+slope*slope);
        double normal_y = -1/sqrt(1+slope*slope);
        for (double j = -thickness+1;j<thickness;j+=0.05) {
            //drawLineOnSurface(toDraw, start_x, start_y, A_end_x, A_end_y, color);
            Uint32 colorToUse = color;
            if (j+0.05>=thickness) {
                colorToUse=edgeColor;
                drawLineOnSurface(toDraw, start_x+normal_x*(-thickness+1), start_y+normal_y*(-thickness+1), A_end_x+normal_x*(-thickness+1), A_end_y+normal_y*(-thickness+1), colorToUse);
            }
            drawLineOnSurface(toDraw, start_x+normal_x*j, start_y+normal_y*j, A_end_x+normal_x*j, A_end_y+normal_y*j, colorToUse);
        }
        //Don't draw B->C as that will be handled in the next iteration! (b/c it could be cut off again by another circle)
        //So save its variables
        start_x = B_start_x;
        start_y = B_start_y;
        //Find the circle's center points
        double center_y = (A_end_x+slope_A_B*A_end_y-B_start_x-slope_B_C*B_start_y)/(slope_A_B-slope_B_C);
        double center_x = B_start_x+slope_B_C*B_start_y-slope_B_C*center_y;
        //Find its radius
        double radius = sqrt((B_start_x-center_x)*(B_start_x-center_x)+(B_start_y-center_y)*(B_start_y-center_y));
        //Find its starting and ending radians
        //Angles can be computed between 2 vectors as atan2(determinantV1&V2,dotV1&V2)
        //Compute between RS and (1,0) for start_rad
        //add angle between RS and QS for end_rad
        //dot is x1x2+y1y2
        //determinant is x1y2-x2y1
        double RS_x = center_x-B_start_x;
        double RS_y = center_y-B_start_y;
        double QS_x = center_x-A_end_x;
        double QS_y = center_y-A_end_y;
        //angle between [1,0] and [RS_x,RS_y]:
        double start_rad = atan2((-QS_y), -(QS_x));
        //angle between [1,0] and [QS_x,QS_y]
        double end_rad = atan2((-RS_y), -(RS_x));
        double del_rad = end_rad-start_rad;
        if (del_rad>M_PI||del_rad<-M_PI) {//Absolute value greater than half the circle, too big, use smaller one
            if (del_rad>M_PI) {
                del_rad-=2*M_PI;
            }
            else {
                del_rad+=2*M_PI;
            }
        }
        //for debug vvv
        //drawLineOnSurface(toDraw, center_x, center_y, A_end_x, A_end_y, 0xff0000ff);
        //drawLineOnSurface(toDraw, center_x, center_y, B_start_x, B_start_y, 0xff00ff00);
        //for debug ^^^
        //Draw the circle:
        //but draw multiple of them to accomodate for thickness
        for (double k = -thickness+1;k<thickness;k+=0.05) {
            int circleResolution = 720;//How many points to draw on the circle
            double effectiveRadius = radius+k;
            Uint32 colorToUse = color;
            if (k+0.05>=thickness) {
                colorToUse=edgeColor;
                for (double j = 0;j<circleResolution;j++) {//Redraw inner circle as it will have been drawn over
                    //(∆rad/circleResolution)*j+start_rad
                    double radianAt = ((del_rad)/circleResolution)*j+start_rad;
                    double circlePos_x = center_x+(radius-thickness+1)*cos(radianAt);
                    double circlePos_y = center_y+(radius-thickness+1)*sin(radianAt);
                    put_pixel32(toDraw, circlePos_x, circlePos_y, colorToUse);
                }
            }
            for (double j = 0;j<circleResolution;j++) {
                //(∆rad/circleResolution)*j+start_rad
                double radianAt = ((del_rad)/circleResolution)*j+start_rad;
                double circlePos_x = center_x+(effectiveRadius)*cos(radianAt);
                double circlePos_y = center_y+(effectiveRadius)*sin(radianAt);
                if (circlePos_x>=0&&circleResolution>=0&&circlePos_x<SCREEN_WIDTH&&circlePos_y<SCREEN_HEIGHT) {
                    put_pixel32(toDraw, circlePos_x, circlePos_y, colorToUse);
                }
            }
        }
    }
    //draw final line
    //Normal line proportional to [slope,-1] b/c normal is -1/slope
    double slope = (pointsToPassThroughY[pointsToPassThroughY.size()-1]-start_y)/(pointsToPassThroughX[pointsToPassThroughX.size()-1]-start_x);
    //normalize it as well
    double normal_x = slope/sqrt(1+slope*slope);
    double normal_y = -1/sqrt(1+slope*slope);
    for (double j = -thickness+1;j<thickness;j+=0.05) {
        Uint32 colorToUse = color;
        if (j+0.05>=thickness) {
            colorToUse=edgeColor;
            drawLineOnSurface(toDraw, start_x+normal_x*(-thickness+1), start_y+normal_y*(-thickness+1), pointsToPassThroughX[pointsToPassThroughY.size()-1]+normal_x*(-thickness+1), pointsToPassThroughY[pointsToPassThroughY.size()-1]+normal_y*(-thickness+1), colorToUse);
        }
        drawLineOnSurface(toDraw, start_x+normal_x*j, start_y+normal_y*j, pointsToPassThroughX[pointsToPassThroughY.size()-1]+normal_x*j, pointsToPassThroughY[pointsToPassThroughY.size()-1]+normal_y*j, colorToUse);
    }
}

void drawCircle(int cx,int cy,int r,Uint32 color) {
    SDL_Surface* circleSurface = createBlankSurfaceWithSize(2*r, 2*r);
    drawCircleOnSurface(circleSurface, r, r, r, color);
    SDL_Texture* toDraw = SDL_CreateTextureFromSurface(gRenderer, circleSurface);
    drawGraphic(cx-r, cy-r, 2*r, 2*r, toDraw);
    SDL_FreeSurface(circleSurface);
    SDL_DestroyTexture(toDraw);
}

void drawHollowCircle(int cx,int cy,int r,Uint32 color) {
    SDL_Surface* circleSurface = createBlankSurfaceWithSize(2*r, 2*r);
    drawHollowCircleOnSurface(circleSurface, r, r, r, color);
    SDL_Texture* toDraw = SDL_CreateTextureFromSurface(gRenderer, circleSurface);
    drawGraphic(cx-r, cy-r, 2*r, 2*r, toDraw);
    SDL_FreeSurface(circleSurface);
    SDL_DestroyTexture(toDraw);
}

void drawBorderedCircle(int cx,int cy,int r,Uint32 color,Uint32 bgColor) {
    drawCircle(cx, cy, r, color);
    drawHollowCircle(cx,cy,r,bgColor);
}




//Logic definitions

void fastSineCosine_float(float* sine,float* cosine,float angle) {
    //It's faster if I need a sine and cosine of 1 angle to use this
    //function than to calculate them individually
    #ifdef DEBUGMODE
        if (angle<0||angle>2*M_PI) {
            throw std::runtime_error("ERROR don't use big angles!");
        }
    #endif
    *sine = sinf(angle);
    *cosine = sqrtf(1-((*sine)*(*sine)))*((angle>M_PI/2&&angle<3*M_PI/2)?-1:1);
}

float slopeOfLine(float startx,float starty,float endx,float endy) {
    return (endy-starty)/(endx-startx);
}

/*float fabs(float in) noexcept {
    if (in<0) {
        return -in;
    }
    return in;
}*/

float randomOnRange(float s,float e) {
    return s+((e-s)*(((double) rand() / (RAND_MAX))));
}

float meanTimeToHappenToPercent(float finalPercent,float time) {
    //if finalPercent==.5, same as MTTH
    return 1-pow(finalPercent,1/time);
}

int hexCharToInt(char in) {
    switch (in) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
            return 10;
        case 'B':
            return 11;
        case 'C':
            return 12;
        case 'D':
            return 13;
        case 'E':
            return 14;
        case 'F':
            return 15;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
    }
    return -1;
}

std::string assembleString(std::vector<std::string> a,std::string between) {
    std::string toReturn = "";
    for (int i = 0;i<a.size();i++) {
        between = (i==a.size()-1)?"":between;
        toReturn+=a[i]+between;
    }
    return toReturn;
}

std::vector<std::string> split(const std::string &text, char sep) {//From "Alec Thomas" @ stack exchange
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}
Uint8 getA(Uint32 color) {
    return (color&0xFF000000)>>24;
}

Uint8 getR(Uint32 color) {
    return (color&0x00FF0000)>>16;
}

Uint8 getG(Uint32 color) {
    return (color&0x0000FF00)>>8;
}
Uint8 getB(Uint32 color) {
    return (color&0x000000FF);
}

Uint32 invertColor(Uint32 c) {
    return (getA(c)*0x01000000+(0xFF-getR(c))*0x00010000+(0xFF-getG(c))*0x00000100+(0xFF-getB(c))*0x0000000001);
}

std::vector<std::string> splitAtColon(std::string input) {//Note: this function actually returns things in the wrong order!  In future, use
                                                        //the better, more generic, 'split' function
    std::string firstStringToReturn = "";
    std::string secondStringToReturn = "";
    bool foundColon = false;
    for(int n = 0;n<input.size();n++) {
        if (input[n]==':') {
            foundColon = true;
        }
        else {
            if (foundColon) {
                firstStringToReturn+=input[n];
            }
            else {
                secondStringToReturn+=input[n];
            }
        }
    }
    return {firstStringToReturn,secondStringToReturn};
}
std::vector<std::string> splitAtIgnoringBetween(std::string input,char splitter,char starter,char ender) {
    int layer = 0;
    std::vector<std::string> toReturn = {""};
    int index = 0;
    for(int n = 0;n<input.size();n++) {
        if (input[n]==splitter&&layer==0) {
            toReturn.push_back("");
            index++;
        }
        else {
            if (input[n] == starter) {
                layer++;
            }
            else if (input[n] == ender) {
                layer--;
            }
            toReturn[index]+=input[n];
        }
    }
    return toReturn;
}
std::vector<std::string> splitAt(std::string input,char splitter) {
    //std::string firstStringToReturn = "";
    //std::string secondStringToReturn = "";
    std::vector<std::string> toReturn = {""};
    int index = 0;
    for(int n = 0;n<input.size();n++) {
        if (input[n]==splitter) {
            toReturn.push_back("");
            index++;
        }
        else {
            toReturn[index]+=input[n];
        }
    }
    return toReturn;
}

std::string replaceInString(std::string theString,char toReplace,char with) {
    for (int i = 0;i<theString.size();i++) {
        if (theString[i]==toReplace) {
            theString[i]=with;
        }
    }
    return theString;
}

/*bool stringContains(std::string theString,char toContain) {
    for (int i = 0;i<theString.size();i++) {
        if (theString[i]==toContain) {return true;}
    }
    return false;
}*/




float floatFromString(std::string theString) {
    float finalNumber = 0;
    bool isAfterDecimalPoint = false;
    int decimalPointDivider = 1;
    bool isNegative = false;
    //std::cout<<"In: "<<input<<"\n";
    for (int hexCounter=0;hexCounter<theString.size();hexCounter++) {
        if (theString[hexCounter]=='.') {//decimal point
            isAfterDecimalPoint = true;
        }
        else if (theString[hexCounter]=='-') {
            isNegative = true;
        }
        else {
            int numToAdd = hexCharToInt(theString[hexCounter]);
            //std::cout<<numToAdd<<"\n";
            if (isAfterDecimalPoint) {
                finalNumber+=numToAdd*pow(10, (int)(theString.size())-hexCounter-1);
                decimalPointDivider*=10;
            }
            else {
                //std::cout<<"S:"<<input.size()<<" H:"<<hexCounter<<" C:"<<(int)(input.size())-hexCounter-2<<" P:"<<pow(10, input.size()-hexCounter-2)<<"\n";
                finalNumber+=numToAdd*pow(10, (int)(theString.size())-hexCounter-2);
            }
        }
    }
    if (isAfterDecimalPoint) {
        finalNumber/=decimalPointDivider;
    }
    else {
        finalNumber*=10;
    }
    if (isNegative) {
        finalNumber*=-1;
    }
    return finalNumber;
}

template<typename numtype> std::string numberToString(numtype toBeString) {
    std::string toReturn  = std::to_string(toBeString);
    bool foundDecimal = false;
    for (int i = (int)toReturn.size()-1;i>0;i--) {
        if (toReturn[i]=='.') {
            foundDecimal = true;
            break;
        }
    }
    if (!foundDecimal) {//Otherwise, 1000->1
        return toReturn;
    }
    for (int  i = (int)toReturn.size()-1;i>0;i--) {
        if (toReturn[i]=='0') {
            toReturn.pop_back();
        }
        else if (toReturn[i]=='.') {
            toReturn.pop_back();
            break;
        }
        else {
            break;
        }
    }
    return toReturn;
}

std::string floatToString(float in) {//trim unnecessary zeroes
    std::string toReturn  = std::to_string(in);
    bool foundDecimal = false;
    for (int i = (int)toReturn.size()-1;i>0;i--) {
        if (toReturn[i]=='.') {
            foundDecimal = true;
            break;
        }
    }
    if (!foundDecimal) {//Otherwise, 1000->1
        return toReturn;
    }
    for (int  i = (int)toReturn.size()-1;i>0;i--) {
        if (toReturn[i]=='0') {
            toReturn.pop_back();
        }
        else if (toReturn[i]=='.') {
            toReturn.pop_back();
            break;
        }
        else {
            break;
        }
    }
    return toReturn;
}
bool intVectorContains(std::vector<int> v,int toContain) {
    if (v.empty()) {
        return false;
    }
    for (int i = 0;i<v.size();i++) {
        if (toContain==v[i]) {
            return true;
        }
    }
    return false;
}

bool stringVectorContains(std::vector<std::string> v,std::string toContain) {
    if (v.empty()) {
        return false;
    }
    for (int i = 0;i<v.size();i++) {
        if (toContain==v[i]) {
            return true;
        }
    }
    return false;
}

bool shiftPressed() {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[SDL_SCANCODE_LSHIFT]||state[SDL_SCANCODE_RSHIFT];
}

bool stringIsWhitespace(std::string s) {
    return std::all_of(s.begin(),s.end(),isspace);//"Michael Goldshteyn" stack overflow
}

std::string trim(const std::string &s) { //"Pushkoff" stack overflow
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

bool charIsNumeral(char input) {
    switch (input) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            return true;
    }
    return false;
}

bool stringContains(std::string s,char c) {
    if (s.empty()) {
        return false;
    }
    for (int i = 0;i<s.size();i++) {
        if (s[i]==c) {
            return true;
        }
    }
    return false;
}
Uint32 hexadecimalFromString(std::string input) {
    Uint32 finalNumber = 0x00000000;
    bool addToNumber = false;
    for (int hexCounter=0;hexCounter<input.size();hexCounter++) {
        if (addToNumber) {
            int numToAdd = hexCharToInt(input[hexCounter]);
            finalNumber+=numToAdd*pow(16, input.size()-hexCounter-1);
        }
        if (input[hexCounter]=='x') {
            addToNumber=true;
        }
    }
    //std::cout<<"Created Province with color "<<input<<" ("<<finalNumber<<")\n";
    return finalNumber;
}



//Parsing Files
std::string ParsedFile::getValue() {
    return value;
}
std::string ParsedFile::getKey() {
    return key;
}
ParsedFile::ParsedFile() {
    version = PARSED_FILE_VERSION_NUMBER;   //Set the version number up correctly
                                            //even though this is a dummy constructor
                                            //so that no errors get thrown
}
ParsedFile* ParsedFile::parseFile(std::fstream* fileToParse) {
    //Splits fileToParse into array of strings
    //to pass on to constructor that takes an
    //array of strings
    //First two lines MUST be version:## and tag:%%%
    //If not, throw an error
    
    //Declare variables
    float versionToSet = 0.0;
    std::string tagToSet = "";
    
    //Split the file
    std::string tempLine;
    int fl = 0;
    while ( getline (*fileToParse,tempLine,'\n') ) {
        fl++;
        //theFile+=tempLine;
    }
    const int fileLength = fl;
    std::vector<std::string> theFile(fileLength);
    fl = 0;
    fileToParse->clear();
    fileToParse->seekg(0, std::ios::beg);
    while ( getline (*fileToParse,tempLine,'\n') ) {
        //tempLine.erase(std::remove(tempLine.begin(), tempLine.end(), ' '), tempLine.end());
        //tempLine.erase(std::remove(tempLine.begin(), tempLine.end(), '\n'), tempLine.end());//removes returns
        theFile[fl]=tempLine;
        fl++;
    }
    bool versionFound = false;
    bool tagFound = false;
    for (int i = 0;i<theFile.size()&&!(versionFound&&tagFound);i++) {
        if (stringIsWhitespace(theFile[i])) {
            continue;
        }
        std::vector<std::string> versionAndTagChecker = splitAt(theFile[i], ':');
        if (trim(versionAndTagChecker[0])=="version") {
            versionToSet = floatFromString(trim(versionAndTagChecker[1]));
            versionFound = true;
        }
        else if (trim(versionAndTagChecker[0])=="tag") {
            tagToSet = trim(versionAndTagChecker[1]);
            tagFound = true;
        }
        else {
            throw std::runtime_error("Error! File missing version or tag!  They must be placed at the top of the file!");
        }
    }
    if (!tagFound) {
        throw std::runtime_error("Error! Tag is missing from file! It must be placed at the top of the file!");
    }
    if (!versionFound) {
        throw std::runtime_error("Error! Version is missing from file! It must be placed at the top of the file!");
    }
    if (versionToSet!=PARSED_FILE_VERSION_NUMBER) {
        throw std::runtime_error("Error! File is not of correct version!  It is of version " + std::to_string(versionToSet) + " but should be of version " + std::to_string(PARSED_FILE_VERSION_NUMBER));
    }
    
    ParsedFile* toReturn = new ParsedFile(theFile);
    toReturn->version = versionToSet;
    toReturn->tag = tagToSet;
    toReturn->key = "Parent";
    toReturn->value = "Bailey is Awesome!";
    return toReturn;
}
ParsedFile::ParsedFile(std::vector<std::string> relevantInput) {
    /*
        So here's how this parser works:
        Breaks file up into array of strings, line by line (already done)
        Then, it loops through line by line:
            First it looks for a //
                If so, everything to the right of it, and the // itself, are removed
            Secondly it looks for a :
                If none is found
                    Look for a }
                        If none is found
                            If not just whitespace
                                Create component with key noKeyCount and value of the line
                                Increment noKeyCount
     
                        If an } is found
                            Decrement curlyBraceCount
                If : is found
                    Travel leftwards until it finds first non-whitspace character
                    If none is found,
                        throw an error
                    Else,
                        Keep on travelling until it finds new whitespace or reaches start of string
                        The string bounded by these two ends is the key of the component
                    Return to :
                    Travel rightwards until if finds first non-whitespace character
                    If none is found,
                        throw an error
                    Else,
                        If { is found,
                            Scan line by line until it has found the matching }
                            The set of lines inbetween the curly braces should be recursively fed into this function
                            And will equal be added to this component's list of child components
                        Else,
                            Keep on travelling until it finds new whitespace or reaches end of string
                            The string bounded by these two ends is the value of the component
            It has now either found a key and a value or a key and a list of components
     
    */
    int noKeyCount = 0;         //Count of items found without a key (traits:{respectable} is an example of this)
                                //Used for naming them
    bool isInMultilineComment = false;
    for (int i = 0;i<relevantInput.size();i++) {
        //Loop through the lines of the input
        std::string theLine = relevantInput[i];
        int colonLocation = -1;
        int keyStart = -1;
        int keyEnd = -1;
        int valueStart = -1;
        int valueEnd = (int)theLine.size();
        bool curlyBraceFound = false;
        bool foundValue = false;
        bool isInString = false;        //if between ""s, don't break keys/values at spaces
        int hardBracketLevel = 0;       //behaves like isInString but [s can be nested
                                        //[ is used for embed text and triggers
        for (int j = 0;j<theLine.size();j++) {
            //Find all relevant values in one readthrough
            if (isInMultilineComment) {
                if (theLine[j]=='*') {
                    if (theLine.size()-1>j) {
                        if (theLine[j+1]=='/') {
                            isInMultilineComment = false;   //Has exited out of a multiline comment!
                            j+=1;                           //act as if pointing at the slash currently
                                                            //so next iteration it will be pointing at after the slash
                            continue;
                        }
                    }
                    else {
                        continue;
                    }
                }
                else {
                    continue;
                }
            }
            bool charIsWhite = (theLine[j]==' '||theLine[j]=='\t'||theLine[j]=='\n')    //character is whitespace if it is whitespace
                                    &&!isInString                                       //but not counted as such if between "s
                                    &&(hardBracketLevel==0);                            //and can't be between []s
            if (theLine[j]=='/') {
                if (theLine.size()-1>j) {
                    if (theLine[j+1]=='/') {
                        if (keyEnd==-1) {
                            keyEnd=j;
                        }
                        else if (!foundValue) {
                            valueEnd=j;
                            foundValue=true;
                        }
                        break;//comment, ignore rest of line
                    }
                    if (theLine[j+1]=='*') {
                        isInMultilineComment = true;    //multiline comment, ignore until we find an end tag
                        if (keyStart!=-1&&keyEnd==-1) { //comment is inline, end key if currently found it
                            keyEnd = j;
                        }
                        if (valueStart!=-1) {           //comment is inline, end value if currently found it
                            valueEnd = j;
                            foundValue = true;
                                                        //don't break; because what if the comment end is
                                                        //after the value end
                        }
                        j++;//skip so that it is looking at the thing after the * on the next loop
                        continue;
                    }
                }
            }
            if (foundValue) {                           //If value has already been found, the only thing we care
                                                        //about is checking for multi-line comments like those
                                                        //checked for above
                continue;
            }
            if (isInString) {
                if (theLine[j]=='"') {
                    isInString=false;//end the string!
                    if (hardBracketLevel==0) {//only end the string if not in hardbrackets
                        if (keyEnd==-1) {
                            keyEnd = j;
                        }
                        if (valueStart!=-1) {
                            valueEnd = j;
                            foundValue = true;
                        }
                    }
                }
                continue;
            }
            if (theLine[j]=='[') {
                hardBracketLevel++;
                if (hardBracketLevel==1) {//If [ is the first thingy, check if should start key or value
                    //if (keyStart==-1) {
                    //    keyStart = j;//start during the [
                    //}
                    //else if (valueStart==-1) {
                    //    valueStart = j;
                    //}
                    if (keyEnd!=-1&&valueStart==-1) {
                        valueStart=j;
                    }
                    else if (keyStart==-1) {
                        keyStart = j;
                    }
                }
            }
            else if (theLine[j]==']') {
                hardBracketLevel--;
                if (hardBracketLevel==0) {//end it if found end of []s
                    if (keyEnd==-1) {
                        keyEnd = j+1;//end after ]
                    }
                    if (valueStart!=-1) {
                        valueEnd = j+1;//end after ]
                        foundValue = true;
                    }
                }
                continue;//skip otherwise it tries to start new value with ]
                
            }
            if (hardBracketLevel>0) {
                continue;
            }
            if (theLine[j]==':') {
                //Colon found!
                colonLocation = j;
                keyEnd = j;
            }
            else if (theLine[j]=='"') {
                isInString=true;
                if (keyStart==-1) {
                    keyStart = j+1;//start after the "
                }
                else if (valueStart==-1) {
                    valueStart = j+1;
                }
            }
            else if (theLine[j]=='{') {
                curlyBraceFound = true;
                break;
            }
            else if (theLine[j]=='}') {
                throw std::runtime_error("Error: Weidly placed }");
                break;
            }
            else if (!charIsWhite&&keyStart==-1) {
                keyStart = j;
            }
            else if (charIsWhite&&keyStart!=-1&&keyEnd==-1) {
                keyEnd = j;
            }
            else if (!charIsWhite&&keyEnd!=-1&&valueStart==-1) {
                valueStart = j;
            }
            else if (charIsWhite&&valueStart!=-1) {
                valueEnd = j;
                foundValue = true;                      //don't break; as there may be a start to a multi-line comment after this
            }
        }
        //Here's how to read the data we just got:
        //string between keyStart and keyEnd is the key
        //if curlyBraceFound is false, string between
        //valueStart and valueEnd is the value
        //otherwise, we must find the paired closing brace
        //and all lines inbetween them will become a component on their own
        if (colonLocation==-1) {
            if (keyStart==-1) {//is whitespace, ignore line
                continue;
            }
            //In this case, if not whitespace, it's a special type of component with the key autogenerated
            ParsedFile* componentToAdd = new ParsedFile();
            componentToAdd->key = std::to_string(noKeyCount);
            noKeyCount++;
            componentToAdd->value = theLine.substr(keyStart,keyEnd-keyStart);       //Uses keyStart/End instead of valueStart/End
                                                                                    //Because even though we've read in a value,
                                                                                    //because of the way reading happens it's
                                                                                    //registered as a key
            components.push_back(componentToAdd);
        }
        else {
            if (curlyBraceFound) {
                std::vector<std::string> lines = {};
                int curlyBraceCounter = 1;
                while (curlyBraceCounter>0) {
                    i++;
                    if (i>=relevantInput.size()) {
                        throw std::runtime_error("Error: Mismatched curly braces");
                    }
                    if (stringContains(relevantInput[i],'{')) {
                        curlyBraceCounter++;
                    }
                    if (stringContains(relevantInput[i], '}')) {
                        curlyBraceCounter--;
                    }
                    if (curlyBraceCounter>0) {
                        if (lines.empty()) {
                            lines = {relevantInput[i]};
                        }
                        else {
                            lines.push_back(relevantInput[i]);
                        }
                    }
                }
                ParsedFile* componentToAdd = new ParsedFile(lines);
                componentToAdd->key = theLine.substr(keyStart,keyEnd-keyStart);
                components.push_back(componentToAdd);
            }
            else {
                //This line contains colon, and no { meaning it's a normal line
                ParsedFile* componentToAdd = new ParsedFile();
                componentToAdd->key = theLine.substr(keyStart,keyEnd-keyStart);
                componentToAdd->value = theLine.substr(valueStart,valueEnd-valueStart);
                components.push_back(componentToAdd);
            }
        }
    }
}
std::vector<ParsedFile*> ParsedFile::internalComponentFromString(std::vector<std::string> componentID) {
    std::string componentToSearchFor = componentID[componentID.size()-1];
    if (componentToSearchFor=="*") {
        //The * command is special, it gives all of the components of the parent
        //Can't just return components; because due to reference shenanigans it would obviously not be safe
        std::vector<ParsedFile*> toReturn = {};
        if (components.empty()) {
            return toReturn;
        }
        for (int i = 0;i<components.size();i++) {
            toReturn.push_back(components[i]);
        }
        return toReturn;
    }
    else {
        //Search for correct components
        std::vector<ParsedFile*> toReturn = {};
        for (int i = 0;i<components.size()&&!components.empty();i++) {
            if (components[i]->getKey()==componentToSearchFor) {
                toReturn.push_back(components[i]);
            }
        }
        componentID.pop_back();
        if (componentID.size()==0||componentID.empty()) {
            //job is done, don't need to do recursion shenanigans
            return toReturn;
        }
        //job is not done, all the possible search criteria need to be further narrowed down
        std::vector<ParsedFile*> trueToReturn = {};
        for (int i = 0;i<toReturn.size();i++) {
            //trueToReturn.push_back(toReturn[i]->internalComponentFromString(componentID));
            //std::vector<ParsedFile*> old = trueToReturn;
            std::vector<ParsedFile*> newData = toReturn[i]->internalComponentFromString(componentID);
            trueToReturn.insert(trueToReturn.end(), newData.begin(), newData.end());
        }
        return trueToReturn;
    }
    //throw std::runtime_error("Error! ParsedFile has no such componentID: "+componentToSearchFor);
    //return {new ParsedFile({"ERROR"})};//temporary
}
std::vector<ParsedFile*> ParsedFile::componentFromString(std::string componentID) {
    std::vector<std::string> dotBreaks = splitAt(componentID, '.');
    std::reverse(dotBreaks.begin(), dotBreaks.end());//Reverse so popping is easier
    return internalComponentFromString(dotBreaks);
}
std::string ParsedFile::valueOf(std::string componentID,int which) {
    std::vector<ParsedFile*> toReturnArray = componentFromString(componentID);
    if (which>=toReturnArray.size()) {
        throw std::runtime_error("ERROR: Component searching for doesn't exist");
    }
    return toReturnArray[which]->getValue();
}
bool ParsedFile::componentExists(std::string componentID) {
    std::vector<ParsedFile*> searchQueryResults = componentFromString(componentID);
    return !searchQueryResults.empty();
}
std::vector<std::string> ParsedFile::valuesOf(std::string componentID) {
    std::vector<ParsedFile*> temp = componentFromString(componentID);
    std::vector<std::string> toReturn = {};
    for (int i = 0;i<temp.size();i++) {
        toReturn.push_back(temp[i]->getValue());
    }
    return toReturn;
}
std::string ParsedFile::getTag() {
    return tag;
}

//Localized Text stuff
template<typename T> Point<T>::Point(T inx,T iny) {
    x = inx;
    y = iny;
}
std::string localizationLanguage="en";
LocalizedText::LocalizedText(std::string k,EmbedText* text,int size_in,Uint32 color_in,float angle_in) {
    angle = angle_in;
    size = size_in;
    color = color_in;
    key = k;
    angle = angle*M_PI/180;//convert to radians
    actualText = text;
    fastSineCosine_float(&sin_theta, &cos_theta, angle);
    wordByWord = {};
    for (int i = 0;i<text->stringsToAppend.size();i++) {
        std::vector<std::string> temp = splitAt(text->stringsToAppend[i],' ');
        bool works = false;
        int startingSize = (int)wordByWord.size();
        for (int j = 0;j<temp.size();j++) {
            //if (temp[j]!="") {
            works = true;
            if (temp[j]=="") {
                if (j==0) {
                    wordByWord.push_back(" ");
                }
                else {
                    wordByWord.push_back("");
                }
            }
            else {
                if (j==temp.size()-1) {
                    wordByWord.push_back(temp[j]);
                }
                else {
                    wordByWord.push_back(temp[j]+" ");
                }
            }
            //}
        }
        if (works) {
            embedStarts.push_back(startingSize);
            //if (startingSize>text->stringsToAppend.size()) {
                //std::cout<<"Ahh";
            //}
        }
    }
    
    for (int i = 0;i<wordByWord.size();i++) {
        textureOffsets.push_back(Point<int>(0,0));
        generatedTexture.push_back(getTextAsTexture(wordByWord[i], size, color,angle,&(textureOffsets[i].x),&(textureOffsets[i].y)));
    }
    //for (int i = 0;i<text->stringsToAppend.size();i++) {
    //    textureOffsets.push_back(Point<int>(0,0));
    //    generatedTexture.push_back(getTextAsTexture(text->stringsToAppend[i], size, color,angle,&(textureOffsets[i].x),&(textureOffsets[i].y)));
    //}
}
LocalizedText::LocalizedText(ParsedFile* p) {
    key = p->getKey();
    std::vector<ParsedFile*> toBeComponents = p->componentFromString("*");
    bool hasLeafComponents = false; //if contains components that have actual text (not just for grouping)
                                    //needed b/c if it doesn't find the localization language, default to english
    std::vector<std::string> missedLocalizations = {};
    std::vector<std::string> foundLocalizations = {};
    for (int i = 0;i<toBeComponents.size();i++) {
        ParsedFile* curComponent = toBeComponents[i];
        std::vector<std::string> splitstr = splitAt(curComponent->getKey(),'|');
        if (splitstr[0]==localizationLanguage&&splitstr.size()>1) {
            //is actual definer of text!
            components.push_back(
                new LocalizedText(
                    splitstr[1],            new EmbedText(curComponent->valueOf("text")),
                                            floatFromString(curComponent->valueOf("size")),
                                            hexadecimalFromString(curComponent->valueOf("color")),
                                            floatFromString(curComponent->valueOf("angle"))
                )
            );
            hasLeafComponents = true;
            foundLocalizations.push_back(splitstr[1]);
            if (!missedLocalizations.empty()) {
                //remove from missed localizations if its on there (stackoverflow solution, Etherealone)
                //swaps element to be removed with last element and then pops it out
                auto loc = std::find(missedLocalizations.begin(),missedLocalizations.end(),splitstr[1]);
                if (loc!=missedLocalizations.end()) {
                    std::swap(*loc,missedLocalizations.back());
                    missedLocalizations.pop_back();
                }
            }
        }
        else if (splitstr.size()>1) {
            //would be definer of text but not correct language
            hasLeafComponents = true;
            auto loc = std::find(foundLocalizations.begin(), foundLocalizations.end(), splitstr[1]);
            if (loc==foundLocalizations.end()||foundLocalizations.empty()) {//if not already found
                missedLocalizations.push_back(splitstr[1]);
            }
        }
        else {
            //we need to go deeper!
            components.push_back(new LocalizedText(curComponent));
        }
    }
    if (hasLeafComponents&&!missedLocalizations.empty()) {//if didn't find correct language text, default to english
        for (int i = 0;i<toBeComponents.size();i++) {
            ParsedFile* curComponent = toBeComponents[i];
            std::vector<std::string> splitstr = splitAt(curComponent->getKey(),'|');
            if (splitstr[0]=="en"&&splitstr.size()>1) {
                //potential candidate for using defaults
                bool found = false;
                for (int j = 0;j<missedLocalizations.size();j++) {//check if needs defaults
                    if (missedLocalizations[j]==splitstr[1]) {
                        found = true;
                        break;
                    }
                }
                if (!found) {//doesn't need defaults
                    continue;
                }
                //needs defaults
                components.push_back(
                    new LocalizedText(
                        splitstr[1],            new EmbedText(curComponent->valueOf("text")),
                                                floatFromString(curComponent->valueOf("size")),
                                                hexadecimalFromString(curComponent->valueOf("color")),
                                                floatFromString(curComponent->valueOf("angle"))
                    )
                );
            }
        }
    }
}
std::vector<LocalizedText*> LocalizedText::internalComponentFromString(std::vector<std::string> componentID) {
    std::string componentToSearchFor = componentID[componentID.size()-1];
    if (componentToSearchFor=="*") {
        //The * command is special, it gives all of the components of the parent
        //Can't just return components; because due to reference shenanigans it would obviously not be safe
        //or maybe it would, who knows as it's not a pointer to a vector but a normal vector
        std::vector<LocalizedText*> toReturn = {};
        if (components.empty()) {
            return toReturn;
        }
        for (int i = 0;i<components.size();i++) {
            toReturn.push_back(components[i]);
        }
        return toReturn;
    }
    else {
        //Search for correct components
        std::vector<LocalizedText*> toReturn = {};
        for (int i = 0;i<components.size()&&!components.empty();i++) {
            if (components[i]->getKey()==componentToSearchFor) {
                toReturn.push_back(components[i]);
            }
        }
        componentID.pop_back();
        if (componentID.size()==0||componentID.empty()) {
            //job is done, don't need to do recursion shenanigans
            return toReturn;
        }
        //job is not done, all the possible search criteria need to be further narrowed down
        std::vector<LocalizedText*> trueToReturn = {};
        for (int i = 0;i<toReturn.size();i++) {
            //trueToReturn.push_back(toReturn[i]->internalComponentFromString(componentID));
            //std::vector<ParsedFile*> old = trueToReturn;
            std::vector<LocalizedText*> newData = toReturn[i]->internalComponentFromString(componentID);
            trueToReturn.insert(trueToReturn.end(), newData.begin(), newData.end());
        }
        return trueToReturn;
    }
}
bool LocalizedText::componentExists(std::string componentID) {
    std::vector<LocalizedText*> searchQueryResults = componentFromString(componentID);
    return !searchQueryResults.empty();
}
std::vector<LocalizedText*> LocalizedText::componentFromString(std::string componentID) {
    std::vector<std::string> dotBreaks = splitAt(componentID, '.');
    std::reverse(dotBreaks.begin(), dotBreaks.end());//Reverse so popping is easier
    return internalComponentFromString(dotBreaks);
}
void LocalizedText::internalDraw(int x,int y,int width,int height,Uint8 attributes,EmbedTextData* data,Uint32 bgcolor,Uint32 bgborder,LocalizedText* parent) {
    /*
    Has to handle following attributes:
        NO_FLAG: does nothing
        CENTER_X: centers text x-wise
        CENTER_Y: centers text y-wise
        WRAP: wraps text if goes beyond x
        CUTOFF: cuts off text if goes beyond x or y (if goes beyond x but has WRAP as well, wrap still works)
        BACKGROUND: underneath the graphics draws a background!
    */
    int totalOffsetX = 0;
    int totalOffsetY = 0;
    int curembedstart = 0;
    for (int i = 0;i<generatedTexture.size();i++) {//calculate offset & plugins
        int w = 0;
        int h = 0;
        if (i==embedStarts[curembedstart]) {
            if (data!=NULL) {
                std::string textShouldBeGenerating = actualText->stringsToAppend[curembedstart];
                std::string newData = data->getData(textShouldBeGenerating);
                if (newData!=textShouldBeGenerating) {
                    if (generatedTexture[i]!=NULL) {
                        SDL_DestroyTexture(generatedTexture[i]);
                    }
                    generatedTexture[i] = getTextAsTexture(newData, size, color,angle,&(textureOffsets[i].x),&(textureOffsets[i].y));
                }
            }
            curembedstart++;
        }
        SDL_QueryTexture(generatedTexture[i], NULL, NULL, &w, &h);
        totalOffsetX+=w;
        //totalOffsetY+=h;
        totalOffsetY=(totalOffsetY<h)?h:totalOffsetY;
    }
    
    int initialOffsetX = 0;
    int initialOffsetY = 0;
    
    if (attributes&CENTER_X) {//if should x-center
        initialOffsetX = (width-totalOffsetX)/2;
    }
    if (attributes&CENTER_Y) {//if should y-center
        initialOffsetY = (height-totalOffsetY)/2;
    }
    if (attributes&CUTOFF) {
        #ifdef DEBUGMODE
            throw std::runtime_error("ERROR CUTOFF NOT ADDED");
        #endif
    }
    if (attributes&BACKGROUND) {
        if (attributes&CUTOFF) {
            drawBorderedRect(x, y, width, height, bgcolor, bgborder);
        }
        else {
            drawBorderedRect(x, y, totalOffsetX, totalOffsetY, bgcolor, bgborder);
        }
    }
    
    int accumulatedOffsetY = 0;
    int xPointer = 0;//for wraps
    int yPointer = 0;
    for (int i = 0;i<generatedTexture.size();i++) {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(generatedTexture[i], NULL, NULL, &w, &h);
        if (attributes&WRAP) {
            if (xPointer+w > width) {
                xPointer = 0;
                yPointer+=accumulatedOffsetY;
            }
        }
        drawGraphic(
                x+textureOffsets[i].x+xPointer*cos_theta+initialOffsetX, y+textureOffsets[i].y+yPointer+initialOffsetY,
                w, h, generatedTexture[i]);
        xPointer+=w;
        accumulatedOffsetY=(accumulatedOffsetY>h)?accumulatedOffsetY:h;
    }
    parent->offset.x = xPointer;
    parent->offset.y = accumulatedOffsetY+yPointer;
}
void LocalizedText::draw(std::string s,int x,int y,int w,int h,Uint8 attributes,EmbedTextData* data,Uint32 bgcolor,Uint32 bgborder) {
    componentFromString(s)[0]->internalDraw(x, y,w,h,attributes,data,bgcolor,bgborder,this);
}
void LocalizedText::draw(std::string s,int x,int y,EmbedTextData* data) {
    componentFromString(s)[0]->internalDraw(x, y,-1,-1,NO_FLAG,data,0x00000000,0x00000000,this);
}
void LocalizedText::draw(std::string s,int x,int y,Uint8 attributes,EmbedTextData* data,Uint32 bgcolor,Uint32 bgborder) {
    componentFromString(s)[0]->internalDraw(x, y,-1,-1,attributes,data,bgcolor,bgborder,this);
}
std::string LocalizedText::getKey() {
    return key;
}

Point<int> LocalizedText::last_offset() {
    return offset;
}
LocalizedText::LocalizedText(std::string k) {
    key = k;
}
void LocalizedText::drawNew(std::string s,int x,int y,int size,Uint32 color,float angle,std::string grouper) {
    std::string toBeKey = replaceInString(s,'.','`');
    if (grouper!=""&&componentExists(grouper+"."+toBeKey)) {//"Grouper" grouping already exists, add to it
        componentFromString(grouper+"."+toBeKey)[0]->internalDraw(x, y, -1, -1, NO_FLAG, NULL, 0x000000, 0x000000, this);//->drawNew(s,x,y,size,color,angle,"");
    }
    else {//full "Grouper" grouping does not exist, create it
        std::vector<std::string> groups = splitAt(grouper, '.');
        if (grouper!="") {
            if (componentExists(groups[0])) {
                //need to go deeper
                std::string temp = groups[0];
                groups.erase(groups.begin());
                componentFromString(temp)[0]->drawNew(s, x, y, size, color, angle, assembleString(groups,"."));
            }
            else {
                //need to create more groups
                components.push_back(new LocalizedText(groups[0]));
                groups.erase(groups.begin());
                components[components.size()-1]->drawNew(s, x, y, size, color, angle,assembleString(groups,"."));
            }
        }
        else {
            //is at a leaf!
            components.push_back(new LocalizedText(toBeKey,new EmbedText(s),size,color,angle));
            draw(s, x, y);
        }
    }
}

//Embed Text Class
EmbedText::EmbedText(std::string inputToParse) {
    //Format input into parseable thingamajig for later
    int whichToAppend = 0;
    stringsToAppend = {""};
    bool insideQuotationMarks = false;
    for (int i = 0;i<inputToParse.size();i++) {
        if (inputToParse[i]=='"') {                         //Ignore quotation marks
            if (i-1>=0&&inputToParse[i-1]!='\\') {          //Unless they are \"
                insideQuotationMarks=!insideQuotationMarks;
                continue;
            }
            else if (i-1>=0) {
                stringsToAppend[whichToAppend][i-1]='"';    //In which case, get rid of the \ and replace it with "
            }
            else {
                insideQuotationMarks=!insideQuotationMarks;
                continue;
            }
        }
        if (!insideQuotationMarks&&(inputToParse[i]=='['||inputToParse[i]==']')) {
            continue;//skip []s unless meant to be there, indicated by quotations surrounding them
        }
        if (inputToParse[i]=='+'&&!insideQuotationMarks) {
            whichToAppend++;
            stringsToAppend.push_back("");
            continue;
        }
        stringsToAppend[whichToAppend].push_back(inputToParse[i]);
    }
    
}
std::string EmbedText::getText(EmbedTextData* data) {
    //Returns the correct string to draw
    std::string toReturn = "";
    int l_STA = (int)(stringsToAppend.size());
    for (int i = 0;i<l_STA;i++) {
        toReturn+=data->getData(stringsToAppend[i]);
    }
    return toReturn;
}


//EmbedTextData Class
std::string EmbedTextData::getData(std::string tag) {
    for (int i = 0;i<data.size();i++) {
        if (data[i][0]==tag) {
            return data[i][1];
        }
    }
    return tag; //if no tag corresponds to it then just return the tag!
                //this is good because we can then treat everything like a tag
}
EmbedTextData::EmbedTextData(std::vector<std::vector<std::string>> inputData) {
    data=inputData;
}
void EmbedTextData::addData(EmbedTextData* toAdd) {
    for (int i = 0;i<toAdd->data.size();i++) {
        data.push_back(toAdd->data[i]);
    }
}

void EmbedTextData::addData(std::vector<std::string> toAdd) {
    data.push_back(toAdd);
}
