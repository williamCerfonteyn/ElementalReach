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

///CURRENT DEVELOPMENT NOTES:
///NEED TO FIX CINEMATIC VIEW WAYPOINTS -> CALCULATED DIRECTION NOT CORRECT. ADJUSTSCREEN VALUES PROBABLE CAUSE.
///NEED TO CREATE UI SYSTEM FOR NODES, ASWELL AS LINKING ECT. QUITE A LARGE PROJECT CHUNK.
///REFERENCE TIME == 0 AT DELAYSOME CAUSES FAULT. FIX. //POSSIBLY FIXED.
///DEBUG. ALL HAS GONE TO HELL IT WOULD SEEM.
///IT WOULD APPEAR THAT FIREBALL LAUNCH DEVIATION IS NOT COMPLETELY CORRECT. I BELIEVE ITS BECAUSE X AND Y SHARE THE SAME DEVIATION VALUE, CONSEQUENTLY FURTHER OBLIQUE DISTANCES SUFFER LESS.
///HORRIFIC DESIGN AT CINEMATIC VIEW MOVEX AND MOVEY. PLEASE FIX INTO A SINGLE FUNCTION.
///CINEMATICVIEW WAYPOINT DELAY DOES NOT WORK YET.

///NEED TO DESIGN AND IMPLEMENT CINEMATIC VIEW.
///NEED TO PROVIDE SUSTAINABILITY.
///NEED TO FIX ALL BUGS.
///NEED TO IMPLEMENT A MENU SYSTEM.
///NEED TO CHECK AND FIX INSCREEN.

///NOTES!:
///REFERENCETIME != 0 CONDIATION ADDED TO DELAYSOME.

///URGENT FIXES:
///NEED TO FIX PARTICLE->MOVETO'S INT VALUE AND UNDERSTAND WHY IT NEEDS TO BE THERE. CURRENTLY NODECREATE IN FIRECHAIN ANIMATION IS HACKING THIS VALUE. BEING HACKED IN CREATED NODE STUFF TOO!
///NEED TO FIX DELAYSOME SETEVENTLOOP SO THAT IT STILL DELAYS AND DOESN'T JUST LET EVERYTHING THROUGH. THIS WILL MESS AROUND WITH THE TIMINGS A LITTLE, BUT IS IS A URGENT FIX.
///NEED TO DISABLE ANY MOVEMENT/ANIMATION OF DIABLED PARTICLES SO THAT IT DOESN'T RAPE YOUR COMPUTER.

///Stuff that needs developing:
//Need to add inScreen checks to all drawing functions.
//Need to build general functions for stuff like collision and delays.
//Need to correct collision so that x > p-size/2 && x < p+size/2, so that it's mid collision and not side.
//Need to fix stuff that uses ints but "returns" floats. In short - fix warnings ect.
//Need to give option to go fullscreen like in many games with these flags: SDL_NOFRAME | SDL_FULLSCREEN and aN option bolean to see when it needs to be fullscreen.
//Need to fix fireElemental where all parts are defined and only that which are added needs to be defined. Refer to the FE constructor for more info.
//Need to rewrite the gameName font into a complete one, and then use openGL (Like Print) and build a font like that.
//Need to solve global variable GLOBAL_VISIBILITY_OF_GAME_CONTENTS used for fading the game (OPTIONAL)
//Need to rewrite all arrays into a linked system with the std library.
//Need to fix the flawed time and speed systems that is defined as 0.001 ect, because it isn't moddify friendly, something like 20 parts per seconds is better.
//Need to fix the declaration of many particles [ARRAY PROBLEM, VOIDED WITH LINKED LISTS] in fireChains ect. (NOTE: fireChain::collisionWithNode() uses this very feature, just take into cons.)
//Need to

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
#include <windows.h>
#include <vector>

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

void initCoutRedirecting() { ///CAUSE OF PROBLAMATIC EXIT. REMOVE WHEN DEBUGGING IS FINISHED.
//SDL_Init(/*whatever*/);
//AllocConsole();
freopen("CONOUT$", "wb", stdout);
freopen("CONOUT$", "wb", stderr);
freopen("CONIN$", "rb", stdin);
/*...*/
}

///I UNDERSTAND THE PRACTISE OF USING GLOBAL VARIABLES ARE VERY FAULTY, YET I FEEL THERE ARE A PLACE FOR THEM. ONCE I LEARN MORE DYNAMIC AND ADVANCED DESIGN, I WILL PROHIBIT MYSELF FROM
///THE TEMPTATION LEADING TO CERTAIN DEATH OF HIGHER LEVEL PROGRAMS.

float GLOBAL_VISIBILITY_OF_GAME_CONTENTS = 1; ///IT'S USED BECAUSE OF HOW EASY IT FITS INTO THE STRUCTURE, HOW SIMPLE IT IS TO USE
///AND HOW THERE CAN BE VIRTUALLY NO CONFLICTS BECAUSE OF THE EXTENSIVE NAME ASWELL AS THE LIMITED USE IN PARTICLES. ONE COULD HAVE USED MULTIPLE FUNCTION PARAMETERS AS IN RESOURCESYTEM *RS
///BUT THIS SEEMED MORE LOGICAL AND LESS PRONE TO FAILURE. IF THIS IS HIGHLY FAULTY, THIS WILL BE CHANGED.
///ONLY 2 CLSSES AND ONE FUNCTION (NON PARTICLE) USE THIS VALUE: PARTICLE AND FADE. AND PRINT.

/*
///I FEEL QUITE DISTRAUGHT USING THESE GLOBALS, YET I FEEL THAT A DIFFERNT APPROCH USING THE CURRENT GAME DESIGN WILL CREATE MORE PROBLEMS THAN SOLUTIONS.
///THIS WILL BE USED TO CHECK IF THERE KEYS ARE DOWN, SINCE A SINGLE EVENT CAN ONLY HOUSE ONE KEY HELD AT A TIME, THUS THIS CREATES A ABILITY TO "HOLD 2 DOWN".
///AS FAR AS I AM CONCERNED, THIS WILL BE MONITORED VERY CLOSELY SO THAT NO FAULT CAN OCCUR. IN THE REWRITE, WE WILL DISCARD THESE AND DO REAL HIGH TIER PROGRAMMING!
///THESE ARE USED IN SETTING CONTROL GROUPS FOR FIRE CREATED NODES.
bool CONTROL_DOWN = false;
bool SHIFT_DOWN = false;
*/ //Found a way to make it work without globals. The fade one is universal, where this is based on a class action, we should keep it seperate, it could later maybe pose a problem :).

//Apparently const int are better than #defines.
int SW = 850; ///VERY IMPORTANT, THEY ARE NOT CONSTANT, AS THE SCREEN IS RESIZABLE.
int SH = 560;

//#define REMOVE_INTRO_FOR_DEBUGGING

int beforeResizeSW = SW;
int beforeResizeSH = SH;

const int TextNum = 19; ///Number of textures used.

//Cursor info:
const int amountOfCursorParts = 5;
const int cursorSize = 10;

//To keep the ratios between x/y the same, I would recommend keeping this as a multiple value of SW/SH. You can however, change this as you see fit.
const int worldCordsX = SW*3;
const int worldCordsY = SH*3;
const int PositionAboveObjectsForInfoText = 46;
const int PositionLeftObjectsForInfoText = 35;

///Prining system (NotificationSystem)
const int MaxConvoLines = 100; //Max lines that can be pushed into one object of convo.
const int lineSpacing = 15; //The space between lines as they are printed.
const int maxDelayTime = 5000; //Milisecond delay for the lines to appear in world method drawing.

const float standardFontSize = 1; //1 is standard. 2 is double 1's size. Use floating values to adjust accordingly.

const int SBPP = 16;//32
///
///
const int WHICH_TEXTURE_TO_TEST = 0; ///TEST POSITION.
///
const int TS = 30; //Texture size;
const float allParticles_base_life = 0.9;
///
const int FRAMES_PER_SECOND = 100; ///WINDOWS FUNCTION USED WITHIN THIS CLASS.
///
const int scrollingLeverage = 5; //How far to the side does the cursor need to be for scrolling to happen.
const float scrollSpeed = 5;
///
const float displayReduction = 5; //Reduces amount of particles displayed to increase performance.
///
//FADE SYSTEM information:
 const float amountOfFadeDelay = 0.001;//Delays fade(Time) Seconds.
 const float rateOfFadeIn = 0.00125;//How fast it should fade (Value).
 const float delayTimeBetweenFade = 2;

//RESOURCES INFO:
const int maxNodeStrength = 360; //About 6 minutes mining from a node.
const int minimumNodeStrength = 180; //About 3 minutes mining from a node.
const int maxNodesOnMap = 20;
const int minimunAmountOfNodesOnMap = maxNodesOnMap/2;
const float ResourceParticleFlowRatePerSecond = 1;
const int fireChainAttachOnResourceNodeAdditionLeverage = 15;
const float movementRateOfResourceParts = 3;
const float ResourceCollisionMultiplier = 1;

//NODE CREATED INFO:
const int howManyParticlesShouldBePushedPerSecond = 20; //20 particles are pushed per second. Probably going to be 21, since the first part is pushed without a delay.
const int maxPowerOfNodeCreated = 300;
const float mouseNodePushingLeverage = 30;
const float selectedCircleRotationSpeed = 0.2;
const float selectedCircleOpaqueValue = 1; //was 0.4
//NODE/SPELL INFO:
const int maxAmountOfKillParticles = 300; //Should probably be equal to maxPowerOfNodeCreated
//CHARGED FIRE INFO:
const float chargedFireCastingDelay = 100; //Miliseconds. Rate at which particles are pushed. 100 means 10 every 1 seconds.
//FIREBALL INFO:
const float fireBallCastingDelay = 1000; //Miliseconds. Rate at which the fireballs are casted. 500 means 2 every 1 second.
const int fireBallPower = 10; //The amount of particles that are within a fireball.
const float rateOfKillParticlesMovement = 3; //Higher is faster.

const int intensityOfDeviationOfFireBall = 16; //How far off target it can be.
const float variableDistributionOfDeviation = 0.1; //I believe this is the deviation amoung the variables. Also how large the gap is. just leave this alone and modify above. Else find and solve.

const float killParticlesCollisionLeverage = 30;

//FIRE ELEMENTAL INFO:
const int totalParticlesPerClass = 2000;
const int fireElemSize = 50;

const int FireChainParticleSmoothMovementLevel = 10; //Higher is better.

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
const int maxFireChains = 200;

//FIRE CHAINS INFO:
const int movementRateOfFireChainParts = 20;
const int FireChainsParticleFeedRatePerSecondFromFireElemental = 18; //Would advice to be linked to firePart movement
const int cursorLeverage = 5;
const int NumberOfTransitResourceParticlesPerFireChain = 300; //Maximum number of particles that can be returned from resources per fireChain at a time;
const int NumberOfTransitPushedParticlesPerFireChain = 300; //Number of particles that can be pushed in a fireChain at one time.
const float AmountOfTimeToDelayFireChainRetractionFromResourceNode = 2; //Will retract ALL RESOURCE PARTICLES HAVE BEEN COLLECTED. Seconds.
const float ParticleDelayRetractionOfFireChain = 0.08; //Speed at which the fireChain retracts.
///Becomes FASTER the LOWER the number is.
const int fireChainSettleSpeedDivided = 10; //Speed where fireChain has settled (In a node ect) and when movement happens, how fast does it react to get more parts ect.

//EXPLOSION INFORMATION:
const float lightDissipationLevel = 0.06;
const int explosiveIntensity = 30;
const int explosiveSize = 40;
const float standardExplosiveIgnitionColorR = 1.0;
const float standardExplosiveIgnitionColorG = 0.1;
const float standardExplosiveIgnitionColorB = 0.1;

//FIRE TRAIL INFO:
const float standardFireTrailFade = 0.017;
const int fireTrailIntensity = 30;
const float sizeOfFireTrail = 40;
const float fireTrailStandardColorR = 1.0;
const float fireTrailStandardColorG = 0.1;
const float fireTrailStandardColorB = 0.1;
const float fireTrailOpaquecy = 1;

//BASIC PARTICLE INFORMATION
const float StandardHealth = 12; //A good standard for what most particles should have. Can be modified accordingly. Just a nice standard to be modded from.
const float StandardDamage = 5; //Basic damage particles should inflict. A nice standard to be modded from.
#define enableStandardDamage
//
const int intensityOfParticleSpeedSpread = 15;
const float ParticleSpeedVariableDistributionDeviation = 0.15;

//CINEMATIC VIEW INFO:
const float rateOfCinematicViewActivation = 1;
const float sizeOfCinematicViewScreenDecrease = 140;
const int pushedInBorderLine = TS*2;
const int widthOfBorderLine = 15;
const float rateOfCameraMove = 3.2;
const float standardCameraStandStillDelay = 1000; //Miliseconds. Each waypoint has its own version.
const float initiationDelayOfCameraMovement = 200; //Miliseconds. Used at the start of cinematicView.

//POWER ELEMENTAL INFO:
const int maxPowerParts = 100;
const int powerElemSize = 40;
const int maxKilledParts = 100;
const float powerElementalStartPositionX = 900;
const float powerElementalStartPositionY = 300;

//IntroSystem (Part of preGameSystems)
const int IntroParticleSaturation = 300; //Very similar to rogue parts on the map, just intro version.
const int maxSpeedIntroParticlesCanMove = 10; //How fast they will swirl about the map. :)

const float speedDistributionOfIntroParts = 0.05; //The lower, the less distributed the speed will be.
const int maxSpeedOfFadeInGameNameParts = 20;
const float speedDistributionOfFadingInGameNameParts = 0.05;

const float rateOfVisiblityIntroPlay = 0.0005; //The higher the number, the quicker it plays.
const float StartingVisiblityOfIntro = 0;
const int gameNameLength = 14; //ELEMENTAL REACH. (14) Used from printing name in the beginning.
const int LetterDistanceAtGameNamePrint = 50;
const int speedOfFadeInForGameNameIntro = 10000;
const int ambientEffectAmount = 1; //The amount of particles that should give a background ambient fireEffect to the gameName;

using namespace std;

enum fireElementalSpellType {
    chargedFire,
    fireball
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

//initCoutRedirecting();

class delaySome {
public:
    delaySome(bool isThisAnEventDelay);
    void setDelay(float deL) { delayTime = deL*1000;}
    void setDelayMili(float deL) { delayTime = deL; }
    void startClock() { if(referenceTime == 0) referenceTime = SDL_GetTicks()+1; } //An inherit check.
    void stopClock() { referenceTime = 0; }
    bool isDelayOver();
    float getReferenceTime() { return referenceTime; }
    void startEvent(bool SE = true) { eventStarted = SE; }
    void restartEvent() { eventStarted = false; }
    bool isEventStarted() { return eventStarted; }
    void setLoopEvent(bool setLoop = true) { if(thisIsAnEvent) loopEvent = setLoop; } //NB CHECK HERE.
    void setDelayAsEventBased(bool eventYes = true) { thisIsAnEvent = eventYes; }
    bool clockNotStartedYet() { if(referenceTime == 0) return true; else return false; }
private:
    float referenceTime;
    float delayTime;
    float currentTime;
    bool thisIsAnEvent;
    bool eventStarted;
    bool loopEvent;
};

delaySome::delaySome(bool isThisAnEventDelay = false)
{
    referenceTime = 0;
    delayTime = 0;
    currentTime = 0;
    thisIsAnEvent = isThisAnEventDelay;
    eventStarted = false;
    loopEvent = false;
}

bool delaySome::isDelayOver()
{
    if(!eventStarted && referenceTime != 0) //!!/REFERENCETIME CONDITION ADDED.
    {
        currentTime = SDL_GetTicks();

        if((currentTime - referenceTime) > delayTime)
        {
            referenceTime = SDL_GetTicks();

            if(thisIsAnEvent)
                eventStarted = true;

            if(eventStarted)
                return !loopEvent;

            else return true; //Basicly just invert every return if the event should loop.
        }
    }
    if(eventStarted && referenceTime != 0)
        return loopEvent;

    else return false;
}
//This fade class is built for the current particle engine, thats why it adds to the colour3f alpha value, instead of setting it self.
class fade {
public:
    fade();
    void in();
    void out();
    bool fadedIn() { return isFadedIn; } //Simply two functions for ease of use.
    bool fadedOut() { return isFadedOut; }
protected:
    delaySome fadeDelay;
    delaySome betweenFadeDelay;
    bool isFadedIn;
    bool isFadedOut;
};

fade::fade()
{
    fadeDelay.setDelay(amountOfFadeDelay);
    betweenFadeDelay.setDelay(delayTimeBetweenFade);
    betweenFadeDelay.setDelayAsEventBased();
    fadeDelay.startClock(); //Doesn't matter that it starts now, it will just be 1 step ahead of where it would have been.

    isFadedIn = true;
    isFadedOut = false;
}
//The fact that there are two functions are just to make it easier and more logical to use.
void fade::in()
{
    betweenFadeDelay.startClock(); //Inherit protection will protect is from restarting the whole time.. :)

    if(betweenFadeDelay.isDelayOver())
    {
        betweenFadeDelay.setLoopEvent();

        if(fadeDelay.isDelayOver() && GLOBAL_VISIBILITY_OF_GAME_CONTENTS < 1+rateOfFadeIn)
            GLOBAL_VISIBILITY_OF_GAME_CONTENTS += rateOfFadeIn;

        else if(GLOBAL_VISIBILITY_OF_GAME_CONTENTS >= 1+rateOfFadeIn)
        {
            isFadedIn = true;
            isFadedOut = false;
            betweenFadeDelay.restartEvent();
            betweenFadeDelay.setLoopEvent(false);
            betweenFadeDelay.stopClock();
            betweenFadeDelay.startClock();
        }
    }
}

void fade::out()
{
    betweenFadeDelay.startClock();

    if(betweenFadeDelay.isDelayOver())
    {
        betweenFadeDelay.setLoopEvent();

        if(fadeDelay.isDelayOver() && GLOBAL_VISIBILITY_OF_GAME_CONTENTS > 0-rateOfFadeIn)
            GLOBAL_VISIBILITY_OF_GAME_CONTENTS -= rateOfFadeIn;

        else if(GLOBAL_VISIBILITY_OF_GAME_CONTENTS <= 0-rateOfFadeIn)
        {
            isFadedOut = true;
            isFadedIn = false;
            betweenFadeDelay.restartEvent();
            betweenFadeDelay.setLoopEvent(false);
            betweenFadeDelay.stopClock();
            betweenFadeDelay.startClock();
        }
    }
}

class cinematicWaypoint{
public:
    cinematicWaypoint();
    void setGoTo(float desigX, float desigY);
    bool isWaypointOver() { return doneWithThisWaypoint; }

    float giveDesignationX() { return designationX; }
    float giveDesignationY() { return designationY; }

    void setCalculateDirectionX(float SCDX) { calculateDirectionX = SCDX; }
    void setCalculateDirectionY(float SCDY) { calculateDirectionY = SCDY; }

    float passCalculateDirectionX() { return calculateDirectionX; }
    float passCalculateDirectionY() { return calculateDirectionY; }

    void setCameraMovementSpeed(float SCMS) { cameraMovementSpeed = SCMS; }
    float passCameraMovementSpeed() { return cameraMovementSpeed; }

    void setDelayCameraMovement(float SDCM) { delayMovement.setDelayMili(SDCM); }
    bool isDelayingOfCameraMovementOver() { return delayMovement.isDelayOver(); }

    void coordinatesReachedSetDelayStart() { delayMovement.startClock(); } //Already has protection :)

protected:
    delaySome delayMovement;
    float designationX;
    float designationY;
    float cameraMovementSpeed;
    bool doneWithThisWaypoint;

    float calculateDirectionX;
    float calculateDirectionY;
};

void cinematicWaypoint::setGoTo(float desigX, float desigY)
{
    designationX = desigX;
    designationY = desigY;
}

cinematicWaypoint::cinematicWaypoint()
{
    designationX = 0;
    designationY = 0;

    calculateDirectionX = 0;
    calculateDirectionX = 0;

    cameraMovementSpeed = rateOfCameraMove;
}
//For now we only have SH decreased, but we leave the option for SW to also decrease.
//Because cinematicView moveX and moveY is called in a scroolcontrol function before everything starts, some values are not that important to be perfectly defined.
//weather or not this design is faulty or alright, is something that needsto be addressed later.
class cinematicView {
public:
    cinematicView();
    void play();
    //void activateCV(bool actV) { active = actV; }
    float passEffectiveSW() { return effectiveSW; }
    float passEffectiveSH() { return effectiveSH; }
    bool isActive() { return active; }
    void addNewWaypoint(float posX, float posY,float);
    //Could've been one function. \/
    void getCurrentScreenXval(float GCSXV) { currentScreenValueX = GCSXV; }
    void getCurrentScreenYval(float GCSYV) { currentScreenValueY = GCSYV; }
    float moveCameraX(float);
    float moveCameraY(float);
    bool isDesignationReached() { return designationReached; }
    //void deactivateCVOnDesignationReached(bool DODR) {deactOnDesignationReach = DODR; }
    bool isFullyInCinematicView() { return fullyInCinematicView; }
    bool isFullyOutCinematicView() { return fullyOutCinematicView; }
protected:
    vector<cinematicWaypoint> CWP;
    delaySome delayStart;
    bool fullyInCinematicView;
    bool fullyOutCinematicView;
    void drawBorderLine();
    float getRadius(float distx, float disty);

    float effectiveSW;
    float effectiveSH;

    bool positionXReached;
    bool positionYReached;

    float currentScreenValueX;
    float currentScreenValueY;

    float calculateDirectionX;
    float calculateDirectionY;

    int currentWaypoint;
    int totalWaypoints;

    bool startedMovementYet;

    bool designationReached;

    //bool deactOnDesignationReach;

    bool active;
};

void cinematicView::addNewWaypoint(float posX, float posY, float delayAtDestination = standardCameraStandStillDelay)
{
    startedMovementYet = true;
    designationReached = false;

    active = true;

    if(posX < SW/2)
        posX = SW/2;

    if(posX > worldCordsX-SW/2)
        posX = worldCordsX-SW/2;

    if(posY < SH/2)
        posY = SH/2;

    if(posY > worldCordsY-SH/2)
        posY = worldCordsY-SH/2;

    float obliqueDistance = 0;

    float adjustedScreenXVal;
    float adjustedScreenYVal;

    if(totalWaypoints > 0)
    {
        adjustedScreenXVal = CWP[totalWaypoints-1].giveDesignationX();
        adjustedScreenYVal = CWP[totalWaypoints-1].giveDesignationY();
    }

    else
    {
        adjustedScreenXVal = currentScreenValueX+SW/2;
        adjustedScreenYVal = currentScreenValueY+SH/2;
    }

    //cout << "\nadjustedScreenXVal " << adjustedScreenXVal << endl;
    //cout << "adjustedScreenYVal " << adjustedScreenYVal << endl;
    /*
    calculateDirectionX = mx - positionX;
    calculateDirectionY = my - positionY;

    obliqueDistance = getRadius((positionX-mx), (positionY-my));

    calculateDirectionX /= obliqueDistance;
    calculateDirectionY /= obliqueDistance;

    */

    calculateDirectionX = posX - adjustedScreenXVal;
    calculateDirectionY = posY - adjustedScreenYVal;

    //cout << "\ndesignatedX " << posX << endl;
    //cout << "designatedY " << posY << endl;

    //cout << "adjustedScreenXVal " << adjustedScreenXVal << endl;
    //cout << "adjustedScreenYVal " << adjustedScreenYVal << endl;

    //cout << "\ndesignatedX - adjustedScreenXVal " << (posX - adjustedScreenXVal);
    //cout << "\ndesignatedY - adjustedScreenYVal " << (posY - adjustedScreenYVal);

    obliqueDistance = getRadius((posX-adjustedScreenXVal), (posY-adjustedScreenYVal));

    //cout << "\nObliqueDistance " << obliqueDistance << endl;

    calculateDirectionX /= obliqueDistance;
    calculateDirectionY /= obliqueDistance;

    //cout << "calculateDirectionX /= obliqueDistance: " << calculateDirectionX << endl;
    //cout << "calculateDirectionY /= obliqueDistance: " << calculateDirectionY << endl;

    //Sleep(10000);

    cinematicWaypoint tempWaypoint;

    tempWaypoint.setCalculateDirectionX(calculateDirectionX);
    tempWaypoint.setCalculateDirectionY(calculateDirectionY);

    //cout << "\ntempWaypoint.passCalculateDirectionX()" << tempWaypoint.passCalculateDirectionX() << endl;
    //cout << "tempWaypoint.passCalculateDirectionY()" << tempWaypoint.passCalculateDirectionY() << endl;

    tempWaypoint.setDelayCameraMovement(delayAtDestination);
    tempWaypoint.setGoTo(posX,posY);

    totalWaypoints++;

    CWP.push_back(tempWaypoint);

   // cout << "currentWaypoint: " << currentWaypoint << endl;
   // cout << "totalWaypoints: " << totalWaypoints << endl;

    //cout << "\ntempWaypoint.passCalculateDirectionX()" << CWP[totalWaypoints-1].passCalculateDirectionX() << endl;
    //cout << "tempWaypoint.passCalculateDirectionY()" << CWP[totalWaypoints-1].passCalculateDirectionY() << endl;

    //Sleep(5000);
}

float cinematicView::moveCameraX(float screenXVal)
{
   float adjustedScreenXVal = screenXVal+SW/2;

   if(fullyInCinematicView)
   {
       //cout << "GET REFERECETIME: " << delayStart.getReferenceTime() << endl;
      // cout << SDL_GetTicks() << endl;

        if(delayStart.clockNotStartedYet())
        {
            //cout << "CLOCK ACTIVATED";
            delayStart.startClock();
        }
   }

    if(delayStart.isDelayOver() && totalWaypoints > 0)
    {
        delayStart.setLoopEvent(true);

       // cout << "\nadjustedScreenXVal " << adjustedScreenXVal << endl;
        //cout << "giveDesignationX " << CWP[currentWaypoint].giveDesignationX() << endl;
        //Sleep(10);

        if(!(adjustedScreenXVal > CWP[currentWaypoint].giveDesignationX()-10 && adjustedScreenXVal < CWP[currentWaypoint].giveDesignationX()+10) && fullyInCinematicView && !designationReached)
        {
            screenXVal += CWP[currentWaypoint].passCalculateDirectionX()*CWP[currentWaypoint].passCameraMovementSpeed();
            //cout << "\nCWP[currentWaypoint].passCalculateDirectionX(): " << CWP[currentWaypoint].passCalculateDirectionX() << endl;
            //cout << "CWP[currentWaypoint].passCameraMovementSpeed(): " << CWP[currentWaypoint].passCameraMovementSpeed() << endl;
            //cout << "screenXVal: " << screenXVal << endl;
           // Sleep(10);
            currentScreenValueX = screenXVal;
        }

        else if((adjustedScreenXVal >= CWP[currentWaypoint].giveDesignationX()-10 && adjustedScreenXVal <= CWP[currentWaypoint].giveDesignationX()+10))
        {
            if(!positionXReached)
                positionXReached = true;

            if(positionXReached && positionYReached)
            {
                CWP[currentWaypoint].coordinatesReachedSetDelayStart();

                if(CWP[currentWaypoint].isDelayingOfCameraMovementOver())
                {
                   // cout << "\n\nTARGET HAS BEEN REACHED!!!!!!!!!!!!!!\n\n" << endl;
                    currentWaypoint++;

                    if(currentWaypoint == totalWaypoints) ///deactOnDesignationReach &&
                        active = false;

                    startedMovementYet = false;

                    positionXReached = false;
                    positionYReached = false;

                    if(currentWaypoint == totalWaypoints)
                    {
                        currentWaypoint = 0;
                        totalWaypoints = 0;
                        delayStart.restartEvent();
                        delayStart.stopClock();
                        delayStart.setLoopEvent(false);
                        CWP.erase(CWP.begin(),CWP.end());
                        designationReached = true;

                    }
                }
            }
        }
    }

    return screenXVal;
}

float cinematicView::moveCameraY(float screenYVal)
{
    float adjustedScreenYVal = screenYVal+SH/2;

    if(fullyInCinematicView)
        if(delayStart.clockNotStartedYet())
            delayStart.startClock();

    if(delayStart.isDelayOver() && totalWaypoints > 0)
    {
        delayStart.setLoopEvent(true);

        //cout << "\nadjustedScreenYVal " << adjustedScreenYVal << endl;
        //cout << "giveDesignationY " << CWP[currentWaypoint].giveDesignationY() << endl;
        //Sleep(10);

        if(!(adjustedScreenYVal > CWP[currentWaypoint].giveDesignationY()-10 && adjustedScreenYVal < CWP[currentWaypoint].giveDesignationY()+10) && fullyInCinematicView && !designationReached)
        {
            screenYVal += CWP[currentWaypoint].passCalculateDirectionY()*CWP[currentWaypoint].passCameraMovementSpeed();
            //cout << "\nscreenYVal: " << screenYVal << endl;
            //cout << "CWP[currentWaypoint].giveDesignationY(): " << CWP[currentWaypoint].giveDesignationY();
            //Sleep(10);
            currentScreenValueY = screenYVal;
        }

        else if((adjustedScreenYVal >= CWP[currentWaypoint].giveDesignationY()-10 && adjustedScreenYVal <= CWP[currentWaypoint].giveDesignationY()+10))
        {
            //Sleep(1000);
            //cout << "\n\nYYYYYYY TARGET HAS BEEN REACHED!!!!!!!!!!!!!!\n\n" << endl;
            if(!positionYReached)
                positionYReached = true;

            if(positionYReached && positionXReached)
            {
                CWP[currentWaypoint].coordinatesReachedSetDelayStart();

                if(CWP[currentWaypoint].isDelayingOfCameraMovementOver())
                {
                    currentWaypoint++;

                    if(currentWaypoint == totalWaypoints) //deactOnDesignationReach &&
                        active = false;

                    startedMovementYet = false;

                    positionXReached = false;
                    positionYReached = false;

                    if(currentWaypoint == totalWaypoints)
                    {
                        currentWaypoint = 0;
                        totalWaypoints = 0;

                        designationReached = true;

                        delayStart.setLoopEvent(false);

                        delayStart.restartEvent();
                        delayStart.stopClock();
                        CWP.erase(CWP.begin(),CWP.end());

                    }
                }
            }
        }
    }

    return screenYVal;
}

float cinematicView::getRadius(float distx, float disty)
{
    float radiusSquared;
    float radius;

    radiusSquared = pow(distx,2) + pow(disty,2);
    radius = sqrt(radiusSquared);

    return radius;
}

cinematicView::cinematicView()
{
    fullyInCinematicView = false;
    fullyOutCinematicView = true;

    designationReached = false;
    //deactOnDesignationReach = false;

    startedMovementYet = false;

    calculateDirectionX = 0;
    calculateDirectionY = 0;

    currentScreenValueX = 0;
    currentScreenValueY = 0;

    positionXReached = false;
    positionYReached = false;

    currentWaypoint = 0;
    totalWaypoints = 0;

    delayStart.setDelayMili(initiationDelayOfCameraMovement);
    delayStart.setDelayAsEventBased(true);

    effectiveSW = SW;
    effectiveSH = SH;
}

void cinematicView::drawBorderLine()
{
    glLoadIdentity();
    glColor4f(1.0,1.0,1.0,1.0);
    glEnable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, texture[18]);
    glBegin(GL_QUADS);
    //The -10 and +10 to effectiveSW is just so thtat the line is covering the entire section.
    glTexCoord2f(0.0,0.0); glVertex3f(-10, (SH-effectiveSH)-pushedInBorderLine, 0.0);
    glTexCoord2f(1.0,0.0); glVertex3f(effectiveSW+10, (SH-effectiveSH)-pushedInBorderLine, 0.0);
    glTexCoord2f(1.0,1.0); glVertex3f(effectiveSW+10, (SH-effectiveSH)-(pushedInBorderLine-widthOfBorderLine), 0.0);
    glTexCoord2f(0.0,1.0); glVertex3f(-10, (SH-effectiveSH)-(pushedInBorderLine-widthOfBorderLine), 0.0);

    glTexCoord2f(0.0,0.0); glVertex3f(-10, effectiveSH+pushedInBorderLine, 0.0);
    glTexCoord2f(1.0,0.0); glVertex3f(effectiveSW+10, effectiveSH+pushedInBorderLine, 0.0);
    glTexCoord2f(1.0,1.0); glVertex3f(effectiveSW+10, effectiveSH+(pushedInBorderLine-widthOfBorderLine), 0.0);
    glTexCoord2f(0.0,1.0); glVertex3f(-10, effectiveSH+(pushedInBorderLine-widthOfBorderLine), 0.0);

    glEnd();
    glDisable(GL_BLEND);

}

void cinematicView::play()
{
    if(active)
    {
        if(!fullyInCinematicView)
            effectiveSH -= rateOfCinematicViewActivation;

        if(SH - effectiveSH >= sizeOfCinematicViewScreenDecrease)
        {
            fullyInCinematicView = true;
            fullyOutCinematicView = false;
        }

        SDL_WarpMouse(-worldCordsX-50,-worldCordsY-50); //So that no mouse action can ever be performed.
        drawBorderLine();
    }

    else if(!active)
    {
        if(!fullyOutCinematicView)
        {
            effectiveSH += rateOfCinematicViewActivation;
            SDL_WarpMouse(SW/2,SH/2);
            drawBorderLine();
        }

        if(SH - effectiveSH <= 0)
        {
            fullyInCinematicView = false;
            fullyOutCinematicView = true;
        }


    }
}

class scrollingFeature {
public:
    scrollingFeature();
    cinematicView CV;
    void scrolling();
    float drawInRelationToScreenX(float); //Yes I believe coordinates could be used to limit this to one function, but that will only be needed in vs. 1.1.
    float drawInRelationToScreenY(float);
    bool inScreen(float,float,float); //Please read above drawInRelationToScreen definition.
    float adjustInputValueX(float inputValueX) { return (inputValueX+screenXCords);}
    float adjustInputValueY(float inputValueY) { return (inputValueY+screenYCords);}
    void shakeCameraInitiation(float duration, int intensity);
    void shakeCamera();
    void cinematicViewUpdateCameraMovement();

    ///ONLY PASSED TO CV.
    float passScreenXCords() { return screenXCords; }
    float passScreenYCords() { return screenYCords; }

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

    if(screenXCords+CV.passEffectiveSW() >= worldCordsX)
        screenXCords = worldCordsX-CV.passEffectiveSW();

    if(screenYCords <= 0)
        screenYCords = 0;

    if(screenYCords+CV.passEffectiveSH() >= worldCordsY)
        screenYCords = worldCordsY-CV.passEffectiveSH();
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
    if(((objectWorldCordsX+(objectSize)) >= screenXCords+(SW-CV.passEffectiveSW()) && (objectWorldCordsX-(objectSize)) <= (screenXCords+CV.passEffectiveSW())) &&//Checks x boxing collision.
        ((objectWorldCordsY+(objectSize)) >= screenYCords+(SH-CV.passEffectiveSH()) && (objectWorldCordsY-(objectSize)) <= (screenYCords+CV.passEffectiveSH())) ) //Checks y boxing collision.
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
    scrollingMultiplierX = ((mx - (SW/2))/(CV.passEffectiveSW()/2)); //This effective feature will probably never get used, since we cannot scroll while CV is active. Yet its complete.
    scrollingMultiplierY = ((my - (SH/2))/(CV.passEffectiveSH()/2));

    screenXCords += (scrollingMultiplierX * scrollSpeed);
    screenYCords += (scrollingMultiplierY * scrollSpeed);

    handleEndMapCollision();
    }
}

void scrollingFeature::cinematicViewUpdateCameraMovement()
{
    CV.getCurrentScreenXval(screenXCords);
    CV.getCurrentScreenYval(screenYCords);

    screenXCords = CV.moveCameraX(screenXCords);
    screenYCords = CV.moveCameraY(screenYCords);
}

bool scrollingFeature::checkIfScroll(float mx, float my)
{
    //Important to note that reducing the amount of checks or variables has not proven viable. Little thought however was spent due to the logical nature of the problem, that 4 checks are needed
    //If these checks are not run, the screen will scroll the entire time. If there is an more efficient way it would be quite awesoome :).
    if(!CV.isActive())
    {
        if(mx >= (CV.passEffectiveSW() - scrollingLeverage))
            return true;

        if(mx <= ((SW-CV.passEffectiveSW()) + scrollingLeverage))
            return true;

        if(my >= (CV.passEffectiveSH() - scrollingLeverage))
            return true;

        if(my <= ((SH-CV.passEffectiveSH()) + scrollingLeverage))
            return true;
    }

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

    ///THESE TEXTURES WERE OBTAINED FROM THE SEARCH PHRASE: "fiery text <Insert Letter>"
    ///IT IS HIGHLY ADVISABLE TO BUILD AN ENTIRE FONT FOR THIS GAME.
    ///IMAGES PROVIDED BY: WALLCO.COM
    if(!(textureImages[4] = IMG_Load("FieryEAlphad.png"))) return false;
    if(!(textureImages[5] = IMG_Load("FieryLAlphad.png"))) return false;
    if(!(textureImages[6] = IMG_Load("FieryMAlphad.png"))) return false;
    if(!(textureImages[7] = IMG_Load("FieryNAlphad.png"))) return false;
    if(!(textureImages[8] = IMG_Load("FieryAAlphad.png"))) return false;
    if(!(textureImages[9] = IMG_Load("FieryTAlphad.png"))) return false;
    if(!(textureImages[10] = IMG_Load("FieryRAlphad.png"))) return false;
    if(!(textureImages[11] = IMG_Load("FieryCAlphad.png"))) return false;
    if(!(textureImages[12] = IMG_Load("FieryHAlphad.png"))) return false;

    if(!(textureImages[13] = IMG_Load("SelectedCircleRotationAlpha.png"))) return false;
    if(!(textureImages[14] = IMG_Load("fireballAlpha.png"))) return false;
    if(!(textureImages[15] = IMG_Load("chargedFireAlpha.png"))) return false;
    if(!(textureImages[16] = IMG_Load("SmoothFire_PowerAlpha.png"))) return false;
    if(!(textureImages[17] = IMG_Load("BetterSimpleParticleAlpha.png"))) return false;
    if(!(textureImages[18] = IMG_Load("BorderLineAlpha.png"))) return false;

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
//Copied from template.
void Print(int x, int y, int set, float fontSize, bool opaque, char *string, ... )
{
    va_list ap;
    char text[128];

    if(scrollControl.inScreen(x,y,5))
    {
        if(opaque && GLOBAL_VISIBILITY_OF_GAME_CONTENTS >= 1)
            glColor4f(1.0, 1.0, 1.0, 1.0);

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
        in the texture font i suppose. Then we + (128 * set) because there is actually only 128 symbols/letters, thus when we go over it for eg
        156 its just differnt font style. Thus when set is 1 its a differnt font style then when set is 0(BOLD). that is also why we can only
        have 128 letters - text[128] - because if we had more we give the opertunity to go from font styles we going over 128's style. Hope
        you understand more now :) */

        glScalef(fontSize, fontSize, fontSize);


        glCallLists(strlen(text), GL_BYTE, text);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
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

            Print(scrollControl.drawInRelationToScreenX(x), scrollControl.drawInRelationToScreenY(y+i*lineSpacing), 1, standardFontSize, false, conversation[i]);
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

    SDL_WM_GrabInput(SDL_GRAB_ON);

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

    float damage;
    float health;

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
    int FireChainSmoothMovement;
    bool rotateThisParticle;
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

    float getDamage() { return damage;}
    void setDamage(float damageVal) { damage = damageVal; }
    float getHealth() { return health; }
    void setHealth(float HPset) { health = HPset; }
    void modifyHealthBy(float setValue) { health += setValue; }
    void modifyDamageBy(float setValue) { damage += setValue; } //Kept in for if we want standard damage;

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
    void setToNoRotation() { rotateThisParticle = false; }

    void multiplyRotation(float multiplyValue) { particleRotation *= multiplyValue; }

    void movePartTo(particle *Part, int*);
    float FCSmoothMovementXandY();
};

float particle::FCSmoothMovementXandY()
{
    if(FireChainSmoothMovement > FireChainParticleSmoothMovementLevel*1000) //Usally this level will never be reached, but it's just for extra protection.
        FireChainSmoothMovement = 0;

    FireChainSmoothMovement++;
    return FireChainSmoothMovement*movementRateOfResourceParts/FireChainParticleSmoothMovementLevel;
}

void particle::updates()
{
    life -= fade;
    ///NB THIS WAS USED IN PREVIOUS GAMES.
    //if(life < 0){
    //    x = permaX;
    //    y = permaY;
    //    life = allParticles_base_life;
    //}
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
    FireChainSmoothMovement = 0;
    rotateThisParticle = true;

    health = StandardHealth;

    damage = 0;
    #ifdef enableStandardDamage
    damage = StandardDamage;
    #endif

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

        glColor4f(R, G, B, life*GLOBAL_VISIBILITY_OF_GAME_CONTENTS);

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture[Text]);

        if(rotateThisParticle)
        {
            glRotatef(particleRotationConcatenated,0,0,1);
            particleRotationConcatenated+=particleRotation;

            if(particleRotationConcatenated > 360)
                particleRotationConcatenated = 0;
        }

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

class explosiveIgnition {
public:
    explosiveIgnition();
    void setPosition(float selectedPositionX, float selectedPositionY);
    void explosiveLaunch();
    void showExplosion();
    void setExplosionColor(float r, float g, float b) { colorR = r; colorG = g; colorB = b; }
    void setExplosionSize(float sizeSet) { exploSize = sizeSet; }
    void setLightDissipation(float LD) { lightDissp = LD; }
protected:
    particle *explosiveParts[explosiveIntensity];
    //Used american spelling because particle also does.
    float colorR;
    float colorG;
    float colorB;
    float exploSize;
    float lightDissp;
};

explosiveIgnition::explosiveIgnition()
{
    exploSize = explosiveSize;
    colorR = standardExplosiveIgnitionColorR;
    colorG = standardExplosiveIgnitionColorG;
    colorB = standardExplosiveIgnitionColorB;

    lightDissp = lightDissipationLevel;

    for(int i = 0; i < explosiveIntensity; i++)
    {
        explosiveParts[i] = new particle;
        explosiveParts[i]->activate(true);
        explosiveParts[i]->setTexture(17);
        explosiveParts[i]->setColor(colorR, colorG, colorB);
        explosiveParts[i]->setFade(0);
        explosiveParts[i]->setSize(exploSize);
        explosiveParts[i]->setLife(0.0);

    }
}

void explosiveIgnition::setPosition(float selectedPositionX, float selectedPositionY)
{
    for(int i = 0; i < explosiveIntensity; i++)
    {
        explosiveParts[i]->position(selectedPositionX, selectedPositionY);
    }
}

void explosiveIgnition::explosiveLaunch()
{
    for(int i = 0; i < explosiveIntensity; i++)
    {
        explosiveParts[i]->setColor(colorR, colorG, colorB);
        explosiveParts[i]->setFade(lightDissp);
        explosiveParts[i]->setSize(exploSize);
        explosiveParts[i]->setLife(1.0);
    }

}

void explosiveIgnition::showExplosion()
{
    for(int i = 0; i < explosiveIntensity; i++)
    {
        if(explosiveParts[i]->Rlife() >= 0)
            explosiveParts[i]->move();

        else
            explosiveParts[i]->setFade(0);
    }

}

class fireTrail {
public:
    fireTrail();
    void showFireTrail(float,float);
   // void setfireTrailPosition(float x, float y) { FTx = x; FTy = y; }
    void fireTrailLength(float fadingTrail) { trailFade = fadingTrail; }
    void setFireTrailColor(float r, float g, float b) { colorR = r; colorG = g; colorB = b; }
    void setFireTrailSize(float SS) { fireTrailSizeVar = SS; }
    void setActiveState(bool Act) { activated = Act; }
protected:
    particle *fireTrailParts[fireTrailIntensity];
    float trailFade;
    float colorR;
    float colorG;
    float colorB;
    float fireTrailSizeVar;

    bool activated;
};

fireTrail::fireTrail()
{
    colorR = fireTrailStandardColorR;
    colorG = fireTrailStandardColorG;
    colorB = fireTrailStandardColorB;
    fireTrailSizeVar = sizeOfFireTrail;
    activated = true;

    trailFade = standardFireTrailFade;

    float tempfireTrailIntensity = fireTrailIntensity;
    float calculateLifeVariableDistribution = 1/tempfireTrailIntensity;

    for(int i = 0; i < fireTrailIntensity; i++)
    {
        fireTrailParts[i] = new particle;
        fireTrailParts[i]->activate(true);
        fireTrailParts[i]->setTexture(17);
        fireTrailParts[i]->setSize(fireTrailSizeVar);
        fireTrailParts[i]->setColor(colorR, colorG, colorB);
        fireTrailParts[i]->setLife(i*calculateLifeVariableDistribution*fireTrailOpaquecy); //So that each one has a differnt life, so that we have continious trail and its equal.
        fireTrailParts[i]->position(-100,-100); //Out of screen so that everything stababilzes in the screen.
        //cout << i << endl;
        fireTrailParts[i]->setFade(trailFade);
    }
}

void fireTrail::showFireTrail(float posX, float posY) //Consistently needs to update to a certain point, so makes sence consistently passing the neccesary positions.
{
    if(activated)
    {
        for(int i = 0; i < fireTrailIntensity; i++)
        {
            if(fireTrailParts[i]->Rlife() > 0)
                fireTrailParts[i]->move();

            else
            {
                fireTrailParts[i]->setLife(fireTrailOpaquecy);
                fireTrailParts[i]->setFade(trailFade);
                fireTrailParts[i]->position(posX,posY);
            }
        }
    }
}

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
    float getPower() { return nodePower; }
    float getInitialPower() { return initialPowerOfNode; }
protected:
    particle *nodeStrength[maxNodeStrength];
    int nodePower;
    int initialPowerOfNode;
    float x;
    void lifeIntensity();
    float y;
    bool linked; //Means the firechain is going to be connected to this Node;
    bool connected; //Means the firechain IS connected to this node;
    bool active;
};

resourceNode::resourceNode(float positionX, float positionY, int nodeS)
{
    linked = false;
    connected = false;
    x = positionX;
    y = positionY;
    nodePower = nodeS;
    initialPowerOfNode = nodePower;
    active = true;

    for(int i = 0; i < nodePower; i++)
    {
        nodeStrength[i] = new particle;
        nodeStrength[i]->activate(true);
        nodeStrength[i]->position(x,y);
    }

    lifeIntensity();
}

void resourceNode::lifeIntensity()
{
    float lifeValue = (float)1/(nodePower/lifeDiminishingFactor);

    for(int i = 0; i < nodePower; i++)
    {
        if(nodeStrength[i]->isActive())
        {
            nodeStrength[i]->setLife(lifeValue);
        }
    }
}

void resourceNode::animate()
{
    if(active)
    {
       // lifeIntensity();

        for(int i = 0; i < (int)(nodePower/displayReduction); i++)
        {
            if(nodeStrength[i]->isActive())
            {
                nodeStrength[i]->move();
            }
        }

        Print(x-PositionLeftObjectsForInfoText,y-PositionAboveObjectsForInfoText, 0, standardFontSize, true, "{%d}", nodePower); //< max, not <=. Thus not +1 anymore.
    }
}

void resourceNode::drainingNode(particle *partx, int totalParts)
{
    if(nodePower > 0)
    {
        nodeStrength[nodePower-1]->movePartTo(partx, &totalParts);
        nodeStrength[nodePower-1]->activate(false);
        nodePower--;
    }

    else active = false;
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
    float getNodePower(int whichNode) { return RNs[whichNode]->getPower(); }
    float InitialNodePower(int whichNode) { return RNs[whichNode]->getInitialPower(); }
    void setNodeActiveState(int whichNode, bool actV = true) { RNs[whichNode]->activateRN(actV); }

    void checkToConnectYet(int, float, float);
    void tapNode(int whichNode, particle *, int );
protected:
    resourceNode *RNs[maxNodesOnMap]; //Probably not neccesary, but I like using pointers and assinging new.
    int nodesActive;
    int nodesOnMap;
    int randoming(int maxRand, int addition);
};

void resourceSystem::tapNode(int whichNode, particle *prx, int totalPrts)
{
    RNs[whichNode]->drainingNode(prx, totalPrts);
}

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
    int valuex = rand()%maxRand+1;
    valuex += addition;

    if(valuex > (maxRand+addition))
        valuex = (maxRand+addition);

    return valuex;
}

resourceSystem::resourceSystem()
{
    //const int maxNodeStrength = 10; Used for testing and debugging.
    //const int initialNodePower = 5;

    nodesOnMap = randoming((maxNodesOnMap-minimunAmountOfNodesOnMap), minimunAmountOfNodesOnMap);

    nodesActive = nodesOnMap;

    for(int i = 0; i < nodesOnMap; i++)
    {
        RNs[i] = new resourceNode(randoming(worldCordsX, 0), randoming(worldCordsY, 0), randoming((maxNodeStrength-minimumNodeStrength), minimumNodeStrength));
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

class NodeCreated : public globalFunctions{
public:
    NodeCreated();
    void created(bool create) { createdByFireChain = create;}
    bool IsNodeCreated() {return createdByFireChain; }
    bool IsNodeConnected() { return amIConnected; }
    void increaseNodePower(particle *, int);
    void checkToConnectYet(float X, float Y);
    void animate();
    void setPositionX(float X) { positionX = X;}
    void setPositionY(float Y) { positionY = Y;}
    float getXCords() { return positionX; }
    float getYCords() { return positionY; }
    void checkToSelect(float Mousemx, float Mousemy);
    void handleSpellThrow(float mx, float my);
    void spellSelect(fireElementalSpellType FESP) {whichSpellIsCurrentlyActive = FESP;}
    void setMouseButtonLeftStatus(bool setS) { mouseButtonDown = setS; }
    void handleControlGroups(int,bool,bool);
    void setSelectedManually(bool SSM) { currentlySelected = SSM; }
    float collisionKillParticles(int,float,float);
    int getCurrentAmountOfKillParts() { return currentKillParticle; }
protected:
    particle *nodeParts[maxPowerOfNodeCreated]; //Once again, fixed later with linked list.
    particle *killParticles[maxAmountOfKillParticles];
    explosiveIgnition explodingLaunch;
    fireTrail fireBallFireTrail[fireBallPower/maxAmountOfKillParticles];
    delaySome spellDelay; //Will be set manually depending on which spell is being casted.
    int createdNodePower;
    int currentKillParticle;
    fireElementalSpellType whichSpellIsCurrentlyActive;
    int controlGroupNodeIsIn;
    bool amIConnected;
    bool maxNodePowerReached;
    float positionX;
    void lifeIntensity();
    float positionY;
    bool createdByFireChain;
    void selectedCircleDisplay();
    bool currentlySelected;
    float selectedCircleRotation;
    void castChargedFire(float mx, float my);
    void castFireBall(float mx, float my);

    bool mouseButtonDown; //This is neccesary, since SDL does not have a API function for when the button is HELD DOWN. We create our own.
};

float NodeCreated::collisionKillParticles(int whichKillParticleToCheck, float collisionPositionX, float collisionPositionY)
{
    if(killParticles[whichKillParticleToCheck]->isActive())
    {
        if(killParticles[whichKillParticleToCheck]->returnX() > collisionPositionX-killParticlesCollisionLeverage && killParticles[whichKillParticleToCheck]->returnX() < collisionPositionX+killParticlesCollisionLeverage)
            if(killParticles[whichKillParticleToCheck]->returnY() > collisionPositionY-killParticlesCollisionLeverage && killParticles[whichKillParticleToCheck]->returnY() < collisionPositionY+killParticlesCollisionLeverage)
            {
                killParticles[whichKillParticleToCheck]->position(-10,-10); ///Puts it out of the world so that it could never collide again. NOT GOOD DESIGN.
                ///REASON FOR THIS, IS THAT IM NOT COMPLETELY SURE HOW IMPORTANT ACTIVATE AND EVERYTHING IS CONCERNING THE CURRENT GAME STATE. WHEN WE HAVE CHANCE, WE NEED TO CHECK IT'S
                ///RELAVANCE TO EVERYTHING AND THEN DESIGN THIS ACCORDINGLY. FOR EG: IF RETURNX IS NOW CANCELED ON DISACTIVATE, MAYBE A DIFFERENT PART OF THE GAME FAILS.
                killParticles[whichKillParticleToCheck]->activate(false);
                //killParticles[i]->playDeathAnimation(true);

                ///SINCE WE WILL NOT HAVE SEQUENCIAL DEACTIVATION. WE CANNOT REDUCE THEM :(. A RELATIVELY MAJOR DESIGN FLAW.
                //currentKillParticle--;
                return killParticles[whichKillParticleToCheck]->getDamage();
            }
    }
    return -1; // False int value; Could've maybe used 0. But -1 is more understandable.

}

void NodeCreated::castChargedFire(float mx, float my)
{
    if(mouseButtonDown)
    {

    }

    else if(!mouseButtonDown)
    {

    }
}

void NodeCreated::castFireBall(float mx, float my)
{
    int ahackvalueformovepartto = 0; ///LITERALLY CAUSES CRASHES WITHOUT THIS VALUE. IT IS ESSENTIAL THAT THIS ***** FUNCTION GETS FIXED!
    float calculateDirectionX;
    float calculateDirectionY;
    float obliqueDistance;

    float fireBallDeviation;

    if(mouseButtonDown)
    {
        if(spellDelay.isDelayOver())
        {
            if(createdNodePower >= fireBallPower)
            {
                ///NB!! DESIGN OPTION!:
                ///IF YOU WANT EACH PARTICLE TO HAVE IT'S OWN DEVIATION, COPY THIS CODE INTO THE FOR LOOP! (JUST THE FIREBALLDEVIATION PART)

                explodingLaunch.setPosition(positionX,positionY);
                explodingLaunch.explosiveLaunch();

                calculateDirectionX = mx - positionX;
                calculateDirectionY = my - positionY;

                obliqueDistance = getRadius((positionX-mx), (positionY-my));

                calculateDirectionX /= obliqueDistance;
                calculateDirectionY /= obliqueDistance;

                fireBallDeviation = (rand()%intensityOfDeviationOfFireBall) - intensityOfDeviationOfFireBall/2;
                fireBallDeviation *= variableDistributionOfDeviation;

                for(int i = (createdNodePower-1); i > (createdNodePower-1-fireBallPower); i--)
                {
                  //  cout << i << endl;
                  //  cout << currentKillParticle << endl;

                    killParticles[currentKillParticle] = new particle;
                    nodeParts[i]->movePartTo(killParticles[currentKillParticle], &ahackvalueformovepartto); ///HACKING THE SECOND VARIABLE.
                    nodeParts[i]->activate(false);

                    killParticles[currentKillParticle]->setXspeed(calculateDirectionX*rateOfKillParticlesMovement+fireBallDeviation);
                    killParticles[currentKillParticle]->setYspeed(calculateDirectionY*rateOfKillParticlesMovement+fireBallDeviation);

                    currentKillParticle++;

                    if(currentKillParticle > maxAmountOfKillParticles)
                        currentKillParticle = 0;
                }

            createdNodePower-= fireBallPower;
            }
        }
    }
}

void NodeCreated::handleControlGroups(int controlGroupKeyPassed, bool cntrDown, bool shftDown)
{
    if(controlGroupKeyPassed == controlGroupNodeIsIn)
        currentlySelected = true;

    else if(!cntrDown && controlGroupKeyPassed != controlGroupNodeIsIn)
        currentlySelected = false;

    if(cntrDown && currentlySelected)
    {
        currentlySelected = false;
        controlGroupNodeIsIn = controlGroupKeyPassed;
    }
}

void NodeCreated::handleSpellThrow(float mx, float my)
{
    if(currentlySelected && mouseButtonDown) //Just additional protection.
    {
        switch(whichSpellIsCurrentlyActive)
        {
            case chargedFire:
                spellDelay.setDelayMili(chargedFireCastingDelay);
                castChargedFire(mx,my);
            break;

            case fireball:
                spellDelay.setDelayMili(fireBallCastingDelay);
                castFireBall(mx,my);
            break;

            default:
            break;
        }
    }
}

void NodeCreated::checkToSelect(float Mousemx, float Mousemy)
{
    if(Mousemx > positionX-mouseNodePushingLeverage && Mousemx < positionX+mouseNodePushingLeverage)
        if(Mousemy > positionY-mouseNodePushingLeverage && Mousemy < positionY+mouseNodePushingLeverage)
            currentlySelected = !currentlySelected;
}

void NodeCreated::selectedCircleDisplay()
{
    float screenAdjustedPositionX;
    float screenAdjustedPositionY;

    if(currentlySelected)
    {
        screenAdjustedPositionX = scrollControl.drawInRelationToScreenX(positionX);
        screenAdjustedPositionY = scrollControl.drawInRelationToScreenY(positionY);

        glLoadIdentity();
        //For a fading in and out circle, alpha value (Just copy in): selectedCircleOpaqueValue*fabs(sin(selectedCircleRotation/4))+selectedCircleOpaqueValue/2
        glColor4f(1.0, 1.0, 1.0, selectedCircleOpaqueValue);
        glTranslatef(screenAdjustedPositionX,screenAdjustedPositionY,0);

        glEnable(GL_BLEND);
        glRotatef(selectedCircleRotation, 0,0,1);

            selectedCircleRotation += selectedCircleRotationSpeed;
            if(selectedCircleRotation > 360)
                selectedCircleRotation = 0;

        glBindTexture(GL_TEXTURE_2D, texture[13]); //Circle Texture
        glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0); glVertex2f(TS*1.5,+TS*1.5);
        glTexCoord2f(1.0,0.0); glVertex2f(-TS*1.5,+TS*1.5);
        glTexCoord2f(1.0,1.0); glVertex2f(-TS*1.5,-TS*1.5);
        glTexCoord2f(0.0,1.0); glVertex2f(TS*1.5,-TS*1.5);
        glEnd();
        glDisable(GL_BLEND);
        glLoadIdentity();
    }
}

NodeCreated::NodeCreated()
{
    amIConnected = false;
    createdByFireChain = false;
    currentlySelected = false;
    selectedCircleRotation = 0;
    currentKillParticle = 0;
    whichSpellIsCurrentlyActive = fireball;
    positionX = 0;
    controlGroupNodeIsIn = -1; //False value signifying that it isn't part of any group.
    positionY = 0;
    createdNodePower = 0;
    maxNodePowerReached = false;

    spellDelay.startClock();

    //Since nodePower is 0, not one node needs to be created at the start.
}

void NodeCreated::increaseNodePower(particle *partsX, int value)
{
    if(createdNodePower < maxPowerOfNodeCreated)
    {
        nodeParts[createdNodePower] = new particle;

        partsX->movePartTo(nodeParts[createdNodePower], &value);
        partsX->activate(false);

        nodeParts[createdNodePower]->position(positionX,positionY); //Just to make sure its on the node.
        createdNodePower++;
    }

    else {maxNodePowerReached = true;}
}
void NodeCreated::checkToConnectYet(float X, float Y)
{
    if(!amIConnected)
    {
        if(X > (positionX-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && X < (positionX + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
            if(Y > (positionY-fireChainAttachOnResourceNodeAdditionLeverage*2.5) && Y < (positionY + (TS+fireChainAttachOnResourceNodeAdditionLeverage*2.5)))
                amIConnected = true;
    }
}

void NodeCreated::lifeIntensity()
{
    float lifeValue = (float)1/(createdNodePower/lifeDiminishingFactor);

    for(int i = 0; i < createdNodePower; i++)
    {
        if(nodeParts[i]->isActive())
        {
            nodeParts[i]->setLife(lifeValue);
        }
    }
}

void NodeCreated::animate()
{
    ///PLEASE CHECK IF THIS IS CORRECT. VERY TIRED AT THE MOMENT.
    if(maxNodePowerReached)
        createdNodePower = maxPowerOfNodeCreated;

   // lifeIntensity();
   selectedCircleDisplay(); //So that it blends.

    for(int i = 0; i < currentKillParticle; i++)
        killParticles[i]->move();

    for(int i = 0; i < (int)(createdNodePower/displayReduction); i++)
    {
        nodeParts[i]->move();
    }

    explodingLaunch.showExplosion();
    Print(positionX-PositionLeftObjectsForInfoText,positionY-PositionAboveObjectsForInfoText, 0, standardFontSize, true, "{%d}", createdNodePower);
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
    void flowResourceParticles(resourceSystem *rs, int );
    bool delayParticleAdditionOrRemoval();
    float getResourceParticlesX(resourceSystem *RS);
    float getResourceParticlesY(resourceSystem *RS);
    void passResourceParticle(particle *, int, resourceSystem *RS);
    bool collidedWithAResourceNode() { return collisionWithResourceNode; }
    bool isResourceNodeEmpty(resourceSystem *RS);
    bool isFireChainEmpty() { if(currentFireChainPart <= 0) return true; else return false;}
    bool allResourceParticlesCollected(resourceSystem*);
    bool checkParticleRetractionDelay();
    bool isFireChainFinished() { return fireChainFinished; }
    void setFireChainFinished(bool FF = true) { fireChainFinished = FF; }
    bool isFireChainCompletelyEmpty(resourceSystem*);
    void addPushPopDefenceWhenRetracting() { pushPopDefenceWhenRetracting++; }
    void startRetractionDelay();
    bool shouldFireChainRetractYet();
    void fireChainHasCompletelyRetracted();
    bool createdItsOwnNode() { return fireChainNodeCreated; }
    void collectPartsFromFireElemental(particle *, int );
    bool collisionWithNode();


    float getLastParticleCordX() { return fireChainParts[currentFireChainPart-1]->returnX(); } //CurrentFireChainPart is not yet defined with a particle. AFAIK.
    float getLastParticleCordY() { return fireChainParts[currentFireChainPart-1]->returnY(); }

    void checkifNodeNeedsToSelectYet(float mouseXz, float mouseYz);

    void passControlGroupKeyPressed(int, bool, bool);

    void checkMouseButtonLeftStatus(bool setS);
    void throwSpellThroughNode(float mx, float my);

    float checkCollisionWithCreatedNodeKillParts(float,float);
protected:
    particle *fireChainParts[MaxParticlesInFireChain];
    //Reason for two different "particles" is because MAYBE pushing and popping can happen at the same time later, the we have the infrastructure.
    particle *resourceTransitParticles[NumberOfTransitResourceParticlesPerFireChain];
    particle *fireElementalTransitParticles[NumberOfTransitPushedParticlesPerFireChain];

    delaySome delayParticleFlow;
    delaySome delayRetraction;
    delaySome delayFireChainRetraction;

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
    bool fireChainNodeCreated;
    int whichResourceNodeCollided;

    int currentResourceTransitParticle;
    int particleToCheckCollisionOn;
    int currentFireElementalTransitParticle;
    int pushPopDefenceWhenRetracting;

    bool resourceTransitpartsAreAllFilled;
    bool fireChainTransitpartsAreAllFilled;
    bool fireChainFinished; //Completely out of action.
    int fireElementalTransitParticleCollisionCheck;
};

float fireChain::checkCollisionWithCreatedNodeKillParts(float colX, float colY)
{
    float concatenatedDamage = 0;
    float damageFromParticle;

    if(fireChainNodeCreated)
    {   //Usally more than one particle will hit at a time since their grouped. Maked sence doing it this way.
        for(int i = 0; i < NC.getCurrentAmountOfKillParts(); i++)
        {
            damageFromParticle = NC.collisionKillParticles(i,colX,colY);

            if(damageFromParticle != -1)
                concatenatedDamage += damageFromParticle;
        }
    }

    return concatenatedDamage;
}

void fireChain::checkMouseButtonLeftStatus(bool setS)
{
    if(fireChainNodeCreated)
    {
        NC.setMouseButtonLeftStatus(setS);
    }
}

void fireChain::throwSpellThroughNode(float mx, float my)
{
    if(fireChainNodeCreated)
    {
        NC.handleSpellThrow(mx,my);
    }
}

void fireChain::passControlGroupKeyPressed(int controlGroupKeyPassed, bool isControlDown, bool isShiftDown)
{
    if(fireChainNodeCreated)
    {
        NC.handleControlGroups(controlGroupKeyPassed, isControlDown, isShiftDown);
    }
}

void fireChain::checkifNodeNeedsToSelectYet(float mouseXz, float mouseYz)
{
    if(fireChainNodeCreated)
    {
        NC.checkToSelect(mouseXz, mouseYz);
    }
}

bool fireChain::collisionWithNode()
{
    if(currentFireElementalTransitParticle > 2) //To make sure the flow has started yet.
    {
        if(fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck]->returnX() != -1) ///CHECK TO SEE IF THIS PARTICLE HAS BEEN "ALLOCATED YET"/POSITIONED AT THE FIRECHAIN
            if(fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck]->returnX() > (NC.getXCords()-fireChainAttachOnResourceNodeAdditionLeverage) && fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck]->returnX() < (NC.getXCords()+fireChainAttachOnResourceNodeAdditionLeverage))
                if(fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck]->returnY() > (NC.getYCords()-fireChainAttachOnResourceNodeAdditionLeverage) && fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck]->returnY() < (NC.getYCords()+fireChainAttachOnResourceNodeAdditionLeverage))
                    return true;
    }

    else return false;
}

void fireChain::collectPartsFromFireElemental(particle *partsX, int value)
{
    if(currentFireElementalTransitParticle < NumberOfTransitPushedParticlesPerFireChain)
    {
       /// fireElementalTransitParticles[currentFireElementalTransitParticle] = new particle; WORTH TRYING THIS LATER.

        partsX->movePartTo(fireElementalTransitParticles[currentFireElementalTransitParticle], &value);
        partsX->activate(false);
        currentFireElementalTransitParticle++;

        if(currentFireElementalTransitParticle >= NumberOfTransitPushedParticlesPerFireChain)
            fireChainTransitpartsAreAllFilled = true;
    }

    ///NOT SURE ABOUT THIS LINE. //JUST MAKES SENCE THAT MOST PARTICLES WOULD BE ABSORBED BY THE TIME THE LIMIT IS REACHED. JUST MAKE THE LIMIT HIGH ENOUGH.
    ///PLEASE MAKE SURE THIS LINE IS CORRECT, IS MAY CAUSE COLLISON FAILURES AND DISPLAY/GAMEPLAY DIFFICULTIES.
    else
    {
        currentFireElementalTransitParticle = 0;
    }
}

void fireChain::fireChainHasCompletelyRetracted()
{
    delayFireChainRetraction.setLoopEvent(false);
}

bool fireChain::shouldFireChainRetractYet()
{
    if(delayFireChainRetraction.isDelayOver())
        return true;

    else return false;
}

bool fireChain::isFireChainCompletelyEmpty(resourceSystem *RS)
{
    if(collisionWithResourceNode)
    {
        if(particleToCheckCollisionOn >= RS->InitialNodePower(whichResourceNodeCollided) && currentFireChainPart <= 0) ///CHECK >=, NOT SURE IF ITS EVER = OR >.
            return true;
    }

    else return false;
}

bool fireChain::checkParticleRetractionDelay()
{
    if(delayRetraction.isDelayOver())
        return true;

    else return false;
}

bool fireChain::allResourceParticlesCollected(resourceSystem *RS)
{
    if(particleToCheckCollisionOn < RS->InitialNodePower(whichResourceNodeCollided))
        return false;

    else
    {
        delayFireChainRetraction.startClock();
        activated = false; //Firechain should start retracting now.
        return true;
    }
}

bool fireChain::isResourceNodeEmpty(resourceSystem *RS)
{
    if(RS->getNodePower(whichResourceNodeCollided) <= 0)
    {
        RS->setNodeActiveState(whichResourceNodeCollided, false);
       // activated = false; //Chain needs to retract.
        return true;
    }
    return false;
}

void fireChain::passResourceParticle(particle *prts, int CP, resourceSystem *RS)
{
    if(particleToCheckCollisionOn < RS->InitialNodePower(whichResourceNodeCollided)) ///CHECK THE -1 HERE AND MAKE SURE ITS CORRECT.
    {
        resourceTransitParticles[particleToCheckCollisionOn]->movePartTo(prts, &CP);
        resourceTransitParticles[particleToCheckCollisionOn]->activate(false);
        particleToCheckCollisionOn++;
    }
}

float fireChain::getResourceParticlesX(resourceSystem *RS)
{
    if(collisionWithResourceNode && currentResourceTransitParticle > 0) //Because fireChain animation (Which flows the particles from resourceNode is on the end of fireElem, before this, this check needs to be added to avoid SF)
    {
        //if(particleToCheckCollisionOn < RS->InitialNodePower(whichResourceNodeCollided))
        return resourceTransitParticles[particleToCheckCollisionOn]->returnX();
        //else return -100; //Offscreen value, so that collision is never true.
    }
}

float fireChain::getResourceParticlesY(resourceSystem *RS)
{
    if(collisionWithResourceNode && currentResourceTransitParticle > 0)
    {
        //if(particleToCheckCollisionOn < RS->InitialNodePower(whichResourceNodeCollided))
        return resourceTransitParticles[particleToCheckCollisionOn]->returnY();
        //else return -100;
    }
}

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
    fireChainNodeCreated = true;
    NC.setPositionX(lastCoordinateX);
    NC.setPositionY(lastCoordinateY);
}

bool fireChain::delayParticleAdditionOrRemoval()
{
    timeElapsed = SDL_GetTicks();

    if((timeElapsed-lastTimePosition) > howManyParticlesReleasedPerSecond){
        lastTimePosition = SDL_GetTicks();
        return true;
    }

    return false;
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
    activated = true;
    drawingInProgress = false;
    currentFireChainPart = 0;
    totalFireChainAccumulated = 0; //Might be the same as currentFireChainPart, but we'll address that in vs 1.1.
    positionX = 0;
    positionY = 0;
    lastCoordinateX = 0;
    lastCoordinateY = 0;
    pushPopDefenceWhenRetracting = 0;
    pathDone = false;
    particleToCheckCollisionOn = 0;
    resourceTransitpartsAreAllFilled = false;
    fireChainTransitpartsAreAllFilled = false;
    fireChainFinished = false;
    fireElementalTransitParticleCollisionCheck = 0;

    delayRetraction.setDelay(ParticleDelayRetractionOfFireChain);
    delayRetraction.startClock();

    delayFireChainRetraction.setDelayAsEventBased();
    delayFireChainRetraction.setDelay(AmountOfTimeToDelayFireChainRetractionFromResourceNode);
    delayFireChainRetraction.setLoopEvent();

    delayParticleFlow.setDelay(ResourceParticleFlowRatePerSecond);

    currentResourceTransitParticle = 0;
    currentFireElementalTransitParticle = 0;

    collisionWithResourceNode = false;
    fireChainNodeCreated = false;
    whichResourceNodeCollided = -1; //SO no confusing can occur, this is used as NOT TRUE variable value.

    timeElapsed = 0;
    lastTimePosition = 0;
    howManyParticlesReleasedPerSecond = 1000/FireChainsParticleFeedRatePerSecondFromFireElemental;

    for(int i = 0; i < NumberOfTransitResourceParticlesPerFireChain; i++)
        resourceTransitParticles[i] = new particle;

    for(int i = 0; i < NumberOfTransitPushedParticlesPerFireChain; i++)
    {
        fireElementalTransitParticles[i] = new particle;
        fireElementalTransitParticles[i]->position(-1,-1); //THIS IS USED AS A FEATURE TO MAKE SURE THE PARTICLES ARE POSIONED BEFORE USE.
    }
}

void fireChain::popPart(particle *fireElementalParticle, int currentParticle)
{
    //Checking if there is actually anything to push is probably also something that needs to be built in...
    if(currentFireChainPart > 0)
    {
        fireChainParts[currentFireChainPart-1]->movePartTo(fireElementalParticle, &currentParticle);
        fireChainParts[currentFireChainPart-1]->activate(false);
        currentFireChainPart--;
        totalFireChainAccumulated--;
    }
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
    float chainDistance = getRadius((positionX+((totalFireChainAccumulated+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.x)-positionX), (positionY+((totalFireChainAccumulated+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.y-positionY)));
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
        chainDistance = getRadius((positionX+((totalFireChainAccumulated-1+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.x)-positionX), (positionY+((totalFireChainAccumulated-1+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.y-positionY)));

    else
        chainDistance = getRadius((positionX+((totalFireChainAccumulated+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.x)-positionX), (positionY+((totalFireChainAccumulated+pushPopDefenceWhenRetracting)*movementRateOfFireChainParts*direction.y-positionY)));

    float mouseDistance = getRadius((positionX-mx), (positionY-my));

    //cout << "\n\nchainDistance: " << chainDistance;
    //cout << "\nmouseDistance: " << mouseDistance;
    //cout << "\nTotalFireChainAccumulated: " << totalFireChainAccumulated;
    //Sleep(100);

    if(chainDistance > (mouseDistance+cursorLeverage))
        return true;

    return false;
}

void fireChain::flowResourceParticles(resourceSystem *rs, int whichNode)
{
    if(delayParticleFlow.isDelayOver())
    {
        if(currentResourceTransitParticle < NumberOfTransitResourceParticlesPerFireChain && rs->getNodePower(whichResourceNodeCollided) > 0)
        {
            rs->tapNode(whichNode, resourceTransitParticles[currentResourceTransitParticle], currentResourceTransitParticle); ///-1 REMOVED FROM 2ND PARAMETER.
            currentResourceTransitParticle++;
        }

        else if(currentResourceTransitParticle >= NumberOfTransitResourceParticlesPerFireChain)
        {
            resourceTransitpartsAreAllFilled = true;
            currentResourceTransitParticle = 0;
        } ///NOT SURE ABOUT FUNCTIONALITY 10% CHANCE THIS WORKS.
    }
}

void fireChain::animation(resourceSystem *RS)
{
    int mxTemp; //Guess this could have been compressed into a function since it's being used frequently. This will be updated in vs 1.1.
    int myTemp;

    float mx;
    float my;

    int lengthOfIteration = 0;

    if(whichResourceNodeCollided != -1)
        if(!RS->IsNodeConnected(whichResourceNodeCollided) && RS->IsNodeLinked(whichResourceNodeCollided)) //Double protection.
        {
            RS->checkToConnectYet(whichResourceNodeCollided, fireChainParts[currentFireChainPart-1]->returnX(), fireChainParts[currentFireChainPart-1]->returnY());
            delayParticleFlow.startClock(); //Just makes it a little more awesome - delays the first flowed particle also.
        }

    if(collisionWithResourceNode && RS->IsNodeConnected(whichResourceNodeCollided))
        flowResourceParticles(RS, whichResourceNodeCollided);

    if(fireChainNodeCreated && !NC.IsNodeConnected() && !collisionWithResourceNode)
        NC.checkToConnectYet(fireChainParts[currentFireChainPart-1]->returnX(), fireChainParts[currentFireChainPart-1]->returnY());

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
    if(collisionWithResourceNode)
    {
        isResourceNodeEmpty(RS); //Checks if this resourceNode needs to be disabbled because of its drained power.
       // cout << RS->getNodePower(whichResourceNodeCollided) << endl;
        if(RS->IsNodeConnected(whichResourceNodeCollided))
        {
            if(currentResourceTransitParticle > 0)
            {
                if(!resourceTransitpartsAreAllFilled)
                    lengthOfIteration = currentResourceTransitParticle;

                else lengthOfIteration = NumberOfTransitResourceParticlesPerFireChain;

                for(int i = 0; i < lengthOfIteration; i++)
                {
                    resourceTransitParticles[i]->position((RS->getNodeXValue(whichResourceNodeCollided)-(resourceTransitParticles[i]->FCSmoothMovementXandY()*direction.x)), (RS->getNodeYValue(whichResourceNodeCollided)-(resourceTransitParticles[i]->FCSmoothMovementXandY()*direction.y)));
                    resourceTransitParticles[i]->move();

                    //checkCollisionWithFireElemental(resourceTransitParticles[i]);
                }
            }
        }
    }

    if(fireChainNodeCreated)
    {
        if(NC.IsNodeConnected())
        {
            if(!fireChainTransitpartsAreAllFilled)
                    lengthOfIteration = currentFireElementalTransitParticle;

                else lengthOfIteration = NumberOfTransitPushedParticlesPerFireChain;

            if(collisionWithNode())
            {
                NC.increaseNodePower(fireElementalTransitParticles[fireElementalTransitParticleCollisionCheck], 0); ///THIS 0 IS LITTERLY HACKING THE FUNCTION, WE NEED TO FIX THIS.

                ///This line is also very risky.
                if(fireElementalTransitParticleCollisionCheck >= NumberOfTransitPushedParticlesPerFireChain)
                    fireElementalTransitParticleCollisionCheck = 0;

                fireElementalTransitParticleCollisionCheck++;
            }

            for(int i = 0; i < lengthOfIteration; i++)
            {
                fireElementalTransitParticles[i]->position((positionX+(fireElementalTransitParticles[i]->FCSmoothMovementXandY()*direction.x)), (positionY+(fireElementalTransitParticles[i]->FCSmoothMovementXandY()*direction.y)));
                fireElementalTransitParticles[i]->move();
            }

            NC.animate();
        }
    }

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
    bool fireElementalOutOfParticles();

    float getPositionX() { return x; }
    float getPositionY() { return y; }

    int mouseCollisionWithWhichFireChainToPushToNode();
    void pushParticlesToCreatedNode(int whichFireChain);

    void startPushingToCreatedNode(bool shouldIStart = true) { mouseDownCanStartPushingToCreatedNode = shouldIStart; }
    bool createdNodeIsBeingPushedTo() { return mouseDownCanStartPushingToCreatedNode; }

    void nodeSelectionToCheck(float mousePositionX, float mousePositionY);

    void passControlGroupKey(int controlGroupKeyPassed);

    void setControlDown(bool setKey) { controlDown = setKey;}
    void setShiftDown(bool setKey) { shiftDown = setKey;}

    void passLeftMouseButtonState(bool setS);
    void castSpellThroughNode(float mouseX, float mouseY);

    float observeIfKillPartsFromNCCollided(float colX, float colY);
protected:
    fireChain *FCs[maxFireChains]; //Anyway too much to actually allocate. Needs to be fixed with a linked list later in vs 1.1.
    particle *fireParts[totalParticlesPerClass];
    particle referencePart;
    fireTrail fireElemFireTrail;
    delaySome delayPushedParticlesToNode;
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

    //We define this here, because if we define it in NC, we have to go through every version each time to set a arbitrary variable, that coud just be passed from fireElemental.
    //It seems, in other words, as redundant variables definitions when shift and control are used in the fireELemental.
    ///If, however, we want different uses for fireElmenetal to NC with shift and control, we need to do the former, but thats the problem then :)
    bool controlDown;
    bool shiftDown;

    bool fireChainsInitiated;
    int currentFireChain;
    int fireChainsActivated;

    bool collisionOnFireElemental(float mx, float my);
    bool collisionWithResourceParts(int, int);

    bool mouseDownCanStartPushingToCreatedNode;

    void lifeIntensity();
    float lifeValue;
};

float fireElemental::observeIfKillPartsFromNCCollided(float colX, float colY)
{
    float totalDamageDoneOnSingleTargetByAllKillParticles = 0;

    for(int i = 0; i < currentFireChain; i++)
        totalDamageDoneOnSingleTargetByAllKillParticles += FCs[i]->checkCollisionWithCreatedNodeKillParts(colX, colY); //Since it will return 0, no check needed.

    return totalDamageDoneOnSingleTargetByAllKillParticles;
}

void fireElemental::passLeftMouseButtonState(bool setS)
{
    for(int i = 0; i < currentFireChain; i++)
    {
        FCs[i]->checkMouseButtonLeftStatus(setS);
    }
}

void fireElemental::castSpellThroughNode(float mx, float my)
{
    for(int i = 0; i < currentFireChain; i++)
    {
        FCs[i]->throwSpellThroughNode(mx, my);
    }
}

void fireElemental::passControlGroupKey(int controlGroupKeyPassed)
{
    for(int i = 0; i < currentFireChain; i++)
    {
        FCs[i]->passControlGroupKeyPressed(controlGroupKeyPassed, controlDown, shiftDown);
    }
}

void fireElemental::nodeSelectionToCheck(float mousePositionX, float mousePositionY)
{
    for(int i = 0; i < currentFireChain; i++)
        FCs[i]->checkifNodeNeedsToSelectYet(mousePositionX, mousePositionY);
}

int fireElemental::mouseCollisionWithWhichFireChainToPushToNode() //M<ouse x and mouse Y
{
    int mouseCordSX;
    int mouseCordSY;

    SDL_GetMouseState(&mouseCordSX,&mouseCordSY);

    mouseCordSX = scrollControl.adjustInputValueX(mouseCordSX);
    mouseCordSY = scrollControl.adjustInputValueY(mouseCordSY);

    for(int i = 0; i < currentFireChain; i++)
    {
        if(FCs[i]->createdItsOwnNode())
        {   //Returns the point where the createdNode will lay.
            if(FCs[i]->getLastParticleCordX() > mouseCordSX-mouseNodePushingLeverage && FCs[i]->getLastParticleCordX() < mouseCordSX+mouseNodePushingLeverage)
                if(FCs[i]->getLastParticleCordY() > mouseCordSY-mouseNodePushingLeverage && FCs[i]->getLastParticleCordY() < mouseCordSY+mouseNodePushingLeverage)
                    return i;
        }
    }

    return -1; // False collision. No collision.
}

void fireElemental::pushParticlesToCreatedNode(int whichFireChain)
{
    if(whichFireChain != -1 )//Check above. It's a false collision statement.
    {
        if(FCs[whichFireChain]->createdItsOwnNode()) //Additional protection. Theoretically will never be called because above will not return that value.
        {
            if(!fireElementalOutOfParticles())
            {
                if(delayPushedParticlesToNode.isDelayOver())
                {
                    FCs[whichFireChain]->collectPartsFromFireElemental(fireParts[currentPart-1], currentPart);
                    currentPart--;
                    partCount--;
                }
            }
        }
    }
}


bool fireElemental::fireElementalOutOfParticles()
{
    if(currentPart <= 0 || partCount <= 0)
        return true;

    else return false;
}

bool fireElemental::collisionWithResourceParts(int givenX, int givenY)
{
    if(givenX > x-(fireElemSize/2*ResourceCollisionMultiplier) && givenX < x+(fireElemSize/2*ResourceCollisionMultiplier))
        if(givenY > y-(fireElemSize/2*ResourceCollisionMultiplier) && givenY < y+(fireElemSize/2*ResourceCollisionMultiplier))
            return true;

    else return false;
}

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
        if(!FCs[i]->isFireChainFinished())
        {
            FCs[i]->setPositions(x,y);

            //if(FCs[i]->isActive())
            //{

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

                        else if(FCs[i]->makeDecisionToPop())
                        {
                            FCs[i]->popPart(fireParts[currentPart], currentPart);
                            partCount++;
                            currentPart++;
                        }
                        //Obviously if non of these functions are called, nothing happens...
                    }

                }

            //}

            if(!FCs[i]->isActive())
            {
                if(FCs[i]->shouldFireChainRetractYet())
                {
                    if(FCs[i]->checkParticleRetractionDelay()) //Retract at the same speed as it extends.
                    {
                        FCs[i]->popPart(fireParts[currentPart], currentPart);
                        FCs[i]->addPushPopDefenceWhenRetracting();
                        partCount++;
                        currentPart++;

                        if(FCs[i]->isFireChainEmpty())
                            FCs[i]->fireChainHasCompletelyRetracted(); //Stops this event from continiously trying to popParts from FCs[i].
                    }
                }
            }

            if(FCs[i]->isFireChainCompletelyEmpty(RS))
                FCs[i]->setFireChainFinished();

            FCs[i]->animation(RS);
        }
    }
}

bool fireElemental::collisionOnFireElemental(float mx, float my)
{
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
    ///Used in the castThroughNode statement, (last line I think of this function)
    ///We can not have mx and my be passed on left click, because it does not register "hold ins", so coords will be off. We need to catch our own consistently.
    int mxTemps;
    int myTemps;

    SDL_GetMouseState(&mxTemps, &myTemps);

    float mouseCoordinatesX = mxTemps;
    float mouseCoordinatesY = myTemps;

    mouseCoordinatesX = scrollControl.adjustInputValueX(mouseCoordinatesX);
    mouseCoordinatesY = scrollControl.adjustInputValueY(mouseCoordinatesY);

    if(true)//scrollControl.inScreen(x,y,(referencePart.returnSize() + (partCount*sizeIncreasePerPart))))
    {
        //lifeIntensity();
        fireElemFireTrail.showFireTrail(x,y);

        for(int i = 0; i < currentFireChain; i++) ///IF CURRENTFIRECHAIN IS SET TO 0 AGAIN, THIS WILL FAIL HORRIBLY.
        {
            if(FCs[i]->collidedWithAResourceNode()) //Just an added layer of protection.
            {
                if(!FCs[i]->allResourceParticlesCollected(RS))
                {
                    if(collisionWithResourceParts(FCs[i]->getResourceParticlesX(RS), FCs[i]->getResourceParticlesY(RS)))
                        {
                            //Print(300,200,1,"%f",FCs[i]->getResourceParticlesX());
                            //Print(300,240,1,"%f",FCs[i]->getResourceParticlesY());
                            if(!FCs[i]->allResourceParticlesCollected(RS))
                            {
                                fireParts[currentPart] = new particle;
                                FCs[i]->passResourceParticle(fireParts[currentPart], currentPart, RS);
                                currentPart++;
                                partCount++;
                            }
                        }
                }
            }
        }

        for(int i = 0; i < (int)(partCount/displayReduction); i++)
        {
            if(fireParts[i]->isActive())
            {
                fireParts[i]->position(x,y);
                fireParts[i]->move();
            //  fireParts[i]->setColor(0.5 + randomize(0.5), 0.5 + randomize(0.5), 0.5 +randomize(0.5));
            //  fireParts[i]->setLife(1);
            }

        absorbParts(RPs);
        }

    }

    if(mouseDownCanStartPushingToCreatedNode)
        pushParticlesToCreatedNode(mouseCollisionWithWhichFireChainToPushToNode());

    fireChainAnimation(RS); //Has it's own inscreen controll.

    castSpellThroughNode(mouseCoordinatesX,mouseCoordinatesY);

    Print(x-PositionLeftObjectsForInfoText-3,y-PositionAboveObjectsForInfoText-10, 0, standardFontSize, true, "{%d}", currentPart);
}

fireElemental::fireElemental()
{
    x = WorldStartingElementalPositionX + fireElementalStartingPositionX;
    y = WorldStartingElementalPositionY + fireElementalStartingPositionY;

    movementSpeed = StartingMoveSpeedOFFireElemental;
    sizeIncreasePerPart = sizeIncreasedPerPart;

    delayPushedParticlesToNode.setDelayMili(1000/howManyParticlesShouldBePushedPerSecond);
    delayPushedParticlesToNode.startClock();

    ACX = false;
    ACY = false;

    multiplierX = 0;
    multiplierY = 0;

    controlDown = false;
    shiftDown = false;

    fireChainsActivated = 0;
    currentFireChain = 0;

    fireChainsInitiated = false;

    mouseDownCanStartPushingToCreatedNode = false;

    for(int i = 0; i <= initialStrength; i++)
    {
        currentPart = i;
        partCount = i;

        fireParts[i] = new particle;
        fireParts[i]->activate(true);
        fireParts[i]->position(x,y);
    }

    ///THIS IS EXTRODANARILY INEFFECTIVE. THIS, HOWEVER, CAN BE FIXED IN THE NEXT PATCH. FOR NOW IT WORKS AND WORKING IS WHAT IM INTERESTED IN.
    ///JUST BUILD WHERE FIRECHAIN POPS A FEATURE WHERE IT CHECKS IF A NEW PART NEEDS TO BE DEFINED. NOT THAT EASY, BECAUSE REMEMBERING WHERE YOU LEFT OFF WITH YOUR NEW PARTICLE ASSIGNEMENT
    ///IS NECCESARY, BUT CERTAINLY NOT DIFFICULT. THIS IS SIMPLY A VERY MINOR PROBLEM, AND CONSEQUENTLY SOMETHING ONLY DESIGN RELATED.
    for(int i = initialStrength+1; i <= totalParticlesPerClass; i++)
    {
        fireParts[i] = new particle;
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

class powerElemental {
public:
    powerElemental();
    void animate();
    float passX() { return x;}
    float passY() { return y;}
    bool stillAlive() { if(currentPowerParts >= 0) return true; else return false; } ///NOTE THE EQUAL SIGN HERE.
    void reduceHealth(float HPreduction);
protected:
    particle *powerParts[maxPowerParts];
    particle *killedParts[maxKilledParts];
    explosiveIgnition explosiveHit;
    void lifeIntensity();
    float x;
    float y;
    bool maxkilledPartsReached;
    int currentPowerParts;
    int currentKilledParts;
};

void powerElemental::reduceHealth(float HPreduction)
{
    int hackingMovePartTo = 0;
    float particleSpeedSpreadX = 0;
    float particleSpeedSpreadY = 0;
    maxkilledPartsReached = false;

    if(currentPowerParts > 0)
    {
        while(HPreduction >= powerParts[currentPowerParts-1]->getHealth())
        {
            if(currentPowerParts > 0)
            {
                explosiveHit.setPosition(x,y);
                explosiveHit.explosiveLaunch();

                HPreduction -= powerParts[currentPowerParts-1]->getHealth();

                particleSpeedSpreadX = (rand()%intensityOfParticleSpeedSpread) - intensityOfParticleSpeedSpread/2;
                particleSpeedSpreadX *= ParticleSpeedVariableDistributionDeviation;

                particleSpeedSpreadY = (rand()%intensityOfParticleSpeedSpread) - intensityOfParticleSpeedSpread/2;
                particleSpeedSpreadY *= ParticleSpeedVariableDistributionDeviation;

                killedParts[currentKilledParts] = new particle;
                powerParts[currentPowerParts-1]->movePartTo(killedParts[currentKilledParts], &hackingMovePartTo);
                powerParts[currentPowerParts-1]->activate(false);

                killedParts[currentKilledParts]->multiplyRotation(100);
                killedParts[currentKilledParts]->setTexture(16);
                killedParts[currentKilledParts]->setXspeed(particleSpeedSpreadX);
                killedParts[currentKilledParts]->setYspeed(particleSpeedSpreadY);

                currentKilledParts++;
                currentPowerParts--;

                if(currentKilledParts > maxKilledParts)
                {
                    maxkilledPartsReached = true;
                    currentKilledParts = 0;
                }
            }
        }

        powerParts[currentPowerParts-1]->modifyHealthBy(-HPreduction);
    }
}

void powerElemental::lifeIntensity()
{
    float lifeValue = (float)1/(currentPowerParts/lifeDiminishingFactor);

    for(int i = 0; i < currentPowerParts; i++)
    {
        if(powerParts[i]->isActive())
        {
            powerParts[i]->setLife(lifeValue);
            powerParts[i]->setSize(powerElemSize);
        }
    }
}

powerElemental::powerElemental()
{
    currentPowerParts = maxPowerParts;
    currentKilledParts = 0;

    x = powerElementalStartPositionX;
    y = powerElementalStartPositionY;

    for(int i = 0; i < maxPowerParts; i++)
    {
        powerParts[i] = new particle;

        powerParts[i]->setTexture(16);
        powerParts[i]->setSize(powerElemSize);
        powerParts[i]->activate(true);
        powerParts[i]->position(x,y);
    }

    explosiveHit.setExplosionSize(45);
    explosiveHit.setExplosionColor(0.0,0.5,1.0);
   // explosiveHit.setLightDissipation(0.05);
    lifeIntensity();
}

void powerElemental::animate()
{
    int killedPartsinterationValue;

    for(int i = 0; i < (int)(currentPowerParts/displayReduction); i++)
        powerParts[i]->move();

    if(maxkilledPartsReached)
        killedPartsinterationValue = maxKilledParts;

    else killedPartsinterationValue = currentKilledParts;

    for(int i = 0; i < killedPartsinterationValue; i++)
        killedParts[i]->move();

    explosiveHit.showExplosion();
    Print(x-PositionLeftObjectsForInfoText,y-PositionAboveObjectsForInfoText, 0, standardFontSize, true, "{%d}", currentPowerParts);
}

class preGameSystem {
public:
    preGameSystem();
    void setIntroToPlay(bool SIP = true) { shouldIntroPlay = SIP; }
    bool preGameSystemStillInEffect() { return preGameSystemActivated; }
    void animate(fade *);
protected:
    particle *introParticles[IntroParticleSaturation]; //Very similar to how a game world will look
    particle *gameName[gameNameLength]; //Amount of letters to spel elmenetalReach.
    particle *ambientEffectOnGameName[ambientEffectAmount];
    delaySome visibilityDelay;

    delaySome tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded;

    bool shouldIntroPlay;
    float visibilityEffect;
    bool preGameSystemActivated;
};

preGameSystem::preGameSystem()
{
    shouldIntroPlay = true;
    #ifdef REMOVE_INTRO_FOR_DEBUGGING //Used define, because it stands out of the other ifs.
    preGameSystemActivated = true;
    #endif
    visibilityEffect = 0;
    visibilityDelay.setDelay(0.00001/rateOfVisiblityIntroPlay);
    //visibilityDelay.setDelayAsEventBased(true);
    visibilityDelay.startClock(); //0.001/rateOfVisiblityIntroPlay
   // visibilityDelay.setLoopEvent();

    tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded.setDelay(13); //After 10 sec it will see it as though this event should start ending.
    tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded.startClock();
    tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded.setDelayAsEventBased();
    tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded.setLoopEvent();

    float tempSpeedDistrX;
    float tempSpeedDistrY;
    float tempFadeRandom;

    for(int i = 0; i < IntroParticleSaturation; i++)
    {
        introParticles[i] = new particle;
        introParticles[i]->activate(true);
        introParticles[i]->position(rand()%worldCordsX,rand()%worldCordsY); //Simple as that;

        tempSpeedDistrX = rand()%maxSpeedIntroParticlesCanMove - maxSpeedIntroParticlesCanMove/2;
        tempSpeedDistrY = rand()%maxSpeedIntroParticlesCanMove - maxSpeedIntroParticlesCanMove/2;

        introParticles[i]->setXspeed(speedDistributionOfIntroParts/tempSpeedDistrX);
        introParticles[i]->setYspeed(speedDistributionOfIntroParts/tempSpeedDistrY);

        introParticles[i]->setLife(StartingVisiblityOfIntro);
    }

    for(int i = 0; i < gameNameLength; i++)
    {
        gameName[i] = new particle;
        gameName[i]->activate(true);

        if(i < 9)
            gameName[i]->position(SW/4+i*LetterDistanceAtGameNamePrint,SH/6); //Simple as that;
        else
            gameName[i]->position(SW/4+(i-7)*LetterDistanceAtGameNamePrint,SH/3);

        gameName[i]->setSize(TS*2);
        gameName[i]->setToNoRotation();
        gameName[i]->setLife(StartingVisiblityOfIntro);
        tempFadeRandom = rand()%maxSpeedOfFadeInGameNameParts;

        gameName[i]->setFade(-(speedDistributionOfFadingInGameNameParts/tempFadeRandom)); // - fade will act like a recoveryLife system.
    }
    ///This will be fixed in a future patch. Please bear with me.
    gameName[0]->setTexture(4);
    gameName[1]->setTexture(5);
    gameName[2]->setTexture(4);
    gameName[3]->setTexture(6);
    gameName[4]->setTexture(4);
    gameName[5]->setTexture(7);
    gameName[6]->setTexture(9);
    gameName[7]->setTexture(8);
    gameName[8]->setTexture(5);
    gameName[9]->setTexture(10);
    gameName[10]->setTexture(4);
    gameName[11]->setTexture(8);
    gameName[12]->setTexture(11);
    gameName[13]->setTexture(12);

}

void preGameSystem::animate(fade *FS) //Could also have declared a fade in the protected variables.
{
    if(shouldIntroPlay)
    {
        for(int i = 0; i < IntroParticleSaturation; i++)
        {
            introParticles[i]->setLife(visibilityEffect);
            introParticles[i]->move();
        }

        for(int i = 0; i < gameNameLength; i++)
        {
            //Keeps the Name centreScreen;
          //  if(i < 9)
          //      gameName[i]->position(scrollControl.drawInRelationToScreenX(scrollControl.adjustInputValueX(SW/4+i*LetterDistanceAtGameNamePrint),scrollControl.drawInRelationToScreenY(scrollControl.adjustInputValueY(SH/6))); //Simple as that;
          //  else
          //      gameName[i]->position(scrollControl.drawInRelationToScreenX(scrollControl.adjustInputValueX(SW/4+(i-7)*LetterDistanceAtGameNamePrint),scrollControl.drawInRelationToScreenY(scrollControl.adjustInputValueY(SH/3)));

            //gameName[i]->setLife(visibilityEffect);
            gameName[i]->move();
        }
    }
    if(visibilityEffect < 1)
        if(visibilityDelay.isDelayOver())
        {
            //visibilityDelay.setLoopEvent(true);
            //visibilityDelay.setDelay(0.001/rateOfVisiblityIntroPlay);
            visibilityEffect += rateOfVisiblityIntroPlay;
        }

    if(tempDelayForPreviewOnlyRemoveWhenMenuSystemIsAdded.isDelayOver())
    {
        FS->out();
        if(FS->fadedOut())
            preGameSystemActivated = false;
    }
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
    case SDLK_LCTRL: case SDLK_RCTRL:
            FE->setControlDown(true);
        break;
        ///CURRENTLY THERE IS NO NEED FOR SHIFT TO BE PASSED, BUT IS KEPT IF NEEDED IN THE FUTURE.
    case SDLK_LSHIFT: case SDLK_RSHIFT:
            FE->setShiftDown(true);
        break;

    case SDLK_0:
            FE->passControlGroupKey(0);
        break;
    case SDLK_1:
            FE->passControlGroupKey(1);
        break;
    case SDLK_2:
            FE->passControlGroupKey(2);
        break;
    case SDLK_3:
            FE->passControlGroupKey(3);
        break;
    case SDLK_4:
            FE->passControlGroupKey(4);
        break;
    case SDLK_5:
            FE->passControlGroupKey(5);
        break;
    case SDLK_6:
            FE->passControlGroupKey(6);
        break;
    case SDLK_7:
            FE->passControlGroupKey(7);
        break;
    case SDLK_8:
            FE->passControlGroupKey(8);
        break;
    case SDLK_9:
            FE->passControlGroupKey(9);
        break;

	default:
	    break;
	}

    switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
	    /* ESC key was pressed */
	    Quit(0);
	    break;
    case SDLK_SPACE:
        Quit(0);
        break;
	case SDLK_F1:
	    /* F1 key was pressed
	     * this toggles fullscreen mode
	     */
	    SDL_WM_ToggleFullScreen( surface );
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
            case SDLK_LCTRL: case SDLK_RCTRL:
                    FE->setControlDown(false);
                break;
            case SDLK_LSHIFT: case SDLK_RSHIFT:
                    FE->setShiftDown(false);
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

    mx = scrollControl.adjustInputValueX(mx);
    my = scrollControl.adjustInputValueY(my);

    if(eventx->button.button == SDL_BUTTON_LEFT)
    {
        FE->fireChainConstruction(mx,my);
        FE->passLeftMouseButtonState(true); ///THE POSITION OF THIS LINE RELATIVE TO THE CASTSPELLTHROUGHNODE ONE, DOES MATTER! IF BELOW, WE PROBABLY HAVE A LITTLE DELAY ON CASTING(GUESS).
        FE->nodeSelectionToCheck(mx, my);
    }

    if(eventx->button.button == SDL_BUTTON_RIGHT)
    {
        FE->startPushingToCreatedNode();
    }
}

void mouseUp(SDL_Event *eventx, fireElemental *FE, resourceSystem *RS)
{
    FE->fireChainFinished(RS);
    FE->passLeftMouseButtonState(false);

    if(FE->createdNodeIsBeingPushedTo()) // I guess this is not neccesary, but it looks neat.
    {
        FE->startPushingToCreatedNode(false);
    }
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
    void controlGame(resourceSystem *rs, fireElemental *fe, rogueParts *rp, animateCursor ac, powerElemental *pe);
private:
};

void gameController::controlGame(resourceSystem *rs, fireElemental *fe, rogueParts *rp, animateCursor ac, powerElemental *pe)
{
    fe->updates();

    fe->animation(rp, rs);

    rp->animation();
    rs->animates();

    if(pe->stillAlive())
        pe->reduceHealth(fe->observeIfKillPartsFromNCCollided(pe->passX(),pe->passY()));

    pe->animate();

    if(scrollControl.CV.isFullyOutCinematicView())
        ac.cursorMove(); //So that it doesn't get blended by anything to become invisible;
}

int main (int argc, char** argv)
{
    srand(time(NULL));
    limitFrameRates LFR;

    animateCursor AC;
    resourceSystem RS;

    gameController GC;
    preGameSystem PGS;

    fade fadeSystem;

    delaySome testDelay;

    testDelay.setDelay(20);
    testDelay.startClock();
    testDelay.setDelayAsEventBased(true);

    delaySome DS;
/*
    notificationSystem NS(true, 6, 6);
    NS.setPosition(SW/4, SH/2);
    NS.addLine("Developer's preview");
    NS.addLine("Version 0.1");
    NS.addLine("Only minor parts of the game");
    NS.addLine("Are being displayed.");

    notificationSystem NS1(true, 30, 8);
    NS1.setPosition(SW/4, SH/2);
    NS1.addLine("Welcome to Elemental Reach");
    NS1.addLine("Home to all the existing elements");

    notificationSystem NS2(true, 44, 8);
    NS2.setPosition(SW/5+50, SH/2+20);
    NS2.addLine("In this preview we take a look at");
    NS2.addLine("the basic mechanics around the");
    NS2.addLine("Fire Elemental");

    notificationSystem NS3(true, 58, 8);
    NS3.setPosition(SW/5-20, SH/2-10);
    NS3.addLine("The Fire Elemental is one of a chaotic nature");
    NS3.addLine("It searches to consume all and grow in power");
    NS3.addLine("Unleashing its fury when opposed");

    notificationSystem NS4(true, 72, 12);
    NS4.setPosition(SW/5-100, SH/2+30);
    NS4.addLine("The Fire Elemental wields its own life force");
    NS4.addLine("consisting of fire particles");
    NS4.addLine("to construct fire chains to absorb and consume");
    NS4.addLine("It is also used to created nodes from which it attacks");

    notificationSystem NS5(true, 140, 8);
    NS5.setPosition(worldCordsX+SW, SH/2+40);
    NS5.addLine("The scrolling system enables for a large world");
    NS5.addLine("Providing for large battles and more game dynamic");

    notificationSystem NS6(true, 154, 10);
    NS6.setPosition(SW/5-120, SH/2+5);
    NS6.addLine("Elemental Reach story will be revealed upon release.");
    NS6.addLine("Elements:air and power, will be introduced.");
    NS6.addLine("All elements will feature unique playstyles");
    NS6.addLine("an exciting Elemental Mastery system enables");
    NS6.addLine("completely different playstyles even in elements");
    NS6.addLine("Fire Elementals that does not need fire chains");
    NS6.addLine("or does not need to harvest");

    notificationSystem NS7(true, 180, 6);
    NS7.setPosition(SW/5-100, SH/2);
    NS7.addLine("This concludes the quick preview of Elemental Reach");
    NS7.addLine("Further development previews will come at a later stage");
    */

    initCoutRedirecting();

    fireElemental *FEx;
    rogueParts *RPx;
    powerElemental *PEx;

    FEx = new fireElemental;
    RPx = new rogueParts;
    PEx = new powerElemental;

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
    videoFlags != SDL_FULLSCREEN;

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
    surface = SDL_SetVideoMode( SW, SH, SBPP, videoFlags);

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

    scrollControl.cinematicViewUpdateCameraMovement();
    //scrollControl.CV.activateCV(true);

    scrollControl.CV.addNewWaypoint(0,500);
    scrollControl.CV.addNewWaypoint(900,0);
    scrollControl.CV.addNewWaypoint(500,400);
   // scrollControl.CV.addNewWaypoint(0,0);

    /* wait for events */
    while ( !done )
	{
	     LFR.limitingFrameRates();
	    /* handle the events in the queue */

	    while ( SDL_PollEvent( &event ) )
		{
		    switch( event.type )
			{
			    if(beforeResizeSW != SW || beforeResizeSH != SH)
                    surface = SDL_SetVideoMode( SW, SH, SBPP, videoFlags ); //SDL_NOFRAME | SDL_FULLSCREEN

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

			    beforeResizeSW = SW;
                beforeResizeSH = SH;

			    SW = event.resize.w;
                SH = event.resize.h;

			    if ( !surface )
				{
				    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
				    Quit( 1 );
				}
                resizeWindow( SW, SH );
			    break;
			case SDL_KEYDOWN:
			    /* handle key presses */
			    handleKeyPress( &event.key.keysym, FEx );
			    break;
            case SDL_KEYUP:
                KeyUpEvents( &event.key.keysym, FEx );
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(!PGS.preGameSystemStillInEffect())
                    mouseEvents(&event, FEx);
                break;
            case SDL_MOUSEBUTTONUP:
                if(!PGS.preGameSystemStillInEffect())
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

            if(!PGS.preGameSystemStillInEffect())
                GC.controlGame(&RS, FEx, RPx, AC, PEx);

            else if(PGS.preGameSystemStillInEffect())
                PGS.animate(&fadeSystem); //Since it uses a global variable it is not neccesary to pass a pointer, but we do anyway.

            if(!PGS.preGameSystemStillInEffect())
                if(!fadeSystem.fadedIn()) //Additional protection.
                    fadeSystem.in();



            /*
            NS.speak();
            NS1.speak();
            NS2.speak();
            NS3.speak();
            NS4.speak();
            NS5.speak();
            NS6.speak();
            NS7.speak();
            NS.speak();
            */

         //       scrollControl.shakeCameraInitiation(30, 20);

            //Print(scrollControl.drawInRelationToScreenX(50), scrollControl.drawInRelationToScreenY(100), 1, "HOLD LEFT MOUSE BUTTON ON FIRE ELEMEMENTAL");
            //Print(scrollControl.drawInRelationToScreenX(50), scrollControl.drawInRelationToScreenY(120), 1, "AND DRAG TO A CERTAIN DESTINATION");
            //Print(scrollControl.drawInRelationToScreenX(50), scrollControl.drawInRelationToScreenY(140), 1, "LET GO OF THE LEFT MOUSE BUTTON :)");

            //LFR.displayRealFramesPerSecond();
            scrollControl.shakeCamera();

            if(testDelay.isDelayOver())
            {
                Sleep(1000);
                scrollControl.CV.addNewWaypoint(1000, 1000);
                scrollControl.CV.addNewWaypoint(200, 1000);
                scrollControl.CV.addNewWaypoint(800, 900);
                scrollControl.CV.addNewWaypoint(0, 0);
            }

            scrollControl.CV.play();
            scrollControl.cinematicViewUpdateCameraMovement();
            //scrollControl.CV.deactivateCVOnDesignationReached(true);

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

float damageReturnValueSoThatKillPartsCanBeDeactivated;

    for(int i = 0; i < currentKillParticle; i++)
    {
        if(killParticles[i]->returnX() > collisionPositionX-killParticlesCollisionLeverage && killParticles[i]->returnX() < collisionPositionX+killParticlesCollisionLeverage)
            if(killParticles[i]->returnY() > collisionPositionY-killParticlesCollisionLeverage && killParticles[i]->returnY() < collisionPositionY+killParticlesCollisionLeverage)
            {

                damageReturnValueSoThatKillPartsCanBeDeactivated = killParticles[i]->getDamage();
                killParticles[i]->activate(false);
                //killParticles[i]->playDeathAnimation(true);

                currentKillParticle--;
            }
    }
    return -1; //False int value;
*/
