#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "Lanes.h"
#include <vector>

Lanes* Gallery;
int nlanes;
using namespace std;


//std::mutex shooterLock


void ShooterAction(int rate, Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     while(1) {

          //std::lock(shooter_lock)
          //shooterLock.lock()
          int lanenum = Gallery->Count();
          int randLane = (rand() % (lanenum));
          int color = Gallery->Get(randLane);
          if (color == white) {
               Gallery->Set(randLane,PlayerColor);
          }
          //shooterLock.unlock()
          sleep(rate);
          //need ending condition
     }
     //shooterLock.unlock()
     
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

   while(1)
   {
       sleep(rate);
       Gallery->Print();

   }

}



int main(int argc, char** argv)
{
    int numlanes = 5;
    int redShotsPerSec = -1;
    int blueShotsPerSec = -1;
    int numRounds = 1;
    int redRate;
    int blueRate;
    // get args from argv for redrate, bluerate, numRounds, lanes
    if (redShotsPerSec <= 0) {
         redRate = 1;    
    }
    else {
         redRate = (int) (1000.0/(double) redShotsPerSec);
    }
    if (blueShotsPerSec <= 0) {
         blueRate = 1;
    }
    else {
         blueRate = (int) (1000.0/(double) blueShotsPerSec);
    }

    Gallery = new Lanes(numlanes);
    cout<<"making threads\n";
    //    std::thread RedShooterT,BlueShooterT,CleanerT,PrinterT;
    std::thread CleanerT(&Cleaner);
    std::thread PrinterT(&Printer, 10);
    std::thread RedShooterT(&ShooterAction,redRate,red);
    std::thread BlueShooterT(&ShooterAction,blueRate, blue);

    cout<<"threads made\n";
    sleep(20);

    // Join with threads
    cout<<"joining threads\n";
    RedShooterT.join();
    BlueShooterT.join();
    CleanerT.join();
    PrinterT.join();


    return 0;
}
