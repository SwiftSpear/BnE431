#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "Lanes.h"
#include <vector>
#include <mutex>
#include <immintrin.h>


Lanes* Gallery;
int nlanes;
int coloredLanes = 0;
//std::mutex shooterLock;
using namespace std;
mutex shooterLock;




void ShooterAction(int rate, Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     
       int lock;



while (__atomic_exchange_n(&lock, 1, __ATOMIC_ACQUIRE|__ATOMIC_HLE_ACQUIRE) != 0) {


    _mm_pause();

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

 }


     
         __atomic_store_n(&lock, 0, __ATOMIC_RELEASE|__ATOMIC_HLE_RELEASE);
          
          sleep(rate);
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
     






     int lanenum = Gallery->Count();
     while(coloredLanes != lanenum){
      //cout << "coloredLanes " << coloredLanes << endl;
     }

  
     shooterLock.lock();
    if (coloredLanes == lanenum){
      Gallery->Print();
      Gallery->Clear();
      
     }
  shooterLock.unlock();

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
    std::thread RedShooterT(&ShooterAction,redRate,red);
    std::thread BlueShooterT(&ShooterAction,blueRate, blue);
    std::thread CleanerT(&Cleaner);
    std::thread PrinterT(&Printer, 1);
    

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