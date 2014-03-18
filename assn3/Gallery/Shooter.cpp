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
std::mutex shooterLock


void ShooterAction(int rate,Color PlayerColor) {

    /**
     *  Needs synchronization. Between Red and Blue shooters.
     *  Choose a random lane and shoot.
     *  Rate: Choose a random lane every 1/rate s.
     *  PlayerColor : Red/Blue.
     */
     //std::lock(shooter_lock)
     //shooterLock.lock()
     int randLane = (rand() % (rate-1))
     int color = Gallery->Get(randLane);
     if (color == white) {
          Gallery->Set(randLane,PlayerColor);
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
       sleep(1);
       Gallery->Print();
       cout<<Gallery->Count();

   }

}



int main(int argc, char** argv)
{
    int numlanes = 5;
    std::vector<thread> ths;


    Gallery = new Lanes(numlanes);
    //    std::thread RedShooterT,BlueShooterT,CleanerT,PrinterT;



    ths.push_back(std::thread(&ShooterAction,numlanes,red));
    ths.push_back(std::thread(&ShooterAction,numlanes,blue));
    ths.push_back(std::thread(&Cleaner));
    ths.push_back(std::thread(&Printer,numlanes));


    // Join with threads
    //    RedShooterT.join();
    //  BlueShooterT.join();
    //  CleanerT.join();
    // PrinterT.join();


    for (auto& th : ths) {

        th.join();

    }


    return 0;
}
