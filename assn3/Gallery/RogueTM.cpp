#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "Lanes.h"
#include <vector>
#include <mutex>
#include <immintrin.h>

#include <time.h>
#include <getopt.h>

Lanes* Gallery;
int nlanes;
int coloredLanes = 0;
//std::mutex shooterLock;
using namespace std;





void ShooterAction(int rate, Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     
       int lock;



while (__atomic_exchange_n(&lock, 1, __ATOMIC_ACQUIRE|__ATOMIC_HLE_ACQUIRE) != 0) {

  int val;
 

  /* Wait for lock to become free again before retrying. */
  do {

    _mm_pause();

    /* Abort speculation */

  } while (val == 1);


  int lanenum = Gallery->Count();
     //cout << "lanenum = " << lanenum << endl;
     while(coloredLanes != lanenum) {
          bool cleaner = true;
          
          int randLane = (rand() % (lanenum));
          int color = Gallery->Get(randLane);
          if (color == white) {
               Gallery->Set(randLane,PlayerColor);
               ++coloredLanes; 
               //cout<<"coloredLanes " << coloredLanes << endl;
          }
          
          for (int i =0; i< lanenum; i++){
            if (Gallery->Get(i) == white)
            {
              cleaner = false;
              break;
            }
          }

          if(cleaner)
          {
            //Gallery->Print();
            //cout << "Cleaning" << endl;
            Gallery->Clear();

            }

 }



     
         __atomic_store_n(&lock, 0, __ATOMIC_RELEASE|__ATOMIC_HLE_RELEASE);
          
          usleep(rate);
          //need ending condition
     
     //shooterLock.unlock()
     
}
}

void Cleaner() {

    /**
     *  Cleans up lanes. Needs to synchronize with shooter.
     *  Use a monitor
     *  Should be in action only when all lanes are shot.
     *  Once all lanes are shot. Cleaner starts up.
     *  Once cleaner starts up shooters wait for cleaner to finish.
     */

}


void Printer(int rate) {

    /**
     *  NOT TRANSACTION SAFE; cannot be called inside a transaction. Possible conflict with other Txs Performs I/O
     *  Print lanes periodically rate/s.
     *  If it doesn't synchronize then possible mutual inconsistency between adjacent lanes.
     *  Not a particular concern if we don't shoot two lanes at the same time.
     *
     */
   int lanenum = Gallery->Count();
   while(lanenum != coloredLanes)
   {
       usleep(rate);
       Gallery->Print();
   }

}


static struct option long_options[] =
  {
    {"redrate", required_argument, 0, 'r'},
    {"bluerate", required_argument, 0, 'b'},
    {"rounds", required_argument, 0, 'n'},
    {"lanes", required_argument, 0, 'l'},
    {"printoff", no_argument, 0, 'p'},
    {0, 0, 0, 0}
  };


int main(int argc, char** argv)
{
    int numlanes = 5;
    int redShotsPerSec = -1;
    int blueShotsPerSec = -1;
    int numRounds = 1;
    bool enablePrint = true;
    int redRate;
    int blueRate;
    double elapsed;
    clock_t start, end;

    // get args from argv for redrate, bluerate, numRounds, lanes
    while (true) {
        int option_index = 0;
        int c = getopt_long_only(argc, argv, "r:b:n:l:p",
                                 long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1)
          break;

        switch (c) {
        case 0:
          /* If this option set a flag, do nothing else now. */
          break;

        case 'r':
          redShotsPerSec = atoi(optarg);
          break;

        case 'b':
          blueShotsPerSec = atoi(optarg);
          break;

        case 'n':
          numRounds = atoi(optarg);
          break;

        case 'l':
          numlanes = atoi(optarg);
          break;

        case 'p':
          enablePrint = false;
          break;

        case '?':
          /* getopt_long already printed an error message. */
          exit(1);
          break;

        default:
          exit(1);
        }

    }
    if (redShotsPerSec <= 0) {
         redRate = 1;
    }
    else {
         redRate = (int) (1000000.0/(double) redShotsPerSec);
    }
    if (blueShotsPerSec <= 0) {
         blueRate = 1;
    }
    else {
         blueRate = (int) (1000000.0/(double) blueShotsPerSec);
    }
    Gallery = new Lanes(numlanes);

    start = clock();
    std::thread CleanerT(&Cleaner);
    std::thread PrinterT;
    if (enablePrint) {
        PrinterT = std::thread(&Printer, 1000);}
    std::thread RedShooterT(&ShooterAction,redRate,red);
    std::thread BlueShooterT(&ShooterAction,blueRate, blue);


    // Join with threads
    RedShooterT.join();
    BlueShooterT.join();
    CleanerT.join();
    if (enablePrint) {
        PrinterT.join();}
    end = clock();
    elapsed = (double) (end - start)/(CLOCKS_PER_SEC/1000);
    cout<<"Elapsed time: "<<elapsed<<" ms\n";
    return 0;
}
