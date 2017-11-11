//
//  AlitAn_lib.hpp
//  Solve Flo
//
//  Created by Bailey Andrew on 10/11/2017.
//  Copyright © 2017 Alliterative Anchovies. All rights reserved.
//

#ifndef AlitAn_lib_hpp

#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include <thread>
#include <vector>
#include <fstream>//read & write files
#include <dirent.h>
#include <assert.h>
//#include "Bulk Needed By All.hpp"

template<typename T> class Point {
    public:
        T x=0;
        T y=0;
        Point(T inx,T iny);
};

//Functions:
void drawBorderedRect(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a);
void drawText(std::string text,int text_size,int x,int y, Uint8 r,Uint8 g,Uint8 b);
void drawTextWithBackground(std::string text,int text_size,int x,int y, Uint32 txtColor, Uint32 bgColor, Uint32 edgeColor);
void drawTextInBorder(std::string text,int text_size,int x,int y, int w,int h,Uint8 r,Uint8 g,Uint8 b);
int drawTextInBorderSmartWrap(std::string text,int text_size,int x,int y, int w,int h,Uint8 r,Uint8 g,Uint8 b);
void drawRectBorder(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a);
void drawGraphic (int px,int py,int wid,int hei,SDL_Texture* graph);
void drawRect(int px,int py,int wid,int hei,Uint8 r,Uint8 g,Uint8 b,Uint8 a);
void drawRectWithBorderGraphics(int px,int py,int wid,int hei,SDL_Surface* fill,SDL_Surface* border,SDL_Surface* corner);
bool pointInBounds(int px,int py,int bx,int bxf,int by,int byf);
bool pointInCircle(int px,int py,int cx,int cy,int r);
int hexCharToInt(char in);
void drawLineOnSurface(SDL_Surface* theSurface,double x1,double x2,double y1,double y2,Uint32 color);
void fillAreaOnSurface(SDL_Surface* theSurface,Uint32 fillColor,int centerx,int centery);//DEPRECATED
void drawCircleOnSurface(SDL_Surface* theSurface,int centx,int centy,float radius,Uint32 color);
void drawHollowCircleOnSurface(SDL_Surface* theSurface,int centx,int centy,float radius,Uint32 color);
float meanTimeToHappenToPercent(float finalPercent,float time);
Uint32 get_pixel32( SDL_Surface *surface, int x, int y );
void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel );
Uint32 convertToRGBA(Uint32 ARGB);
void drawGraphicFlipped (int px,int py,int wid,int hei,SDL_Texture* graph,SDL_RendererFlip flipflag);
SDL_Surface* rotateSurfaceNinetyDegreesCounterClockwise(SDL_Surface* toFlip);
SDL_Surface* flipSurfaceOverYAxis(SDL_Surface* toFlip);
SDL_Surface* flipSurfaceOverXAxis(SDL_Surface* toFlip);
void drawTextToSurface(std::string text,int text_size,int x,int y, Uint8 r,Uint8 g,Uint8 b,SDL_Surface* surf);
SDL_Surface* createBlankSurfaceWithSize(int sx,int sy);
std::vector<std::string> split(const std::string &text, char sep);
int getHeightTakenByDrawingTextInBorder(std::string text,int text_size,int w,int h);
int getWidthTakenByDrawingText(std::string text,int text_size);
float randomOnRange(float s,float e);
Uint32 invertColor(Uint32 c);
Uint8 getA(Uint32 color);
Uint8 getR(Uint32 color);
Uint8 getG(Uint32 color);
Uint8 getB(Uint32 color);
std::vector<std::string> splitAtColon(std::string input);
std::vector<std::string> splitAt(std::string input,char splitter);
bool stringContains(std::string theString,char toContain);
float floatFromString(std::string theString);
std::vector<std::string> splitAtIgnoringBetween(std::string input,char splitter,char starter,char ender);
SDL_Surface* shadeSurface(SDL_Surface* toShade,Uint32 colorOver);
Uint32 blend(Uint32 c1, Uint32 c2);
std::string floatToString(float in);//trim unnecessary zeroes;
bool intVectorContains(std::vector<int> v,int toContain);
bool stringVectorContains(std::vector<std::string> v,std::string toContain);
//float fabs(float in) noexcept;//float abs()
void cubicBezier(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4,Uint32 color,int thicknessRadius,float increment=0.001);
void quadraticBezier(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float cx,float cy,Uint32 color,int thicknessRadius,float increment=0.001);
void quadraticBezierWithEdges(SDL_Surface* toDraw,float x1,float y1,float x2,float y2,float cx,float cy,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment=0.001);
void compositeQuadraticBezierWithEdges(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,float firstControlX,float firstControlY,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment=0.001);
void funkyCurveWithEdges(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,float firstControlX,float firstControlY,Uint32 color,Uint32 edgeColor,int thicknessRadius,int edgeSize,float increment=0.001);
bool shiftPressed();
float slopeOfLine(float startx,float starty,float endx,float endy);
void drawLinesFromCollectionOfPoints(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,Uint32 color);
void drawBorderedRect(int px,int py,int wid,int hei,Uint32 fillColor,Uint32 borderColor);
void drawRect(int px,int py,int wid,int hei, Uint32 color);
void drawText(std::string text,int text_size,int x,int y, Uint32 color);
void drawRoundedLinesetFromCollectionOfPoints(SDL_Surface* toDraw,std::vector<float> pointsToPassThroughX,std::vector<float> pointsToPassThroughY,Uint32 color,Uint32 edgeColor,int thickness=1);
bool stringIsWhitespace(std::string s);
std::string trim(const std::string &s);
bool stringContains(std::string s,char c);
Uint32 hexadecimalFromString(std::string input);
bool charIsNumeral(char input);
void drawTextAtAngle(std::string text,int text_size,int x,int y, Uint32 color,int angle);
//SDL_Surface* getTextAsSurface(std::string text,int text_size,Uint32 color);
SDL_Surface* rotateSurfaceByAngleAboutCenter_radians(SDL_Surface* inSurf,float angle,int* resizeOffsetX,int* resizeOffsetY);
//void drawLine(int x,int y,int x2,int y2,Uint32 color);
//Uint32 invertColor(Uint32 color);
void fillTriangleOnSurface(SDL_Surface* toDraw,int x1,int y1,int x2,int y2,int x3,int y3,Uint32 color);
void fillPolygonOnSurface(SDL_Surface* theSurface,Uint32 fillColor,std::vector<float> x,std::vector<float> y);
void fillPolygonOnSurface_old(SDL_Surface* theSurface,Uint32 fillColor,int centerx,int centery,std::vector<float> x,std::vector<float> y);
void drawLineOnSurfaceSafe(SDL_Surface* theSurface,double x1,double y1,double x2,double y2,Uint32 color);
void drawCenteredText(std::string text,int text_size,int x,int y,int w,int h,Uint32 color);
void fastSineCosine_float(float* sine,float* cosine,float angle);
bool pointIsOnSegment(Point<int> segstart,Point<int> segend,Point<int> tocheck);
template<typename numtype> std::string numberToString(numtype toBeString);
std::string assembleString(std::vector<std::string> a,std::string between = "");
std::string replaceInString(std::string theString,char toReplace,char with);
void drawCircle(int cx,int cy,int r,Uint32 color);
void drawHollowCircle(int cx,int cy,int r,Uint32 color);
void drawBorderedCircle(int cx,int cy,int r,Uint32 color,Uint32 bgColor);
template<class A,class B,class C> C foldr(A f1,std::vector<B> vec,C first);//folds list using function right-associatively
template<class A,class B> auto map(A toMap,std::vector<B> mapOnto);//maps function onto list
template<class A,class B> auto mapWithIndex(A toMap,std::vector<B> mapOnto);//maps function onto list, function also takes index as variable
template<class A,class B> B foldr1(A f1,std::vector<B> vec);//folds list using function right-associatively, first value of vector is first returned
template<class A,class B> void mapDo(A toMap,std::vector<B> mapOnto);//maps function onto list but toMap returns void
template<class A,class B> std::vector<B> filter(A f1,std::vector<B> vec);
//Template implementations
template<class A,class B,class C> C foldr(A f1,std::vector<B> vec,C first) {
    C output = first;
    for (int i = 0;i<vec.size();i++) {
        output = f1(vec[i],output);
    }
    return output;
}
template<class A,class B> B foldr1(A f1,std::vector<B> vec) {
    B output = vec[0];
    for (int i = 1;i<vec.size();i++) {
        output = f1(vec[i],output);
    }
    return output;
}
template<class A,class B> std::vector<B> filter(A f1,std::vector<B> vec) {
    std::vector<B> output = {};
    for (int i = 1;i<vec.size();i++) {
        if (f1(vec[i])) output.push_back(vec[i]);
    }
    return output;
}
template<class A,class B> auto map(A toMap,std::vector<B> mapOnto) {
    auto toReturn = std::vector<decltype(toMap(mapOnto[0]))>(mapOnto.size());
    for (int i = 0;i<mapOnto.size();i++) {
        toReturn[i] = toMap(mapOnto[i]);
    }
    return toReturn;
}
template<class A,class B> void mapDo(A toMap,std::vector<B> mapOnto) {
    for (int i = 0;i<mapOnto.size();i++) {
        toMap(mapOnto[i]);
    }
}
template<class A,class B> auto mapWithIndex(A toMap,std::vector<B> mapOnto) {
    auto toReturn = std::vector<decltype(toMap(mapOnto[0],0))>(mapOnto.size());
    for (int i = 0;i<mapOnto.size();i++) {
        toReturn[i] = toMap(mapOnto[i],i);
    }
    return toReturn;
}

//Overload Operators

//Strategy for custom operators (making it actually be 2 operators in disguise using define) provided by Iorro from StackExchange
//https://stackoverflow.com/questions/8425077/can-i-create-a-new-operator-in-c-and-how

//Intermediate stage definition
template<typename LHS, typename OP> struct LHSlt {
    std::string lhs_;
};
//Make a dummy 'class' to perform the intermediate operations on (concatenateString is an instance of this class)
enum { concatenateString };
//First operation: x*concatenateString in x*concatenateString*y
template<typename LHS> typename
std::enable_if<
        std::is_arithmetic<LHS>::value,
        LHSlt<LHS, decltype(concatenateString)>
>::type operator*(const LHS& lhs, decltype(concatenateString)) {
    std::string toReturn = std::to_string(lhs);
    return { toReturn };
}
template<typename LHS> typename
std::enable_if<
        !std::is_arithmetic<LHS>::value,
        LHSlt<LHS, decltype(concatenateString)>
>::type operator*(const LHS& lhs, decltype(concatenateString)) {
    std::string toReturn = lhs;
    return { toReturn };
}
//Convert from intermediate state to result (second * in x*concatenateString*y)
template<typename LHS,typename generictype> typename
std::enable_if <
        std::is_arithmetic<generictype>::value,
        std::string
>::type /*std::string*/ operator*(LHSlt<LHS, decltype(concatenateString)> lhsof, generictype rhs) {
    std::string lhs = lhsof.lhs_;
    //since it is a number type, convert to string
    return lhs+std::to_string(rhs);
}

template<typename LHS,typename generictype> typename
std::enable_if <
    !(std::is_arithmetic<generictype>::value),
    std::string
>::type /*std::string*/ operator*(LHSlt<LHS, decltype(concatenateString)> lhsof, generictype rhs) {
    std::string lhs = lhsof.lhs_;
    //string literal
    return lhs+rhs;
}
#define _C_ *concatenateString* //this makes _C_ act like *concatenateString* so it looks like it behaves like an operator


//2nd generation text functions, used for LocalizedText
SDL_Surface* getTextAsSurface(std::string text,int text_size,Uint32 color,float angle = 0.0,int* offsetX=NULL,int* offsetY=NULL);
SDL_Texture* getTextAsTexture(std::string text,int text_size,Uint32 color,float angle = 0.0,int* offsetX=NULL,int* offsetY=NULL);

//File Reading Functions and Classes
const float PARSED_FILE_VERSION_NUMBER = 2.0;
class ParsedFile {
    private:
        float version = -1;                                 //Versions are used as safety measures
                                                            //in case the method of parsing changes
                                                            //For now, throw error if not equivalent
                                                            //to PARSED_FILE_VERSION_NUMBER
        std::string tag = "";                               //Every file has a tag, which is how
                                                            //other files can reference this
                                                            //If "" then throw an error
    protected:
        std::vector<ParsedFile*> components;                //List of all the pieces of data in this
                                                            //file
        ParsedFile();                                       //Dummy constructor to get around the dumb
                                                            //rule that I have to initialize the parent
                                                            //clase in a constructor for the child class
        ParsedFile(std::vector<std::string> relevantInput); //Actually where all the logic goes in to
                                                            //Creating a new parsed file
        std::string key = "";                               //How it is referenced programatically
        std::string value = "";
        std::vector<ParsedFile*> internalComponentFromString(std::vector<std::string> componentID);
                                                            //Where the actual request for information
                                                            //goes after it is preprocessed by
                                                            //componentFromString
    public:
        static ParsedFile* parseFile(std::fstream* fileToParse);
                                                            //Splits fileToParse into array of strings
                                                            //to feed into main constructor
        std::string valueOf(std::string componentID,int which=0);
                                                            //returns value of component returned
                                                            //by componentFromString
                                                            //which is the index, since actually
                                                            //acting on an array
        std::vector<std::string> valuesOf(std::string componentID);
                                                            //returns array of every component
                                                            //found by componentFromString, but all
                                                            //turned into std::string (their 'value')
        bool componentExists(std::string componentID);      //returns true if component with
                                                            //componentID actually exists
        std::vector<ParsedFile*> componentFromString(std::string componentID);
                                                            //Gets components with key that matches
                                                            //componentID - note that componentID
                                                            //can use "." to indicate nesting
                                                            //for example initialInformation.name.first
                                                            //would grab the component initialInformation,
                                                            //then grab it's component name,
                                                            //then grab it's component first,
                                                            //returning the value of first.
        std::string getTag();                               //Gets the tag of this file
        std::string getValue();                             //Returns the value held in this component
        std::string getKey();                               //Returns the key of this component
};
class EmbedTextData {
    protected:
        std::vector<std::vector<std::string>> data;
    public:
        std::string getData(std::string tag);
        EmbedTextData(std::vector<std::vector<std::string>> inputData);
        void addData(EmbedTextData* toAdd);
        void addData(std::vector<std::string> toAdd);
};
class EmbedText {
    public:
        std::vector<std::string> stringsToAppend;
        /*
        embeddedDataTags:
            FIRST_NAME -> first name of sender
            LAST_NAME -> last name of sender
            LAST_NAME_SHORT -> last name of sender without the 'of' if it has an 'of'
            DATA -> get embedded data sent in through std::string embeddedData
            RULER_RELIGION -> get name of religion of ruler (ex islam)
            RULER_RELIGION_FOLLOWER -> get name of follower of religion of ruler(ex muslim)
            RULER_RELIGION_FOLLOWER_PLURAL -> get name of plural of follower of religion of ruler (ex muslims)
        */
        //std::string getText(Person* pConcerned,std::string embeddedData,Province* provConcerned=NULL,Nation* natConcerned=NULL);
        //std::string getText(Person* pConcerned,std::vector<std::string> embeddedData,Province* provConcerned=NULL,Nation* natConcerned=NULL);
        std::string getText(EmbedTextData* data);
        EmbedText(std::string inputToParse);
};
extern std::string localizationLanguage;
const Uint8 NO_FLAG =   0b00000000;
const Uint8 CENTER_X =  0b00000001;//centers the LocalizedText x-wise
const Uint8 CENTER_Y =  0b00000010;//centers the LocalizedText y-wise
const Uint8 WRAP =      0b00000100;//wraps the localized text
const Uint8 CUTOFF =    0b00001000;//cuts the localized text off if it goes beyond the y boundaries
const Uint8 BACKGROUND =0b00010000;//colors in background of text, does it to computed size of text unless cutoff is there as well in which does given size
class LocalizedText {
    private:
        std::vector<SDL_Texture*> generatedTexture={};//Textures to be drawn
        std::vector<Point<int>> textureOffsets={};
        EmbedText* actualText;
        std::string key = "";
        std::vector<LocalizedText*> components = {};
        std::vector<int> embedStarts = {};//index of generatedTexture is outputed given an index that represents the index of actualText's text
        std::vector<std::string> wordByWord = {};
        int size;
        float angle;
        Uint32 color;
        float sin_theta;
        float cos_theta;
        //int offsetX = 0;
        //int offsetY = 0;
        Point<int> offset = Point<int>(0,0);
    protected:
        LocalizedText(std::string k,EmbedText* text,int size_in,Uint32 color_in,float angle_in = 0.0);//for making leaves
        LocalizedText(std::string k);//for making branches after initial creation using the ParsedFile
        std::vector<LocalizedText*> internalComponentFromString(std::vector<std::string> componentID);
        void internalDraw(int x,int y,int width,int height,Uint8 attributes,EmbedTextData* data,Uint32 bgcolor,Uint32 bgborder,LocalizedText* parent);
    public:
        LocalizedText(ParsedFile* p);//create localized text tree from parsed file
        //following functions emulate ParsedFile in functionality
        bool componentExists(std::string componentID);
        std::vector<LocalizedText*> componentFromString(std::string componentID);
        void draw(std::string s,int x,int y,EmbedTextData* data = NULL);
        void draw(std::string s,int x,int y,int w,int h,Uint8 attributes=NO_FLAG,EmbedTextData* data = NULL,Uint32 bgcolor=0x00000000,Uint32 bgborder=0x00000000);
        void draw(std::string s,int x,int y,Uint8 attributes,EmbedTextData* data = NULL,Uint32 bgcolor=0x00000000,Uint32 bgborder=0x00000000);
        void drawNew(std::string s,int x,int y,int size,Uint32 color,float angle,std::string grouper);//draws text not from localized file and saves it
        std::string getKey();
        Point<int> last_offset();//returns point of total width/height calculated from last drawing
};


//Variables
extern SDL_Renderer* gRenderer;//The window renderer
extern SDL_Surface* gScreenSurface;//The surface contained by the window
extern SDL_Window* gWindow;//The window we'll be rendering to
extern SDL_Texture* mapToDraw;
extern SDL_Rect stretchRect;
//const int SCREEN_WIDTH = 1290;
//const int SCREEN_HEIGHT = 800;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern TTF_Font* theFont24;
extern TTF_Font* theFont20;
extern TTF_Font* theFont16;


#endif /* AlitAn_lib_hpp */

