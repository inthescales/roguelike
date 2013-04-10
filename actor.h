#ifndef ACTOR_H

#define ACTOR_H

#include <curses.h>
#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

//#include "object.h"
//#include "map.h"
//#include "stair.h"
//#include "tile.h"

class object;
class tile;

class actor {

	public:

	// basics
	std::string name; //special name
	short x, y; //position
	int type; //actor class

	// stats
	short HP, maxHP;
	short level;
	
	// inventory
	std::vector<object*> inventory;
	//std::map<int, object> equip;
	long gold;
	
	//ai
	int aitype;
	int state;

	// Functions:
	
	actor(short code);
	chtype get_img();
	std::string get_name();
	
	void move(std::pair<int,int>);
	void attack(std::pair<int,int>);
	void attack(actor *);
	void get_item(object *);
	//void drop(object);
	//void eat(object);
	//void drink(object);
	void open(int, int);
	void close(int, int);
	bool pick_up(object *, tile *);
	//void use(object, int, int);
	//void equip(object);
	//void unequip(object);
	//void transition(stair);
	//void switch_map(map, map, int, int );
};

#endif
