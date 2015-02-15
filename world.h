#ifndef WORLD_H
#define WORLD_H

class map;

class world {

    public:
    static void pond(map *, int, int, int);
    static void forest(map *, int, int, int);
    static void hut(map*, int, int, int);

};

#endif