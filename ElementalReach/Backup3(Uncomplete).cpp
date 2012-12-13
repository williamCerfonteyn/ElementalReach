#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

///Current Development version: 1.0

///PLEASE READ:
//This application is NOT cross plateform friendly, there are windows functions used.
//Mostly it is sleep, just run a find function in your compiler and find a suitable replacement for it, since this is only going to run on windows for vs 1.0

///ResourceSystem needs to be check if fireParts hanve arrived yet in the fireChain animation I belive, just check, then I think testing can happen.

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
#include <windows.h>

#include "SDL_image.h"
//My knowledge of sound/music isn't great, so the code could be sloppy.
#include "SDL_mixer.h"
#include "SDL_audio.h"
//#include "SDL_sound.h"

#include <time.h>
#include <list>

#define GL_BGR 0x80e0
/* Very NB stuff.
Vec2f cursor_pos = get_cursor_pos();
Vec2f elemental_pos = get_elemental_pos();

Vec2f direction = cursor_pos - elemental_pos;
direction = direction / length(direction);

float speed = 5;

elemental_pos = elemental_pos + direction*speed;

NB[2];
struct vec2f
{
float x, y;
};

vec2f elm_pos;
vec2f direction;
float distance;
float speed = 5;

direction.x = cursor.x - elm_pos.x;
direction.y = cursor.y - elm_pos.y;

//Length of the "direction" vector
distance = sqrt(direction.y*direction.y + direction.x*direction.x);

//normalise the direction vector, i.e., make sure it has length of 1
direction.x /= distance;
direction.y /= distance;

//update position along the direction vector
elm_pos.x = elm_pos.x + direction.x*speed;
elm_pos.y = elm_pos.y + direction.y*speed;

*/
GLuint fontText;

//Apparently const int are better than #defines.
const int SW = 850;
const int SH = 560;

const int TextNum = 4; ///Number of textures used.

//Cursor info:
const int amountOfCursorParts = 5;
const int cursorSize = 10;

//To keep the ratios between x/y the same, I would recommend keeping this as a multiple value of SW/SH. You can however, change this as you see fit.
const int worldCordsX = SW*3;
const int worldCordsY = SH*3;

///Prining system (NotificationSystem)
const int MaxConvoLines = 100; //Max lines that can be pushed into one object of convo.
const int lineSpacing = 15; //The space between lines as they are printed.
const int maxDelayTime = 5000; //Milisecond delay for the lines to appear in world method drawing.

const int SBPP = 16;//32
///
///
const int WHICH_TEXTURE_TO_TEST = 0; ///TEST POSITION.
///
const int TS = 30; //Texture size;
const int fireChainAttachOnResourceNodeAdditionLeverage = 10;
const float allParticles_base_life = 0.9;
///
const int FRAMES_PER_SECOND = 100; ///WINDOWS FUNCTION USED WITHIN THIS CLASS.
///
const int scrollingLeverage = 5; //How far to the side does the cursor need to be for scrolling to happen.
const float scrollSpeed = 5;
///
const float displayReduction = 5; //Reduces amount of particles displayed to increase performance.
///
//RESOURCES INFO:
const int maxNodeStrength = 120;
const int initialNodePower = 50;
const int maxNodesOnMap = 20;


//FIRE ELEMENTAL INFO:
const int totalParticlesPerClass = 1000;
const int fireElemSize = 50;

const int lifeDiminishingFactor = 15;

const int fireElementalStartingPositionX = 100; //This is the starting position relative to the SCREEN.
const int fireElementalStartingPositionY = 50; //As above.

const int WorldStartingElementalPositionX = 0;
const int WorldStartingElementalPositionY = 0;

const int initialStrength = 800;
const int roguePartsOnMap = 300;
const float sizeIncreasedPerPart = 0;
const float StartingMoveSpeedOFFireElemental = 1;

const float AbsorptionDelay = 5.0; //Seconds.
const int MaxParticlesInFireChain = 400;
const int maxFireChains = 50;

//FIRE CHAINS INFO:
const int movementRateOfFireChainParts = 20;
const int FireChainsParticleFeedRatePerSecondFromFireElemental = 18; //Would advice to be linked to firePart movement
const int cursorLeverage = 5;
///Becomes FASTER the LOWER the number is.
const int fireChainSettleSpeedDivided = 10; //Speed where fireChain has settled (In a node ect) and when movement happens, how fast does it react to get more parts ect.

using namespace std;

enum FirespellTypes {
    FC
};

/* Bit Masking template/preview. For later use.
const unsigned int BUFFER_BIT_1 = 00001000; //if we would makes this 0000110 ten it would also look as though BB2 was passes.. why?
const unsigned int BUFFER_BIT_2 = 00000100; //if there is a 1 at the place of another buffer, then when its' ored together it looks
const unsigned int BUFFER_BIT_3 = 00000010; //as though that buffer was also passed, for eg.\/

void Bitmasking(unsigned int byte)
{
    if(byte & BUFFER_BIT_1)
        cout << "BUFFER_BIT_1 - Passes.\n\n";

    if(byte & BUFFER_BIT_2)
        cout << "BUFFER_BIT_2 - Passes.\n\n";

    if(byte & BUFFER_BIT_3)
        cout << "BUFFER_BIT_3 - Passes.\n\n";
}

    Bitmasking( BUFFER_BIT_1 | BUFFER_BIT_2);
*/


struct vec2f{
    float x, y;
};

GLuint texture[TextNum];
SDL_Surface *surface;

///Important functions: (Some random template stuff that I kept for referencing)
///    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
///    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
///    glBlendFunc(GL_DST_COLOR,GL_ZERO);
///    glBlendFunc(GL_ONE, GL_ONE);
///    glDisable(GL_DEPTH_TEST);
///    glLineWidth(2.0);
///    glColor4f(1.0, 1.0, 1.0, 1.0);
///    SDL_GetTicks();
///

class scrollingFeature {
public:
    scrollingFeature();
    void scrolling();
    float drawInRelationToScreenX(float); //Yes I believe coordinates could be used to limit this to one function, but that will only be needed in vs. 1.1.
    float drawInRelationToScreenY(float);
    bool inScreen(float,float,float); //Please read above drawInRelationToScreen definition.
    float adjustInputValueX(float inputValueX) { return (inputValueX+screenXCords);}
    float adjustInputValueY(float inputValueY) { return (inputValueY+screenYCords);}
    void shakeCameraInitiation(float duration, int intensity);
    void shakeCamera();
protected:
    float screenXCords;
    float screenYCords;
    float scrollingMultiplierX;
    float scrollingMultiplierY;
    bool checkIfScroll(float,float);
    void handleEndMapCollision();

    float shakeDur;
    int shakeInten;

    float shakeX;
    float shakeY;

    float referenceTime;
    bool durationStillActive(); //Once again, we need to combine this.
    float calmingDown;

    void calculateCalmingDown();

};

void scrollingFeature::calculateCalmingDown()
{
    float checkCurrentTime = SDL_GetTicks();

    calmingDown = (shakeDur - (checkCurrentTime-referenceTime))/shakeDur;
}

bool scrollingFeature::durationStillActive()
{
    if(shakeInten != 0){ //Then it can obviously not shake anymore, maybe a little messy, but it's very obvious.
    float checkCurrentTime = SDL_GetTicks();

    if((checkCurrentTime-referenceTime) > shakeDur){
        return false;
    }

    return true;
    }
}

void scrollingFeature::shakeCamera()
{
    if(durationStillActive()){

        calculateCalmingDown();

        shakeX = ((rand()%shakeInten*2) - shakeInten)*calmingDown;
        shakeY = ((rand()%shakeInten*2) - shakeInten)*calmingDown;
    }

    else
    {
        shakeX = 0; //The identity coeffcient of multiplication.
        shakeY = 0;
    }
}

void scrollingFeature::shakeCameraInitiation(float duration, int intensity) //Because rand works with ints.
{
    referenceTime = SDL_GetTicks();
    shakeDur = duration*1000;
    shakeInten = intensity;
}

void scrollingFeature::handleEndMapCollision()
{
    if(screenXCords <= 0)
        screenXCords = 0;

    if(screenXCords+SW >= worldCordsX)
        screenXCords = worldCordsX-SW;

    if(screenYCords <= 0)
        screenYCords = 0;

    if(screenYCords+SH >= worldCordsY)
        screenYCords = worldCordsY-SH;
}

scrollingFeature::scrollingFeature()
{
    //It's important to note that this will be the screen's relativity to the world coordinates;
    //Starting cords of the human elemental;
    screenXCords = WorldStartingElementalPositionX;
    screenYCords = WorldStartingElementalPositionY;

    scrollingMultiplierX = 0;
    scrollingMultiplierY = 0;

    shakeInten = 0;
    shakeDur = 0;

    shakeX = 0;
    shakeY = 0;

}
///It's very important to note that inScreen could not be used as a protected function and only drawInRelationToScreen is called from the other class, the reason is:
///There needs to be an external test to see if drawing is neccesary to save processing power, we can not test when drawing, because a value needs to be returned then:
/// float DIRTS(float,float) { return ?? //What return to the vertex functions if inScreen is false? }. Hope that clarifies it.
float scrollingFeature::drawInRelationToScreenX(float objectWorldCordsX)
{
    //The objects variables will always be more than the screen, because it is in the screen...
    //cout << "\n\nobjectWorldCordsX: " << objectWorldCordsX;
    //cout << "\nscreenXCords     : " << screenXCords;
    //cout << "\n(objectWorldCordsX - screenXCords): " << (objectWorldCordsX - screenXCords);
    //Sleep(10);
    return (objectWorldCordsX - screenXCords+shakeX);
}

float scrollingFeature::drawInRelationToScreenY(float objectWorldCordsY)
{
    //The objects variables will always be more than the screen, because it is in the screen... Ahh now you can see I just copied above xD
    //cout << "\n\nobjectWorldCordsY: " << objectWorldCordsY;
    //cout << "\nscreenYCords     : " << screenYCords;
    //cout << "\n(objectWorldCordsY - screenYCords): " << (objectWorldCordsY - screenYCords);
    //Sleep(10);
    return (objectWorldCordsY - screenYCords+shakeY);
}

bool scrollingFeature::inScreen(float objectWorldCordsX, float objectWorldCordsY, float objectSize)
{
    if(((objectWorldCordsX+(objectSize)) >= screenXCords && (objectWorldCordsX-(objectSize)) <= (screenXCords+SW)) &&//Checks x boxing collision.
        ((objectWorldCordsY+(objectSize)) >= screenYCords && (objectWorldCordsY-(objectSize)) <= (screenYCords+SH)) ) //Checks y boxing collision.
        return true; //There were lots of debugging code here also, but don't want cluster.


    return false;
}

void scrollingFeature::scrolling()
{
    int mxTemp;
    int myTemp;

    SDL_GetMouseState(&mxTemp,&myTemp);

    float mx = mxTemp;
    float my = myTemp;

    //Not as difficult as it looks, just do the calcs on paper to see.
    if(checkIfScroll(mx, my)){
    scrollingMultiplierX = ((mx - (SW/2))/(SW/2));
    scrollingMultiplierY = ((my - (SH/2))/(SH/2));

    screenXCords += (scrollingMultiplierX * scrollSpeed);
    screenYCords += (scrollingMultiplierY * scrollSpeed);

    handleEndMapCollision();
    }
}

bool scrollingFeature::checkIfScroll(float mx, float my)
{
    //Important to note that reducing the amount of checks or variables has not proven viable. Little thought however was spent due to the logical nature of the problem, that 4 checks are needed
    //If these checks are not run, the screen will scroll the entire time. If there is an more efficient way it would be quite awesoome :).
    if(mx >= (SW - scrollingLeverage))
        return true;

    if(mx <= (0 + scrollingLeverage))
        return true;

    if(my >= (SH - scrollingLeverage))
        return true;

    if(my <= (0 + scrollingLeverage))
        return true;

    return false;
}

///Ofcourse this is a very arguable design, but I believe it's function is more valuable than its "unclassyness"
scrollingFeature scrollControl; //No rhyming intended...

bool loadTextures()
{
    SDL_Surface *textureImages[TextNum];

    if(!(textureImages[0] = IMG_Load("Smoothfire1.png"))) return false;
    if(!(textureImages[1] = IMG_Load("Smoke3.png"))) return false;
    if(!(textureImages[2] = IMG_Load("font1.png"))) return false;
    if(!(textureImages[3] = IMG_Load("Smoothfire1.png"))) return false;
 //   if(!(textureImages[4] = IMG_Load("Smoke.bmp"))) return false;
  //  if(!(textureImages[5] = IMG_Load("Smoke2.bmp"))) return false;
 //   if(!(textureImages[6] = IMG_Load("Smoothfire.bmp"))) return false;
    //if(!(textureImages[8] = IMG_Load(""))) return false;

    glGenTextures(TextNum, texture);

    SDL_WM_SetIcon(textureImages[3], NULL);

    for(int i = 0; i < TextNum; i++){
   // if(low){
   // glBindTexture(GL_TEXTURE_2D, texture[i]);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImages[i]->w, textureImages[i]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImages[i]->pixels);
   // }
   // if(medium){
   // glBindTexture(GL_TEXTURE_2D, texture[i]);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 //   glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImages[i]->w, textureImages[i]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImages[i]->pixels);
 //   }
  //  if(high){
    glBindTexture(GL_TEXTURE_2D, texture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, textureImages[i]->w, textureImages[i]->h, GL_RGBA, GL_UNSIGNED_BYTE, textureImages[i]->pixels);
  //  }
  //system("pause");
    }
    for(int i = 0; i < TextNum; i++){
    if(textureImages[i])
        free(textureImages[i]);
    }
    return true;
}
//Copied from template.
void BuildFont()
{
    float cx; //Varibles we are going to use;
    float cy;

    fontText = glGenLists(256); //how many lists.. 256.. not 255, 255 is for counting 0 - 255 = 256; How many lists we want to store in FT

    glBindTexture(GL_TEXTURE_2D, texture[2]);

    for(int i = 0; i < 256; i++){
        glNewList(fontText + (255 - i), GL_COMPILE); //0.0625 Tell opengl we want a new display list, and then start drawing;
        cx = 1 - (float)(i%16)/16; //Veriables to make life easier and have correct cords;
        cy = 1 - (float)(i/16)/16;
        glBegin(GL_QUADS);//Draw a quad where the letter will be stored in, everything is upside down, because of GL_RGB;
        glTexCoord2f(cx-0.0625,cy); glVertex2i(0,16); //Bottumleft
        glTexCoord2f(cx,cy); glVertex2i(16,16); //Botumright
        glTexCoord2f(cx,cy-0.0625); glVertex2i(16,0); //TopRight
        glTexCoord2f(cx-0.0625,cy-0.0625); glVertex2i(0,0); //Topleft
        glEnd(); //End the drawing of the letter within the display list;
        glTranslatef(15.0, 0, 0); //Translate to the side so the words do not get printed on top of one another.
        glEndList(); //And then we end the list, so we can start a new one or be done building the font;
    }
}

void Print(int x, int y, int set, char *string, ... )
{
    va_list ap;
    char text[128];

    x = scrollControl.drawInRelationToScreenX(x);
    y = scrollControl.drawInRelationToScreenY(y);

    if(string == NULL)
        return ;

    va_start(ap, string);
    vsprintf(text, string, ap);
    va_end(ap);

    if(set > 1)
        set = 1;

    glBindTexture(GL_TEXTURE_2D, texture[2]); //We need to use the font texture with the cords we set in buildfont to have letters.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glLoadIdentity();
    glTranslatef(x,y,0);

    glListBase( fontText - 32 + (128 * set) ); /*Ok lets explain. So we - 32 because the first 32 lists we never built - its not included
    in the texture font i supose. Then we + (128 * set) because there is actually only 128 symbols/letters, thus when we go over it for eg
    156 its just differnt font style. Thus when set is 1 its a differnt font style then when set is 0(BOLD). that is also why we can only
    have 128 letters - text[128] - because if we had more we give the opertunity to go from font styles we going over 128's style. Hope
    you understand more now :) */

    if(set == 0)
        glScalef(1.0, 1.0, 1.0);

    glCallLists(strlen(text), GL_BYTE, text);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
//Many roles overlap in the function/var department, maybe an enhanced inheritance can be implemented in vs 1.1.
class notificationSystem {
public:
    notificationSystem(bool isThisAWorldNotification, float delayTillSpeach = 0, float timeTakenToFin = 0);
    void addLine(char *string); //Using char *, because print works with this. We can maybe improve this in vs 1.1.
    void speak();
    void setPosition(float Rx, float Ry) { x = Rx; y = Ry; }
private:
    char* conversation[MaxConvoLines]; //Really going to struggle to get more than MaxConvoLines lines in 1 row. Yes, LL is needed, I know...
    float *delayTimer[MaxConvoLines];
    int totalConvoLines;
    float x;
    float y;
    bool active;

    bool worldNot;

    void worldNotificationMethod(float);
    void normalPrintingMethod();

    bool finished;
    float delayTillSpeak;
    float referenceTime;
    float timeTakenToFinish;
    bool speakNow;
    void checkToSpeak(); //Very much the same to other delay functions. Reconciliation is on the list yes.

};

void notificationSystem::worldNotificationMethod(float delaySomeMore)
{
    float checkCurrentTime = SDL_GetTicks();

    float startingTimeAndOn = checkCurrentTime-referenceTime - delayTillSpeak - delaySomeMore;

    glColor4f(1.0, 1.0-(startingTimeAndOn*(1/timeTakenToFinish)), 1.0-(startingTimeAndOn*(1/timeTakenToFinish)), 1.0-pow(((startingTimeAndOn - (timeTakenToFinish/2))/(timeTakenToFinish/2)), 2));
   // cout << "\n" << -pow((startingTimeAndOn - (timeTakenToFinish/2)), 2);
   // Sleep(1000);

    if((checkCurrentTime-referenceTime - delayTillSpeak) > (timeTakenToFinish+maxDelayTime)){
        finished = true;
        active = false;
    }
}

void notificationSystem::checkToSpeak()
{
    if(!active && !finished){
        float checkCurrentTime = SDL_GetTicks();

        if((checkCurrentTime-referenceTime) > delayTillSpeak)
            active = true;
    }
}

void notificationSystem::normalPrintingMethod()
{
    glColor4f(1.0, 0.0, 0.0, 1.0);
}

notificationSystem::notificationSystem(bool isThisAWorldNotification, float delayTillSpeach, float timeTakenToFin)
{
    totalConvoLines = 0;
    x = 0;
    y = 0;
    active = false;
    finished = false;

    timeTakenToFinish = timeTakenToFin*1000;
    referenceTime = SDL_GetTicks(); //Current time.

    delayTillSpeak = (delayTillSpeach*1000) + referenceTime;

    speakNow = false;

    worldNot = isThisAWorldNotification;
}

void notificationSystem::speak()
{
    checkToSpeak();

    if(active) //Active stops unnecessary calculations.
    {
        for(int i = 0; i < totalConvoLines; i++) //Printing text.
        {
            if(worldNot)
            worldNotificationMethod(*delayTimer[i]); //Not needed to pass the float, but it's nice design I believe. Should actually be in a class.

            else normalPrintingMethod();

            Print(scrollControl.drawInRelationToScreenX(x), scrollControl.drawInRelationToScreenY(y+i*lineSpacing), 1, conversation[i]);
        }
    }
}

void notificationSystem::addLine(char *stringx)
{
    //conversation[totalConvoLines] = new char*;
    conversation[totalConvoLines] = stringx;
    delayTimer[totalConvoLines] = new float; //Because I'm completely confused why this does not work.
    *delayTimer[totalConvoLines] = (maxDelayTime/5*totalConvoLines);//rand() % maxDelayTime;

    totalConvoLines++;
}

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
    /* clean up the window */
    SDL_Quit( );

    /* and exit appropriately */
    exit( returnCode );
}

bool resizeWindow( int w, int h )
{
    if(h==0)h=1; //Sigh.. please check your if's eh?
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return(true);
}

int initGL( )
{

   //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   // glDepthFunc(GL_LESS);

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    glLineWidth(2);

    srand(time(NULL));

    //SDL_WM_GrabInput(SDL_GRAB_ON);

    SDL_ShowCursor(0);

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    BuildFont();
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   // glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    //glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    if(!loadTextures()){
    cout << "FAILURE LOADING TEXTURES.";
    exit(1);
    }

    glEnable(GL_TEXTURE_2D);
    return(true);
}

void initCoutRedirecting() { ///CAUSE OF PROBLAMATIC EXIT. REMOVE WHEN DEBUGGING IS FINISHED.
//SDL_Init(/*whatever*/);
//AllocConsole();
freopen("CONOUT$", "wb", stdout);
freopen("CONOUT$", "wb", stderr);
freopen("CONIN$", "rb", stdin);
/*...*/
}

class particle{
protected:
    float x;
    float y;
    float ySpeed;
    float xSpeed;
    float life;
    float fade;
    int permaX;
    int permaY;
    bool active;
    float R, G, B;
    int size;
    int Text;
    int multiSizeX;
    int multiSizeY;
    bool sizeSet;

    void updates();

    float particleRotation;
    float particleRotationConcatenated;

    //void setRandomRotation() { float tempGetRand = rand()%200-100; particleRotation = tempGetRand/1000;}
public:
    particle();
    void move(); //Which also includes display

    void activate(bool yesorno);

    void setSize(int value) { size = value; }
    void setXspeed(float value) { xSpeed = value; }
    void setYspeed(float value) { ySpeed = value; }
    void setLife(float value) { life = value; }
    void setFade(float value) { fade = value; }
    void setColor(float r, float g, float b) { R = r, G = g, B = b; }
    void position(int,int);
    void setTexture(int T) { Text = T; }

    float Rlife() { return life; }
    bool isActive() { return active; }
    int returnSize() { return size; }
    float returnColorR() { return R; }
    float returnColorG() { return G; }
    float returnColorB() { return B; }
    int returnPermaX() { return permaX; }
    int returnPermaY() { return permaY; }
    float returnFade() { return fade; }
    float returnxSpeed() { return xSpeed; }
    float returnySpeed() { return ySpeed; }
    float returnX() { return x; }
    float returnY() { return y; }
    void setMultiSize(bool SS, int MSX, int MSY) { sizeSet = SS; multiSizeX = MSX; multiSizeY = MSY; }

    void movePartTo(particle *Part, int*);
};

void particle::updates()
{
    life -= fade;
    if(life < 0){
        x = permaX;
        y = permaY;
        life = allParticles_base_life;
    }
    x += xSpeed;
    y += ySpeed;
}

void particle::movePartTo(particle *part, int *particalCount) //Yea, not great design, shouldn't manipulate pointers. Will be fixed in vs 1.1.
{
    ///Because overloaded operators are too mainstream :P.
    if(*particalCount > totalParticlesPerClass)
    particalCount = 0;

    part->setLife(allParticles_base_life);
    part->activate(true);
    //part->setRandomRotation();
    //FEpart->setSize(size);
    //FEpart->setXspeed(xSpeed);
    //FEpart->setYspeed(ySpeed);
    //FEpart->setLife(life);
    //FEpart->setFade(fade);
    //FEpart->setColor(R, G, B);
    part->position(x,y);
    //FEpart->setTexture(Text);
    ///Many of these function are just here so you know what more this function can do.
}


void particle::activate(bool yesorno)
{
    active = yesorno;
}

particle::particle()
{
    sizeSet = false;
    multiSizeX = 0;
    multiSizeY = 0;
    active = false;
    ySpeed = 0;
    xSpeed = 0;
    life = allParticles_base_life;
    fade = 0;
    R = G = B = 1;
    Text = 0;
    size = TS;

    particleRotationConcatenated = 0;

    float tempGetRand = rand()%200-100;
    particleRotation = tempGetRand/1000;
}

void particle::move()
{
    if(active){ //If anything, it is just another layer of scrolling protection.

        updates();

      if(scrollControl.inScreen(x,y,size)){
          glLoadIdentity();

          float drawX = scrollControl.drawInRelationToScreenX(x);
          float drawY = scrollControl.drawInRelationToScreenY(y);

          glTranslatef(drawX,drawY,0);

        ///Took me an incredible amount of time to realise that my object's x values are being used and that's what makes it fail...
       // x = scrollControl.drawInRelationToScreenX(x); //Yes I believe coordinates could be used to limit this to one function, but that will only be needed in vs. 1.1.
       // y = scrollControl.drawInRelationToScreenY(y);

       // cout << "\n\nx: " << scrollControl.drawInRelationToScreenX(x);
       // cout << "\ny: " << scrollControl.drawInRelationToScreenY(y);

        //Sleep(20);

        glColor4f(R, G, B, life);

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture[Text]);

        glRotatef(particleRotationConcatenated,0,0,1);
        particleRotationConcatenated+=particleRotation;

        if(particleRotationConcatenated > 360)
            particleRotationConcatenated = 0;

        if(!sizeSet){
            glBegin(GL_TRIANGLE_STRIP); //Lets make the particles alot smaller...
            glTexCoord2f(1,1); glVertex2f(size,size); //The smaller the particles the more it'll look like smoke.. maybe even smaller >>.
            glTexCoord2f(0,1); glVertex2f(-size,size);
            glTexCoord2f(1,0); glVertex2f(size,-size);
            glTexCoord2f(0,0); glVertex2f(-size,-size);
            glEnd();
        }
        else {
            glBegin(GL_TRIANGLE_STRIP); //Lets make the particles alot smaller...
            glTexCoord2f(1,1); glVertex2f(drawX+multiSizeX,drawY+multiSizeY); //The smaller the particles the more it'll look like smoke.. maybe even smaller >>.
            glTexCoord2f(0,1); glVertex2f(drawX-multiSizeX,drawY+multiSizeY);
            glTexCoord2f(1,0); glVertex2f(drawX+multiSizeX,drawY-multiSizeY);
            glTexCoord2f(0,0); glVertex2f(drawX-multiSizeX,drawY-multiSizeY);
            glEnd();
        }

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        }
    }
}

void particle::position(int xa, int ya)
{
    permaX = xa;
    permaY = ya;
    x = permaX;
    y = permaY;
    //size = TS;
}

/*There are 3 algorithms to do this:
1) match every rogue particle with the fireElement to see if colision is true;
2) Save "zones" where particles are bound to be found, and check when near a zone. Problem if particles can move...
3) Build a linked list hierarchy where you say which parts are before or infront of the others, like a implicit system of information, so if you're not at the one, you can't be at the other.

The problem is the particles can move, so for now, I'm going with just the easier one - 1.
*/

class globalFunctions{ //Just because it's awesome!
public:
    globalFunctions() { currentPart = 0; partCount = 0;} //This is to account for a for loop oddness that defines 1 more to currentPart than needed.

    void reduceCurrentPartNum() {currentPart--;}
    void reducePartCount() {partCount--;}

    void increaseCurrentPartNum() {currentPart++;}
    void increasePartCount() {partCount--;}

    int returnCurrentPart() { return currentPart;}
    int returnPartCount() { return partCount;}

    void setPartCount(int setPartCount){ partCount = setPartCount; }
protected:
    int currentPart;
    int partCount;
    float getRadius(float distx, float disty);

    float absoluteValueF(float x) { return sqrt(pow(x, 2)); }
    int absoluteValueI(int x) { return sqrt(pow(x, 2)); }
    float randomize(float,float,bool);

    //  y|  \
    //  d|      \ radius
    //  i|          \
    //  s -------------
    //        <->
    //      Distance
    // :)
};

float globalFunctions::randomize(float maxRandomValue, float addToRandom, bool canBeNegative)
{
    int multiplier = 1; //Because int casting canBeNegative does not work.

    if(!canBeNegative)
        multiplier = 0;

    float temp = (float)(rand()%250*(multiplier+1) - 250*multiplier);

    if((addToRandom + ((temp/250)*maxRandomValue)) > maxRandomValue)
        addToRandom = ((temp/250)*maxRandomValue) - (maxRandomValue);

    return (((temp/250)*maxRandomValue) + addToRandom);
}

float globalFunctions::getRadius(float distx, float disty)
{
    float radiusSquared;
    float radius;

    radiusSquared = pow(distx,2) + pow(disty,2);
    radius = sqrt(radiusSquared);

    return radius;
}

class rogueParts: public globalFunctions{
public:
    rogueParts();
    int getPC() { return partCount; }
    particle *getPart(int whichOne) { return RP[whichOne]; }
    void animation();
    void allocateNewPart();
protected:
    particle *RP[totalParticlesPerClass]; //Max rogue particles available.
};

void rogueParts::allocateNewPart()
{
    RP[currentPart] = new particle;
}

void rogueParts::animation()
{
    for(int i = 0; i < partCount; i++)
    {
        if(RP[i]->isActive())
        RP[i]->move();
    }
}

rogueParts::rogueParts()
{
    for(int i = 0; i <= roguePartsOnMap; i++)
    {
        RP[i] = new particle;
        RP[i]->position(rand()%worldCordsX+1, rand()%worldCordsY+1);
        RP[i]->activate(true);

        currentPart = i;
        partCount = i;
    }
}

class resourceNode {
public:
    resourceNode(float positionX, float positionY, int nodeS);
    void animate();
    void drainingNode(particle *partx, int);
    void activateRN(bool act) { active = act; }
    bool isActive() { return active; }
    float giveXValue() {return x;}
    float giveYValue() {return y;}
    void setAsLinked(bool lin = true) { linked = lin; }
    void setAsConnected(bool con = true) { connected = con;}
    bool AmILinked() { return linked;}
    bool AmIConncted() { return connected;}
protected:
    particle *nodeStrength[maxNodeStrength];
    int nodePower;
    float x;
    float y;
    bool linked; //Means the firechain is going to be connected to this Node;
    bool connected; //Means the firechain IS connected to this node;
    bool active;
};

resourceNode::resourceNode(float positionX, float positionY, int nodeS = initialNodePower)
{
    linked = false;
    connected = false;
    x = positionX;
    y = positionY;
    nodePower = nodeS;

    for(int i = 0; i <= nodePower; i++)
    {
        nodeStrength[i] = new particle;
        nodeStrength[i]->activate(true);
        nodeStrength[i]->position(x,y);
    }
}

void resourceNode::animate()
{
    for(int i = 0; i < (int)(nodePower/displayReduction); i++)
    {
        if(nodeStrength[i]->isActive())
        {
            nodeStrength[i]->move();
        }
    }
}

void resourceNode::drainingNode(particle *partx, int totalParts)
{
    nodeStrength[nodePower]->movePartTo(partx, &totalParts);
    nodeStrength[nodePower]->activate(false);
    nodePower--;
}

class resourceSystem : public globalFunctions{
public:
    resourceSystem();
    void animates();
    void update();
    int howManyRNodesOnMap() { return nodesOnMap; }
    float getNodeXValue(int whichNode) {return RNs[whichNode]->giveXValue();}
    float getNodeYValue(int whichNode) {return RNs[whichNode]->giveYValue();}
    void setNodeAsLinked(int whichNode, bool lin = true) { RNs[whichNode]->setAsLinked(lin); }
    void setNodeASConnected(int whichNode, bool con = true) { RNs[whichNode]->setAsConnected(con); }
    bool IsNodeLinked(int whichNode) { return RNs[whichNode]->AmILinked();}
    bool IsNodeConnected(int whichNode) { return RNs[whichNode]->AmIConncted();}

    void checkToConnectYet(int, float, float);
protected:
    resourceNode *RNs[maxNodesOnMap]; //Probably not neccesary, but I like using pointers and assinging new.
    int nodesActive;
    int nodesOnMap;
    int randoming(int maxRand, int addition);
};

void resourceSystem::checkToConnectYet(int whichResourceNode, float X, float Y)
{
    if(!RNs[whichResourceNode]->AmIConncted())
    {
        if(X > (RNs[whichResourceNode]->giveXValue()-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && X < (RNs[whichResourceNode]->giveXValue() + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
            if(Y > (RNs[whichResourceNode]->giveYValue()-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && Y < (RNs[whichResourceNode]->giveYValue() + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
                   RNs[whichResourceNode]->setAsConnected();
    }
}

int resourceSystem::randoming(int maxRand, int addition)
{
    int valuex = rand()%maxRand;
    valuex += addition;

    if(valuex > maxRand)
        valuex = maxRand-1;

    return valuex;
}

resourceSystem::resourceSystem()
{
    nodesOnMap = randoming(maxNodesOnMap, (maxNodesOnMap/2));

    nodesActive = nodesOnMap;

    for(int i = 0; i < nodesOnMap; i++)
    {
        RNs[i] = new resourceNode(randoming(worldCordsX, 0), randoming(worldCordsY, 0), randoming(maxNodeStrength, (maxNodeStrength/2)));
        RNs[i]->activateRN(true);
    }
}

void resourceSystem::update()
{

}

void resourceSystem::animates()
{
    for(int i = 0; i < nodesOnMap; i++)
    {
        if(RNs[i]->isActive())
        {
            RNs[i]->animate();
        }
    }
}

class NodeCreated {
public:
    NodeCreated() {activated = false; createdByFireChain = false; positionX = 0; positionY = 0; }
    void created(bool create) { createdByFireChain = create;}
    bool IsNodeCreated() {return createdByFireChain; }
    bool IsNodeActivated() { return activated; }
    void checkToActivateYet(float X, float Y);
    void setPositionX(float X) { positionX = X;}
    void setPositionY(float Y) { positionY = Y;}
protected:
    bool activated;
    float positionX;
    float positionY;
    bool createdByFireChain;
};

void NodeCreated::checkToActivateYet(float X, float Y)
{
    if(!activated)
    {
        if(X > (positionX-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && X < (positionX + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
            if(Y > (positionY-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && Y < (positionY + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
                activated = true;
    }
}

class fireChain : public globalFunctions { //Yes, not every function is used, but the radius one is, may not be neccesary but o well, vs 1.1's problem :P
public:
    fireChain();
    void animation(resourceSystem *RS);
    bool isActive() { return activated; }
    bool stillDrawing() { return drawingInProgress; }
    void activate(bool x) { activated = x; }
    void drawingActive(bool x) {drawingInProgress = x; }
    void givenParticle(particle *givenPart);
    void setPositions(float x, float y) { positionX = x, positionY = y;}
    void popPart(particle *fireElementalParticle, int currentParticle);
    bool makeDecisionToPop();
    bool makeDecisionToPush();
    void pathCompleted() { pathDone = true; }
    void checkCollisionWithResourceNode(resourceSystem *RS);

    bool delayParticleAdditionOrRemoval();
protected:
    particle *fireChainParts[MaxParticlesInFireChain];
    NodeCreated NC; //A firechain can only create one node.
    int currentFireChainPart;
    int totalFireChainAccumulated;
    bool activated;
    float positionX;
    float positionY;
    bool drawingInProgress;
    float lastCoordinateX;
    float lastCoordinateY;
    bool pathDone;

    float timeElapsed;
    float lastTimePosition;
    bool postponeReturn;
    float howManyParticlesReleasedPerSecond;

    bool collisionWithResourceNode;
    int whichResourceNodeCollided;

};

void fireChain::checkCollisionWithResourceNode(resourceSystem *RS)
{
    for(int i = 0; i < RS->howManyRNodesOnMap(); i++)
    {
        if((lastCoordinateX > RS->getNodeXValue(i)-fireChainAttachOnResourceNodeAdditionLeverage) && (lastCoordinateX < (RS->getNodeXValue(i) + TS+fireChainAttachOnResourceNodeAdditionLeverage)))
            if((lastCoordinateY > RS->getNodeYValue(i)-fireChainAttachOnResourceNodeAdditionLeverage) && (lastCoordinateY < (RS->getNodeYValue(i) + TS+fireChainAttachOnResourceNodeAdditionLeverage)))
            {
                RS->setNodeAsLinked(i);
                collisionWithResourceNode = true;
                whichResourceNodeCollided = i;
            }
    }

    NC.created(true);
    NC.setPositionX(lastCoordinateX);
    NC.setPositionY(lastCoordinateY);
}

bool fireChain::delayParticleAdditionOrRemoval()
{
    timeElapsed = SDL_GetTicks();
    bool postponeReturn = false;

    if((timeElapsed-lastTimePosition) > howManyParticlesReleasedPerSecond){
        postponeReturn = true;
        lastTimePosition = SDL_GetTicks();
    }

    return postponeReturn;
}

void fireChain::givenParticle(particle *givenPart) //Or pushPart as name.
{
    fireChainParts[currentFireChainPart] = new particle;
    givenPart->movePartTo(fireChainParts[currentFireChainPart], &totalFireChainAccumulated);

    currentFireChainPart++;
    totalFireChainAccumulated++;
}

fireChain::fireChain()
{
    activated = false;
    drawingInProgress = false;
    currentFireChainPart = 0;
    totalFireChainAccumulated = 0; //Might be the same as currentFireChainPart, but we'll address that in vs 1.1.
    positionX = 0;
    positionY = 0;
    lastCoordinateX = 0;
    lastCoordinateY = 0;
    pathDone = false;

    collisionWithResourceNode = false;
    whichResourceNodeCollided = -1; //SO no confusing can occur, this is used as NOT TRUE variable value.

    timeElapsed = 0;
    lastTimePosition = 0;
    howManyParticlesReleasedPerSecond = 1000/FireChainsParticleFeedRatePerSecondFromFireElemental;
}

void fireChain::popPart(particle *fireElementalParticle, int currentParticle)
{
    //Checking if there is actually anything to push is probably also something that needs to be built in...
    ///I will be completely honoust in saying I have NO idea why -1 needs to be added, it simply makes no sence, but it works. In vs 1.1 this will be studied.
    fireChainParts[currentFireChainPart-1]->movePartTo(fireElementalParticle, &currentParticle);
    fireChainParts[currentFireChainPart-1]->activate(false);
    currentFireChainPart--;
    totalFireChainAccumulated--;
}

bool fireChain::makeDecisionToPush()
{
    int mxTemp; //Guess this could have been compressed into a function since it's being used frequently. This will be updated in vs 1.1.
    int myTemp;

    float mx = mxTemp;
    float my = myTemp;

    if(!pathDone)
    {
        SDL_GetMouseState(&mxTemp, &myTemp);

        mx = mxTemp;
        my = myTemp;

        mx = scrollControl.adjustInputValueX(mx);
        my = scrollControl.adjustInputValueY(my);

        lastCoordinateX = mx;
        lastCoordinateY = my;
    }
    else
    {
        mx = lastCoordinateX;
        my = lastCoordinateY;
    }

     //VA's code. coordinate based algorithm > straight line algorithm.
    vec2f direction;
    float distance;

    direction.x = mx - positionX;
    direction.y = my - positionY;

    distance = getRadius((positionX-mx), (positionY-my));

    //normalise the direction vector, i.e., make sure it has length of 1
    direction.x /= distance;
    direction.y /= distance;

    //Well then... nothing to see here...
    float chainDistance = getRadius((positionX+(totalFireChainAccumulated*movementRateOfFireChainParts*direction.x)-positionX), (positionY+(totalFireChainAccumulated*movementRateOfFireChainParts*direction.y-positionY)));
    float mouseDistance = getRadius((positionX-mx), (positionY-my));

   // cout << "\n\nchainDistance: " << chainDistance;
   // cout << "\nmouseDistance: " << mouseDistance;
   // cout << "\nTotalFireChainAccumulated: " << totalFireChainAccumulated;
   // Sleep(100);

    if(chainDistance < (mouseDistance-cursorLeverage))
        return true;

    return false;
}

bool fireChain::makeDecisionToPop() //Yes better design will come... Very tired and want to finish this now.
{
    int mxTemp; //Guess this could have been compressed into a function since it's being used frequently. This will be updated in vs 1.1.
    int myTemp;

    float mx = mxTemp;
    float my = myTemp;

    if(!pathDone)
    {
        SDL_GetMouseState(&mxTemp, &myTemp);

        mx = mxTemp;
        my = myTemp;

        mx = scrollControl.adjustInputValueX(mx);
        my = scrollControl.adjustInputValueY(my);

        lastCoordinateX = mx;
        lastCoordinateY = my;
    }
    else
    {
        mx = lastCoordinateX;
        my = lastCoordinateY;
    }

     //VA's code. coordinate based algorithm > straight line algorithm.
    vec2f direction;
    float distance;

    direction.x = mx - positionX;
    direction.y = my - positionY;

    distance = getRadius((positionX-mx), (positionY-my));

    //normalise the direction vector, i.e., make sure it has length of 1
    direction.x /= distance;
    direction.y /= distance;

    float chainDistance;

    //Well then... nothing to see here...
    if(totalFireChainAccumulated > 0)
        chainDistance = getRadius((positionX+((totalFireChainAccumulated-1)*movementRateOfFireChainParts*direction.x)-positionX), (positionY+((totalFireChainAccumulated-1)*movementRateOfFireChainParts*direction.y-positionY)));

    else
        chainDistance = getRadius((positionX+((totalFireChainAccumulated)*movementRateOfFireChainParts*direction.x)-positionX), (positionY+((totalFireChainAccumulated)*movementRateOfFireChainParts*direction.y-positionY)));

    float mouseDistance = getRadius((positionX-mx), (positionY-my));

    //cout << "\n\nchainDistance: " << chainDistance;
    //cout << "\nmouseDistance: " << mouseDistance;
    //cout << "\nTotalFireChainAccumulated: " << totalFireChainAccumulated;
    //Sleep(100);

    if(chainDistance > (mouseDistance+cursorLeverage))
        return true;

    return false;
}

void fireChain::animation(resourceSystem *RS)
{
    int mxTemp; //Guess this could have been compressed into a function since it's being used frequently. This will be updated in vs 1.1.
    int myTemp;

    float mx;
    float my;

    if(!collisionWithResourceNode)
        if(NC.IsNodeCreated() && !NC.IsNodeActivated())
                NC.checkToActivateYet(fireChainParts[currentFireChainPart-1]->returnX(), fireChainParts[currentFireChainPart-1]->returnY());

    if(whichResourceNodeCollided != -1)
        if(!RS->IsNodeConnected(whichResourceNodeCollided) && RS->IsNodeLinked(whichResourceNodeCollided)) //Double protection.
            RS->checkToConnectYet(whichResourceNodeCollided, fireChainParts[currentFireChainPart-1]->returnX(), fireChainParts[currentFireChainPart-1]->returnY());

    //This can maybe be avoided if we have float parameter arguments, but too tired to think now, but I'm going to keep it like this for now.
    if(!pathDone)
    {
        SDL_GetMouseState(&mxTemp, &myTemp);

        mx = mxTemp;
        my = myTemp;

        mx = scrollControl.adjustInputValueX(mx);
        my = scrollControl.adjustInputValueY(my);

        lastCoordinateX = mx;
        lastCoordinateY = my;
    }
    else
    {
        mx = lastCoordinateX;
        my = lastCoordinateY;
    }

    vec2f direction;
    float distance;

    direction.x = mx - positionX;
    direction.y = my - positionY;

    distance = getRadius((positionX-mx), (positionY-my));

    //normalise the direction vector, i.e., make sure it has length of 1
    direction.x /= distance;
    direction.y /= distance;

    //Well then... nothing to see here...
    float chainDistance = getRadius((positionX+(totalFireChainAccumulated*movementRateOfFireChainParts*direction.x)-positionX), (positionY+(totalFireChainAccumulated*movementRateOfFireChainParts*direction.y-positionY)));
    float mouseDistance = getRadius((positionX-mx), (positionY-my));

    if(pathDone && chainDistance >= mouseDistance-cursorLeverage && chainDistance <= mouseDistance+cursorLeverage)
        howManyParticlesReleasedPerSecond = fireChainSettleSpeedDivided;

    //Showing how the variables were found... simply a straight line...
    //positionY = k*positionX + r;
    //my = k*mx + r;

    //r = positionY - k*positionX;
    //r = my - k*mx

    //positionY - k*positionX = my - k*mx
    //-k*positionX + k*mx = my - positionY
    //k*mx - k*positionX = my - positionY
    //k(mx - positionX) = my - positionY

    //update position along the direction vector
    //elm_pos.x = elm_pos.x + direction.x*speed;
    //elm_pos.y = elm_pos.y + direction.y*speed;

    for(int i = 0; i < totalFireChainAccumulated; i++)
    {
        if(fireChainParts[i]->isActive() && scrollControl.inScreen(positionX+(i*movementRateOfFireChainParts*direction.x), positionY+(i*movementRateOfFireChainParts*direction.y), TS))
        {
           // cout << "\nHey [" << i << "]: is Moving!";
           // Sleep(200);
            fireChainParts[i]->position(positionX+(i*movementRateOfFireChainParts*direction.x), positionY+(i*movementRateOfFireChainParts*direction.y)); //Derived from straight line formula.
            fireChainParts[i]->move();
        }
    }

}

class fireElemental: public globalFunctions{
public:
    fireElemental();
    int collisionDetection(rogueParts *RPs);
    void absorbParts(rogueParts *RPs);
    void releaseParts(int whichPartToRelease, rogueParts *RPs);
    void animation(rogueParts *RPs, resourceSystem *RS);
    void moveX(bool activateMoveX, int multiplier);
    void moveY(bool activateMoveY, int multiplier);
    float getMovementSpeed(){ return movementSpeed;}
    void updates();

    void fireChainConstruction(float mx, float my);
    void fireChainFinished(resourceSystem *RS);
    void fireChainAnimation(resourceSystem *RS); //The reason it is here is to prevent class cross interaction. Yes, we will look into design options later, but currently I'm unsure about it.

protected:
    fireChain *FCs[maxFireChains]; //Anyway too much to actually allocate. Needs to be fixed with a linked list later in vs 1.1.
    particle *fireParts[totalParticlesPerClass];
    particle referencePart;
    float sizeIncreasePerPart;
    float x;
    float y;
    float movementSpeed;
    bool ACX;
    bool ACY;
    int multiplierX;
    int multiplierY;
    void updatePositions();
    void handleEndMapCollision();

    bool fireChainsInitiated;
    int currentFireChain;
    int fireChainsActivated;

    bool collisionOnFireElemental(float mx, float my);

    void lifeIntensity();
    float lifeValue;
};

void fireElemental::lifeIntensity()
{
    lifeValue = (float)1/(partCount/lifeDiminishingFactor);

    for(int i = 0; i < partCount; i++)
    {
        if(fireParts[i]->isActive())
        {
            fireParts[i]->setLife(lifeValue);
            fireParts[i]->setSize(fireElemSize);
        }
    }
}

void fireElemental::fireChainAnimation(resourceSystem *RS)
{
    for(int i = 0; i < fireChainsActivated; i++)
    {
        if(FCs[i]->isActive())
        {
            FCs[i]->setPositions(x,y);

            if(FCs[i]->stillDrawing())
            {
                if(FCs[i]->delayParticleAdditionOrRemoval()) //Took 1000years to see this bug, but it needs to be in FCs so that each chain has it's own delay, sigh.
                {
                    if(FCs[i]->makeDecisionToPush() && currentPart > 0){
                        FCs[i]->givenParticle(fireParts[currentPart]);
                        fireParts[currentPart]->activate(false);
                       // cout << "\n\npartCount: " << partCount;
                       // cout << "\ncurrentPart: " << currentPart;
                       // Sleep(100);
                        partCount--;
                        currentPart--; //Because we specificly choose the last entry particle to go.
                    }

                    else if(FCs[i]->makeDecisionToPop()){
                        FCs[i]->popPart(fireParts[currentPart], currentPart);
                        partCount++;
                        currentPart++;
                    }
                    //Obviously if non of these functions are called, nothing happens...
                }

            }
            FCs[i]->animation(RS);
        }
    }
}

bool fireElemental::collisionOnFireElemental(float mx, float my)
{
    mx = scrollControl.adjustInputValueX(mx);
    my = scrollControl.adjustInputValueY(my);

    float distFromCursorToFireElemental = getRadius((mx - x), (my - y)); //() Important as it is passed as is, afaik.

    if(distFromCursorToFireElemental < (referencePart.returnSize() + (partCount*sizeIncreasePerPart)))
        return true;

    return false;
}

void fireElemental::fireChainConstruction(float mx, float my)
{
    if(!fireChainsInitiated && collisionOnFireElemental(mx,my)) //This could maybe be moved inward to avoid checking this if the first var is false.
    {
        fireChainsInitiated = true;
        FCs[currentFireChain] = new fireChain;
        FCs[currentFireChain]->activate(true);
        FCs[currentFireChain]->drawingActive(true);
        FCs[currentFireChain]->setPositions(x,y);
        fireChainsActivated++;
    }

}

void fireElemental::fireChainFinished(resourceSystem *RS)
{
    if(fireChainsInitiated){ //Not so obvious... if this is not checked, mouse_up crashes the program searching for a void function.
    fireChainsInitiated = false;
    FCs[currentFireChain]->pathCompleted();
    FCs[currentFireChain]->checkCollisionWithResourceNode(RS);

    currentFireChain++;
    if(currentFireChain > maxFireChains)
        currentFireChain = 0;
    }
}

//Wanted to do some sort of inhertitance with this function, but since my design is not perfect yet, it was difficult. It's also 1:00 in the morning...
void fireElemental::handleEndMapCollision()
{
     if(x <= 0)
        x = 0;

    if(x >= worldCordsX)
        x = worldCordsX;

    if(y <= 0)
        y = 0;

    if(y >= worldCordsY)
        y = worldCordsY;
}

void fireElemental::updates()
{
    updatePositions();
}

void fireElemental::updatePositions()
{
    if(ACX)
        x += movementSpeed*multiplierX;

    if(ACY)
        y += movementSpeed*multiplierY;

    handleEndMapCollision();
}

void fireElemental::moveX(bool activateMoveX, int multiplier = 1)
{
    ACX = activateMoveX;
    multiplierX = multiplier;
}

void fireElemental::moveY(bool activateMoveY, int multiplier = 1)
{
    ACY = activateMoveY;
    multiplierY = multiplier;
}

void fireElemental::animation(rogueParts *RPs, resourceSystem *RS)
{
    if(scrollControl.inScreen(x,y,(referencePart.returnSize() + (partCount*sizeIncreasePerPart))))
    {
        //lifeIntensity();

        for(int i = 0; i < (int)(partCount/displayReduction); i++)
        {
            if(fireParts[i]->isActive()){
            fireParts[i]->position(x,y);
            fireParts[i]->move();
          //  fireParts[i]->setColor(0.5 + randomize(0.5), 0.5 + randomize(0.5), 0.5 +randomize(0.5));
          //  fireParts[i]->setLife(1);
            }

        absorbParts(RPs);
        }

    }

    fireChainAnimation(RS); //Has it's own inscreen controll.
}

fireElemental::fireElemental()
{
    x = WorldStartingElementalPositionX + fireElementalStartingPositionX;
    y = WorldStartingElementalPositionY + fireElementalStartingPositionY;

    movementSpeed = StartingMoveSpeedOFFireElemental;
    sizeIncreasePerPart = sizeIncreasedPerPart;

    ACX = false;
    ACY = false;

    multiplierX = 0;
    multiplierY = 0;

    fireChainsActivated = 0;
    currentFireChain = 0;

    fireChainsInitiated = false;

    for(int i = 0; i <= initialStrength; i++)
    {
        currentPart = i;
        partCount = i;

        fireParts[i] = new particle;
        fireParts[i]->activate(true);
        fireParts[i]->position(x,y);
    }

    lifeIntensity();
}

int fireElemental::collisionDetection(rogueParts *RPs)
{
    float radiusRP = (referencePart.returnSize()/2);
    float radiusFE = (referencePart.returnSize()/2) * (partCount*sizeIncreasePerPart);
    float distanceRadius;

    for(int i = 0; i < RPs->getPC(); i++)
    {
        if(RPs->getPart(i)->isActive()){
        distanceRadius = getRadius(RPs->getPart(i)->returnX() - x, RPs->getPart(i)->returnY() - y);

        if(distanceRadius < (radiusFE + radiusRP))
        return i;
        }
    }

    return -1;
}

void fireElemental::absorbParts(rogueParts *RPs)
{
    int whichPartColide;

    whichPartColide = collisionDetection(RPs);

    if(whichPartColide == -1) return;

    currentPart++;
    partCount++;

    fireParts[currentPart] = new particle;

    RPs->getPart(whichPartColide)->movePartTo(fireParts[currentPart], &partCount);

    //The reason for below is because it is difficult to test if a part is not NULL if random parts are deleted, rather set their activation to 0.
    //The amount of memory allocated isn't that heavy, but this isn't ideal. I will fix this later if I find a more efficient way.
    RPs->getPart(whichPartColide)->activate(false);
    //RPs->reducePartCount();

    if(partCount > totalParticlesPerClass)
    partCount = 0;

    lifeIntensity();
}
//Not used.
void fireElemental::releaseParts(int whichPartToRelease, rogueParts *RPs)
{
     //A bit of hacking, since RPs->getPart(RPs->returnCurrentPart()) = new particle; doesn't work.
    RPs->allocateNewPart();

    fireParts[whichPartToRelease]->movePartTo(RPs->getPart(whichPartToRelease), &partCount);
    fireParts[whichPartToRelease]->activate(false);

    if(RPs->returnPartCount() > totalParticlesPerClass)
    RPs->setPartCount(0);

    RPs->increasePartCount();
    RPs->increaseCurrentPartNum();

    currentPart--;
    partCount--;

    lifeIntensity();
}

void handleKeyPress( SDL_keysym *keysym, fireElemental *FE )
{
    switch ( keysym->sym ){
    case SDLK_w: case SDLK_UP:
            FE->moveY(true, -1);
        break;
    case SDLK_s: case SDLK_DOWN:
            FE->moveY(true, 1);
        break;
    case SDLK_a: case SDLK_LEFT:
            FE->moveX(true, -1);
        break;
    case SDLK_d: case SDLK_RIGHT:
            FE->moveX(true, 1);
        break;
	default:
	    break;
	}

    switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
	    /* ESC key was pressed */
	    Quit( 0 );
	    break;
	case SDLK_F1:
	    /* F1 key was pressed
	     * this toggles fullscreen mode
	     */
	    SDL_WM_ToggleFullScreen( surface );
	    break;
    case SDLK_w :
        break;
	default:
	    break;
	}

    return;
}

void KeyUpEvents( SDL_keysym *keysym, fireElemental *FE) //Will be defined for the user chosen faction later, but this needs to work now.
{

    switch( keysym->sym ){
            case SDLK_LEFT: case SDLK_a:
                    FE->moveX(false);
                break;
            case SDLK_RIGHT: case SDLK_d:
                    FE->moveX(false);
                break;
            case SDLK_UP: case SDLK_w:
                    FE->moveY(false);
                break;
            case SDLK_DOWN: case SDLK_s:
                    FE->moveY(false);
                break;
            default:
            break;
    }

}

void mouseEvents(SDL_Event *eventx, fireElemental *FE)
{
    int mxTemp;
    int myTemp;

    SDL_GetMouseState(&mxTemp, &myTemp);

    float mx = mxTemp;
    float my = myTemp;

    if(eventx->button.button == SDL_BUTTON_LEFT){
        FE->fireChainConstruction(mx,my);
    }

}

void mouseUp(SDL_Event *eventx, fireElemental *FE, resourceSystem *RS)
{
    FE->fireChainFinished(RS);
}

//Basic version. Needs to be able to predicts frames so that the counter starts at 0 and not at 1 second (For Frame per second display).
class limitFrameRates {
public:
    limitFrameRates();
    void limitingFrameRates();
    void displayRealFramesPerSecond();
    int getRealFramesPerSecond() { return currentFrameRate; }
protected:
    float timeTakenToIterate;
    float timePositionReference;
    float frameRate;
    float timeTakenPerFrame;
    int currentFrameRate;
    float framesPerSecond;
    int framesRunningPerSecond;

    void calculationRealFramesPerSecond();
};

limitFrameRates::limitFrameRates()
{
    framesPerSecond = FRAMES_PER_SECOND;

    frameRate = 1000/framesPerSecond;
    timeTakenToIterate = SDL_GetTicks();

    timePositionReference = 0;
    framesRunningPerSecond = 0;

    timeTakenPerFrame = 0;
    currentFrameRate = 0;
}

void limitFrameRates::displayRealFramesPerSecond()
{
    //Obviously only the vs 1.0 state. Pointless adding another check for only one seond. (if == 0, don't display)
    cout << "\nFrames Per Second: " << currentFrameRate;
}

void limitFrameRates::limitingFrameRates()
{
    timeTakenToIterate = SDL_GetTicks();

    if((timeTakenToIterate-timePositionReference) < frameRate)
        Sleep(frameRate - (timeTakenToIterate-timePositionReference)); ///WINDOWS FUNCTIONS.

    calculationRealFramesPerSecond();

    timePositionReference = SDL_GetTicks();
}

void limitFrameRates::calculationRealFramesPerSecond()
{
    ///The only confusion might be that timeTakenPerFrame is indeed just that, but it is concatenated to form a total time up to one second, to see how long it takes to make 1 second worth frms.
    if((timeTakenToIterate-timePositionReference) < frameRate)
        timeTakenPerFrame += frameRate;

    else
        timeTakenPerFrame += (timeTakenToIterate-timePositionReference);

    framesRunningPerSecond++;

    if(timeTakenPerFrame >= 1000)
    {
        currentFrameRate = framesRunningPerSecond;
        framesRunningPerSecond = 0;
        timeTakenPerFrame = 0;
    }
}

class delaySome {
public:
    delaySome();
    void setDelay(float deL) { delayTime = deL*1000; }
    void startClock() { referenceTime = SDL_GetTicks(); }
    void startEvent(bool SE = true) { eventStarted = SE; }
    bool isEventStarted() { return eventStarted; }
    bool isDelayOver();
private:
    float referenceTime;
    float delayTime;
    bool eventStarted;
};

delaySome::delaySome()
{
    referenceTime = 0;
    delayTime = 0;
    eventStarted = false;
}

bool delaySome::isDelayOver()
{
    float currentTime = SDL_GetTicks();

    if((currentTime - referenceTime) > delayTime)
        return true;

    return false;
}

class animateCursor{
public:
    animateCursor();
    void cursorMove();
protected:
    particle *cursorParts[amountOfCursorParts];
};

animateCursor::animateCursor()
{
    for( int i = 0; i < amountOfCursorParts; i++)
    {
        cursorParts[i] = new particle;
        cursorParts[i]->activate(true);
        cursorParts[i]->setSize(cursorSize);
    }
}

void animateCursor::cursorMove()
{
    int Tmx;
    int Tmy;

    SDL_GetMouseState(&Tmx,&Tmy);

    float mx = Tmx;
    float my = Tmy;

    mx = scrollControl.adjustInputValueX(mx);
    my = scrollControl.adjustInputValueY(my);

    for( int i = 0; i < amountOfCursorParts; i++)
    {
        cursorParts[i]->position(mx,my);
        cursorParts[i]->move();
    }
}

class gameController {
public:
    //gameController();
    void controlGame(resourceSystem *rs, fireElemental *fe, rogueParts *rp, animateCursor ac);
private:
};
//Controls most of the game.
void gameController::controlGame(resourceSystem *rs, fireElemental *fe, rogueParts *rp, animateCursor ac)
{
    ac.cursorMove();

    fe->updates();

    fe->animation(rp, rs);

    rp->animation();
    rs->animates();
}

int main (int argc, char** argv)
{
    srand(time(NULL));
    limitFrameRates LFR;

    animateCursor AC;
    resourceSystem RS;

    gameController GC;
/*
    delaySome DS;
    DS.setDelay(73);
    DS.startClock();
    bool notAgain = false;

    notificationSystem PW(true, 3, 5);
    PW.setPosition(100, 100);
    PW.addLine("Pre-Alpha demonstration");
    PW.addLine("Developer build vs. 1.0");

    notificationSystem NS(true, 11, 21);

    NS.setPosition(200, 200);
    NS.addLine("DRAG A FIRE CHAIN");
    NS.addLine("OUT OF YOUR ELEMENTAL!");
    NS.addLine("AND LET THE WORLD SEE");
    NS.addLine("THE MIGHT OF FIRE!");
    NS.addLine("ABSORB FIRE AND GROW");
    NS.addLine("GROW BEYOND THEIR CONTROL!");

    notificationSystem NS1(true, 38, 21);

    NS1.setPosition(150, 400);
    NS1.addLine("DO NOT FEAR THE POWER ELEMENTALS");
    NS1.addLine("DO NOT HESITATE TO CONSUME THEM!");
    NS1.addLine("THEY HAVE TAINTED THE WOLRD FAR TOO LONG");
    NS1.addLine("IT IS TIME FOR THEIR REIGN");
    NS1.addLine("OF CORRUPTION TO END!");

    notificationSystem NS2(true, 68, 11);
    NS2.setPosition(150, 200);
    NS2.addLine("VOLCANIC ERUPTION DETECTED!");
    NS2.addLine("FEEL THE MIGHT OF FIRE!");
    NS2.addLine("EMBRACE OUR CHAOTIC NATURE!");
    NS2.addLine("MARVEL AT ITS MAGNIFICENCE!");
*/
    initCoutRedirecting();

    fireElemental *FEx;
    rogueParts *RPx;

    FEx = new fireElemental;
    RPx = new rogueParts;

    float tempConvertX = 0;
    float tempConvertY = 0;
    float fadeConvert = 0;
    float lifeConvert = 0;

    float redConvert = 0;
    float colorConvert = 0;

    int videoFlags;
    /* main loop variable */
    int done = false;
    /* used to collect events */
    SDL_Event event;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;
    /* whether or not the window is active */
    int isActive = true;
/*
    particle fireEffect[150];

    for(int i = 0; i < 150; i++){
        fireEffect[i].activate(true);
        fireEffect[i].setTexture(WHICH_TEXTURE_TO_TEST);
        fireEffect[i].setSize(TS+60);

        tempConvertX = rand() % 40 - 15;
        tempConvertY = rand() % 100 - 100;
        tempConvertX /=20;
        tempConvertY /=20;

        fireEffect[i].setXspeed(tempConvertX); // division by 0 possible.
        fireEffect[i].setYspeed(tempConvertY); //

        lifeConvert = (rand()%100+1);
        lifeConvert /= 100;

        fadeConvert = (rand()%100+1);
        fadeConvert /= 8000;
        fadeConvert +=  + 0.001;

        redConvert = 0;
        colorConvert = 0;

        fireEffect[i].setLife(lifeConvert);
        fireEffect[i].setFade(fadeConvert);
        fireEffect[i].setColor(1, 1, 1); //when rLife = 0, the display is already over, no need for special case.
        fireEffect[i].position(420+(rand() % 20 - 10),700+(rand() % 20 -10));
    }
*/
    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
	    fprintf( stderr, "Video initialization failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
	{
	    fprintf( stderr, "Video query failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
   // videoFlags != SDL_FULLSCREEN;

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    surface = SDL_SetVideoMode( SW, SH, SBPP,
				videoFlags );

    /* Verify there is a surface */
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	    Quit( 1 );
	}

    /* initialize OpenGL */
    if ( initGL( ) == false )
	{
	    fprintf( stderr, "Could not initialize OpenGL.\n" );
	    Quit( 1 );
	}

    /* Resize the initial window */
    resizeWindow( SW, SH );
    SDL_WM_SetCaption("Elemental Reach", "Elemental Reach");
    /* wait for events */
    while ( !done )
	{
	     LFR.limitingFrameRates();
	    /* handle the events in the queue */

	    while ( SDL_PollEvent( &event ) )
		{
		    switch( event.type )
			{
			case SDL_ACTIVEEVENT:
			    /* Something's happend with our focus
			     * If we lost focus or we are iconified, we
			     * shouldn't draw the screen
			     */
			    if ( event.active.gain == 0 )
				isActive = false;
			    else
				isActive = true;
			    break;
			case SDL_VIDEORESIZE:
			    /* handle resize event */
			    surface = SDL_SetVideoMode( event.resize.w,
							event.resize.h,
							SBPP, videoFlags );
			    if ( !surface )
				{
				    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
				    Quit( 1 );
				}
			    resizeWindow( event.resize.w, event.resize.h );
			    break;
			case SDL_KEYDOWN:
			    /* handle key presses */
			    handleKeyPress( &event.key.keysym, FEx );
			    break;
            case SDL_KEYUP:
                KeyUpEvents( &event.key.keysym, FEx );
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseEvents(&event, FEx);
                break;
            case SDL_MOUSEBUTTONUP:
                mouseUp(&event, FEx, &RS);
                break;
			case SDL_QUIT:
			    /* handle quit requests */
			    done = true;
			    break;
			default:
			    break;
			}
		}
    /* Draw it to the screen */

	    /* draw the scene */

	    if(!done){

	        scrollControl.scrolling();

	        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glLoadIdentity( );
           //
           //Print(200, 200, 0, "(Text does not work with scrolling...)");
          //  if(DS.isDelayOver())
          //  {
          //      for(int i = 0; i < 150; i++)
          //      {
          //          fireEffect[i].move();
          //      }
          //  }

            GC.controlGame(&RS, FEx, RPx, AC);
/*
            PW.speak();
            NS.speak();
            NS1.speak();
            NS2.speak();

            scrollControl.shakeCameraInitiation(30, 20);

            //Print(50, 100, 1, "HOLD LEFT MOUSE BUTTON ON FIRE ELEMEMENTAL");
            //Print(50, 120, 1, "AND DRAG TO A CERTAIN DESTINATION");
            //Print(50, 140, 1, "LET GO OF THE LEFT MOUSE BUTTON :)");

            //LFR.displayRealFramesPerSecond();
            scrollControl.shakeCamera();

	        SDL_GL_SwapBuffers( );


	        ///ADVANCED VERSION OF DRAWING.
	        /*
	        SDL_GetMouseState(&mx, &my);
	        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glLoadIdentity( );
            glColor3f(1.0, 1.0, 1.0);
            glTranslatef(0.0, 0.0, -1.0); ///INCREDIBLY IMPORTANT LINE.

            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glDisable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);

            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0,0.0); glVertex3f(-1.0, -1.0, 0.0);
            glTexCoord2f(1.0,0.0); glVertex3f(+1.0, -1.0, 0.0);
            glTexCoord2f(1.0,1.0); glVertex3f(+1.0, +1.0, 0.0);
            glTexCoord2f(0.0,1.0); glVertex3f(-1.0, +1.0, 0.0);
            glEnd();

            glLoadIdentity( );
            glColor3f(1.0, 1.0, 1.0);
            glTranslatef(0.4, 0.0, -1.0);

            glBindTexture(GL_TEXTURE_2D, texture[6]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0,0.0); glVertex3f(-0.1, -1.0, 0.0);
            glTexCoord2f(1.0,0.0); glVertex3f(+1.8, -1.0, 0.0);
            glTexCoord2f(1.0,1.0); glVertex3f(+1.8, +1.0, 0.0);
            glTexCoord2f(0.0,1.0); glVertex3f(-0.1, +1.0, 0.0);
            glEnd();

            glLoadIdentity( );
            glColor3f(1.0, 1.0, 1.0);
            glTranslatef(0.0, 0.0, -1.0);

            MXFLOATCONVERSION = mx;

            glLoadIdentity( );
            glColor3f(1.0, 1.0, 1.0);
            glTranslatef(0.0, -3.4, -20.0);
            glRotatef((180-(mx/3.555))-(((320-MXFLOATCONVERSION)/320)*(55-(my/8.72727272))),0,0,1);*/ ///Mx From the left side of the screen, 0 - 640. My will be 0 - 480, from top. 0 - 180 Space 3.555 -> 640 / 180
/*
            glBlendFunc(GL_DST_COLOR,GL_ZERO);

            glBindTexture(GL_TEXTURE_2D, texture[1]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0,0.0); glVertex3f(-1.0, -1.0, 0.0);
            glTexCoord2f(1.0,0.0); glVertex3f(+1.0, -1.0, 0.0);
            glTexCoord2f(1.0,1.0); glVertex3f(+1.0, +1.0, 0.0);
            glTexCoord2f(0.0,1.0); glVertex3f(-1.0, +1.0, 0.0);
            glEnd();

            glBlendFunc(GL_ONE, GL_ONE);

            glBindTexture(GL_TEXTURE_2D, texture[3]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0,0.0); glVertex3f(-1.0, -1.0, 0.0);
            glTexCoord2f(1.0,0.0); glVertex3f(+1.0, -1.0, 0.0);
            glTexCoord2f(1.0,1.0); glVertex3f(+1.0, +1.0, 0.0);
            glTexCoord2f(0.0,1.0); glVertex3f(-1.0, +1.0, 0.0);
            glEnd();

            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

           // PlaceWall();

            //glLoadIdentity( );
            //glColor3f(1.0, 1.0, 1.0);
           // glTranslatef(0.0, -3.4, -10.0);
            glRotatef(-90,0,0,1);

            glBegin(GL_TRIANGLES);
            glVertex3f(0,0,0);
            glVertex3f(0,-1,0);
            glVertex3f(0.1,8,0);
            glEnd();

            glLoadIdentity();
            glTranslatef(0.4, 0.0, -1.0);

            glColor3f(0.0,0.0,0.0);
            glBegin(GL_QUADS);
            glVertex3f(-0.01, -1.0, 0.0);
            glVertex3f(+1.0, -1.0, 0.0);
            glVertex3f(+1.0, +1.0, 0.0);
            glVertex3f(-0.01, +1.0, 0.0);
            glEnd();

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH);

        SDL_GL_SwapBuffers( );*/
	}
		//drawGLScene( );
		//firstModel->draw(SDL_GetTicks());
	}

    /* clean ourselves up and exit */
    Quit( 0 );
    /* Should never get here */
    return( 0 );
}
/*
class classInteraction()
{
public:
protected:
    fireElemental FE;
    rogueParts RP;
}

classInteraction::classInteraction()
{

}

void classInteraction::objectCommunication()
{

}
*/
//When creating the fireChain module, I was watching IEM, so yea, this might not be the best design...

/*if(RPs->getPart(i) != NULL){
        radius = RPs->getRadius(RPs->getPart(i)->returnSize()/2); //Radius is probably always going to be the same, but this adds flexiblity later on.

        distanceRadius = getRadius(RPs->getPart(i)->returnX() - x, RPs->getPart(i)->returnY() - y);
        }
*/

/* Linked list system.
if(mx != previousMX && my != previousMY)
    {
        coordinate *xy = new coordinate;
        //Guess it's ok to reference the internal variables (Not create a class with protected functions for var manipulation), because it's so small.
        xy->x = mx;
        xy->y = my;

        breadcrumbs.push_back(xy);
    }
//Some more info

void remove_dead_particles()
{
  std::list<particle*>::iterator it, it_tmp;
  particle* pParticle
  for (it = particle_list.begin(); it != particle_list.end(); )
  {
    it_tmp = it;
    ++it;
    pParticle = *it_tmp;
    if ( pParticle->state == 0)
    {
      //This particle is dead, remove it from the list
      particle_list.erase( it_tmp );
    }
  }

  std::cout << "Particles in list: " << particle_list.size() << std::endl;
}
*/
