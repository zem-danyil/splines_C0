``Manual build for Windows``
g++ src/main.cpp -o build/main.exe -I C:\raylib\include -L C:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17 -Wall -Wextra
build/main.exe
