# We define the includes which are not specific to the project to avoid compilation errors
# Wall means show warnings
g++ -std=c++14 -Wall -s -I include -I "./include" -c classes/*.h
g++ -std=c++14 -Wall -s -I include -I "./include" -c classes/*.cpp
g++ -std=c++14 -Wall -s -I include -I "./include" -c *.cpp

# We define the result of the build + obj files to link + the libraries included in / lib
g++ -std=c++14 -L "./lib" -o "./bin/afchat-client" *.o -s -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -ltgui -ltgui-s

# starts the application
./bin/afchat-client