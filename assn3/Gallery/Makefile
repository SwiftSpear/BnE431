
all: Shooter 

Shooter: Shooter.o 
	g++ -mrtm -std=c++11 -pthread $< -o $@

Shooter.o: Shooter.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

fine: RogueFine

RogueFine: RogueFine.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueFine.o: RogueFine.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

fine2: RogueFine2

RogueFine2: RogueFine2.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueFine2.o: RogueFine2.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

finecleaner: RogueFineCleaner

RogueFineCleaner: RogueFineCleaner.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueFineCleaner.o: RogueFineCleaner.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

course2: RogueCourse2

RogueCourse2: RogueCourse2.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueCourse2.o: RogueCourse2.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

coursecleaner: RogueCourseCleaner

RogueCourseCleaner: RogueCourseCleaner.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueCourseCleaner.o: RogueCourseCleaner.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

tm: RogueTM

RogueTM: RogueTM.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueTM.o: RogueTM.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

tm2: RogueTM2

RogueTM2: RogueTM2.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueTM2.o: RogueTM2.cpp
	g++ -c -mrtm -std=c++11 -pthread $<

tmcleaner: RogueTMCleaner

RogueTMCleaner: RogueTMCleaner.o
	g++ -mrtm -std=c++11 -pthread $< -o $@

RogueTMCleaner.o: RogueTMCleaner.cpp
	g++ -c -mrtm -std=c++11 -pthread $<


clean:
	rm -rf *o Shooter RogueFine RogueFine2 RogueFineCleaner RogueCourse2 RogueCourseCleaner RogueTM RogueTM2 RogueTMCleaner
