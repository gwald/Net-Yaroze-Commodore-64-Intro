//Bar code from Type 79
//http://www.psxdev.net/forum/viewtopic.php?f=69&t=378

    #include <sys/types.h>
    #include <libps.h>
    #include <stdio.h>
    #include <stdlib.h>   //for srand and rand
    #include <string.h>

   // constants for interface
#define PADLup     (1<<12)
#define PADLdown   (1<<14)
#define PADLleft   (1<<15)
#define PADLright  (1<<13)
#define PADRup     (1<< 4)
#define PADRdown   (1<< 6)
#define PADRleft   (1<< 7)
#define PADRright  (1<< 5)
#define PADi       (1<< 9)
#define PADj       (1<<10)
#define PADk       (1<< 8)
#define PADl       (1<< 3)
#define PADm       (1<< 1)
#define PADn       (1<< 2)
#define PADo       (1<< 0)
#define PADh       (1<<11)
#define PADL1      PADn
#define PADL2      PADo
#define PADR1      PADl
#define PADR2      PADm
#define PADstart   PADh
#define PADselect  PADk

//512 ,240, NTSC
//640 ,256 PAL
    #define DISPLAY_MODE  MODE_PAL
    #define SCREEN_WIDTH   512
    #define SCREEN_HEIGHT   256
    #define ORDERING_TABLE_LENGTH 10
    #define MAX_NO_PACKETS  3000

   #define setRGB( xline, xr, xg, xb)  (xline).r = (xr); (xline).b = (xb); (xline).g = (xg)
   #define C64LINES 20

//#define DEBUG 1

#ifdef DEBUG
    #define debug printf
#else
    #define debug //
#endif


#define typeSpeed 25
/*
   from http://www.godot64.de/german/epalet.htm

C64
Color    Red    Green    Blue
black    0x00    0x00    0x00    1
white    0xff    0xff    0xff    2
red    0x88    0x00    0x00    3
cyan    0xaa    0xff    0xee    4
purple    0xcc    0x44    0xcc    5
green    0x00    0xcc    0x55    6
blue    0x00    0x00    0xaa    7
yellow    0xee    0xee    0x77    8
orange    0xdd    0x88    0x55    9
brown    0x66    0x44    0x00    10
l,red    0xff    0x77    0x77    11
d,gray    0x33    0x33    0x33    12
gray    0x77    0x77    0x77    13
lgreen    0xaa    0xff    0x66    14
l,blue    0x00    0x88    0xff    15
l,gray    0xbb    0xbb    0xbb    16

   */

    GsOT OT[2];
    GsOT_TAG  OrderingTable[2][1<<ORDERING_TABLE_LENGTH];
    PACKET GPUOutputPacket[2][(MAX_NO_PACKETS * 24)];

    unsigned char currentBuffer;

    GsBOXF      myLines[C64LINES];
    GsBOXF      fadeBox;

    GsBOXF      c64InBox;
    GsBOXF      c64OutBox;
    int typingCounter;
    int LoadCounter;
     int dispCounter = 1;
    char  loadString[SCREEN_HEIGHT*SCREEN_HEIGHT];


    void initSystem(void);
    void display(void);
    void initBoxes(void);
    void drawBoxes(void);

      void c64Screen(void);



   // low-level pad buffers: never need to touch
volatile u_char *bb0, *bb1;


u_long PadStatus;    // for the pad



   // call once only in program initialisation
void PadInit (void)
{
   GetPadBuf(&bb0, &bb1);
}





   // call once per VSync(0)
   // puts controller pad status into unsigned long integer

   // please refer to the manuals if you want explanation
   // of the internals of this function
u_long PadRead(void)
{
   return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}


    int main()
    {


   PadStatus = 0;   // set up controller pad
   PadInit();

        initSystem();
        debug("initSystem\n");

        initBoxes();
        debug("initBoxes\n");

        FntLoad(960, 256);
        FntOpen(c64InBox.x, c64InBox.y+4, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1024);


        typingCounter=0;
        LoadCounter = 0;


        srand(rand());


        do
        {
            char loadImg[] = "Sony Net Yaroze             .::::.\nCommodore 64 Intro        .::::::::.\nBy Gwald @XPCOIN          :::::::::::\n                          ':::::::::::..\n                           :::::::::::::::'\n                            ':::::::::::.\n                              .::::::::::::'\n                            .:::::::::::...\n                           ::::::::::::::'\n               .:::.       '::::::::''::::,\n             .::::::::.      ':::::'  ':::,\n            .::::':::::::.    :::::    ':::,\n          .:::::' ':::::::::. :::::      '::\n        .:::::'     ':::::::::.:::::      ':\n      .::::''         '::::::::::::::     \n     .::''              '::::::::::::     \n  ..::::                  ':::::::::'       \n..''''':'                    ':::::.'\nASCII Art by LGB from: chris.com";


             debug("LoadCounter %d\n",LoadCounter);

           if(LoadCounter < 200)  //fade and load screen
            {
                c64Screen();



            }
            else if(LoadCounter < 300)
            {
               // drawBoxes();
            GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);
            //GsSortBoxFill(&c64InBox,&OT[currentBuffer],   1);


            }
            else if(LoadCounter < 500)  //loading screen
            {
                c64Screen();
                FntPrint("~cDDF\n  OK\n\n  SEARCHING\n  FOUND PSX GAME!");

            } else if(LoadCounter < 1000)
            {
                GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);
            }




            else if(LoadCounter < 850)
            {
                drawBoxes();


            } else if(LoadCounter < 3000)
            {
                GsSortBoxFill(&c64InBox,&OT[currentBuffer],   4);
                drawBoxes();
                LoadCounter++;
                typingCounter=0;



                if(typingCounter % 2 == 0)
                {
                    dispCounter++;
                   loadImg[dispCounter]=0;


                }

                typingCounter++;
                  setRGB( c64InBox, 0, 0, 0);
                  FntPrint(loadImg);


            }
            else if(LoadCounter < 3200)
            {
                drawBoxes();


            } else if(LoadCounter < 3210)
            {



            }
            else // restart
            {
                LoadCounter =0;
                dispCounter= 0;
                typingCounter=0;
                strcpy(loadString,"  ");
                loadString[0]=0;
                setRGB( c64InBox, 53, 53, 192); // back to blue
             }


            LoadCounter++;
            PadStatus = PadRead();      // read the pad
            display();


        }while(!(PadStatus & PADselect) || !(PadStatus & PADstart));
    }

    void initBoxes(void)
    {
        int i;
        fadeBox.w=SCREEN_WIDTH+1;
        fadeBox.h=SCREEN_HEIGHT;
        fadeBox.x=0;
        fadeBox.y=0;
        fadeBox.attribute = 0;
        //    fadeBox.attribute += (1<<30) + (2<<28);
      //  setRGB( fadeBox, 0x0, 0x0, 0x0);


        c64OutBox.w=SCREEN_WIDTH;
        c64OutBox.h=SCREEN_HEIGHT;
        c64OutBox.x=0;
        c64OutBox.y=0;
        c64OutBox.attribute = 0;
        setRGB( c64OutBox, 136, 136, 214);


        c64InBox.w=SCREEN_WIDTH*.70;
        c64InBox.h=SCREEN_HEIGHT*.70;
        c64InBox.x=(SCREEN_WIDTH-c64InBox.w)/2;
        c64InBox.y=(SCREEN_HEIGHT-c64InBox.h)/2;
        c64InBox.attribute = 0;
        setRGB( c64InBox, 53, 53, 192);



        myLines[0].w=SCREEN_WIDTH+1;
        myLines[0].h=SCREEN_HEIGHT/C64LINES;
        myLines[0].x=0;
        myLines[0].y=0;

        for(i=1; i<C64LINES; i++)
        {
            myLines[i]=myLines[0];
            myLines[i].y=i*myLines[0].h;
        }
    }




    void c64Screen(void)
    {

             debug("typingCounter = %u\n", typingCounter);
            GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);
            GsSortBoxFill(&c64InBox,&OT[currentBuffer],   1);


           //To change the colour of FntPrint's output, insert the sequence "~crgb"
           //in your string, where r, g and b are colour values from '0' to '9'.
              FntPrint("~cDDF      **** COMMODORE 64 BASIC V2 ****\n\n");
              FntPrint("~cDDF     64k RAM SYSTEM 38911 BASIC BYTES FREE\n\n");
              FntPrint("~cDDF  READY.\n ");

                debug("string = %s\n",loadString);


               if(typingCounter == typeSpeed)
               {

                strcpy(loadString,"~cDDF ");
                  strcat(loadString,"L");


               }else if(typingCounter == typeSpeed*2)
               {


                  strcat(loadString,"O");


               }else if(typingCounter == typeSpeed*3)
               {


                  strcat(loadString,"A");


               }else if(typingCounter == typeSpeed*4)
               {


                  strcat(loadString,"D");

               }
               else if(typingCounter == typeSpeed*5)
               {


                  strcat(loadString,"\n");


               }
               else if(typingCounter == typeSpeed*6)
               {

                   strcat(loadString,"  PRESS PLAY ON TAPE");

               }

                typingCounter++;


        FntPrint(loadString);
        debug("string = %s\n",loadString);


/*
            int fadeCounter = 0;
            if(fadeCounter >= 10 )
            {
                 debug("fadeCounter %d\n",fadeCounter);


                //for (tTemp1=0; tTemp1<256; tTemp1++)
                {


                  //  if(rand()%2 == 1) //increment 1/40 time
                    fadeCounter--;

                    fadeBox.attribute = 0;
                    fadeBox.attribute += (1<<30) + (2<<28);
                    setRGB( fadeBox, fadeCounter, fadeCounter, fadeCounter);



                // set the colour of the box to go from transparent to opaque
                GsSortBoxFill(&fadeBox, &OT[currentBuffer],0);
                // (see Lib Ref page 148) Send rectangle to ordering table at
                // front

                }

            }
            */

    }

    void drawBoxes(void)
    {
        int i;



        for(i=0; i<C64LINES; i++)
        {


           switch(rand()%16)
           {


                // white    0xff    0xff    0xff    2
                case 1:
                setRGB(myLines[i], 0xff, 0xff, 0xff);
                break;

                // red    0x88    0x00    0x00    3
                case 2:
                setRGB(myLines[i], 0x88,0,0);
                break;

                // cyan    0xaa    0xff    0xee    4
                case 3:
                setRGB(myLines[i], 0xaa,0xff,0xee);
                break;

                // purple    0xcc    0x44    0xcc    5
                case 4:
                setRGB(myLines[i], 0xcc,0x44,0xcc);
                break;

                // green    0x00    0xcc    0x55    6
                case 5:
                setRGB(myLines[i], 0,0xcc,0x55);
                break;

                // blue    0x00    0x00    0xaa    7
                case 6:
                setRGB(myLines[i], 0,0,0xaa);
                break;

                // yellow    0xee    0xee    0x77    8
                case 7:
                setRGB(myLines[i], 0xee,0xee,0x77);
                break;

                // orange    0xdd    0x88    0x55    9
                case 8:
                setRGB(myLines[i], 0xdd,0x88,0x55);
                break;

                // brown    0x66    0x44    0x00    10
                case 9:
                setRGB(myLines[i], 0x66,0x44,0);
                break;

                // l,red    0xff    0x77    0x77    11
                case 10:
                setRGB(myLines[i], 0xff,0x77,0x77);
                break;

                // d,gray    0x33    0x33    0x33    12
                case 11:
                setRGB(myLines[i], 0x33,0x33,0x33);
                break;

                // gray    0x77    0x77    0x77    13
                case 12:
                setRGB(myLines[i], 0x77,0x77,0x77);
                break;

                // lgreen    0xaa    0xff    0x66    14
                case 13:
                setRGB(myLines[i], 0xaa,0xff,0x66);
                break;

                // l,blue    0x00    0x88    0xff    15
                case 14:
                setRGB(myLines[i], 0,0x88,0xff);
                break;

                // l,gray    0xbb    0xbb    0xbb    16
                case 15:
                setRGB(myLines[i], 0xbb,0xbb,0xbb);
                break;


                // black    0x00    0x00    0x00    1
                default:
                setRGB( myLines[i], 0x0, 0x0, 0x0);
                break;



            }







               GsSortBoxFill(&myLines[i],&OT[currentBuffer],   10);


        }
    }

    void initSystem(void)
    {
        SetVideoMode(DISPLAY_MODE);
        GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT,GsNONINTER|GsOFSGPU,0,0);
        GsDefDispBuff(0,0,0,SCREEN_HEIGHT);

        OT[0].length = ORDERING_TABLE_LENGTH;
        OT[1].length = ORDERING_TABLE_LENGTH;
        OT[0].org=OrderingTable[0];
        OT[1].org=OrderingTable[1];

        GsClearOt(0,0,&OT[0]);
        GsClearOt(0,0,&OT[1]);

        currentBuffer=GsGetActiveBuff();
        GsSetWorkBase((PACKET*)GPUOutputPacket[currentBuffer]);
    }

    void display(void)
    {
        FntFlush(-1);
        DrawSync(0);
        VSync(0);
        GsSwapDispBuff();
        GsSortClear(0,0,0,&OT[currentBuffer]);
        GsDrawOt(&OT[currentBuffer]);
        currentBuffer=GsGetActiveBuff();
        GsSetWorkBase((PACKET*)GPUOutputPacket[currentBuffer]);
        GsClearOt(0, 0, &OT[currentBuffer]);
    }
