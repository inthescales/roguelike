#ifndef ACTOR_H

#define ACTOR_H

#include "enums.h"

#include <curses.h>
#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

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
	object * equipped_item[ES_MAX];
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
	bool remove_object(object * item);
	
	bool pick_up(object *, tile *);
	bool drop(object *, tile*);
	bool equip(object *, int);
	bool unequip(object *);
	//void eat(object);
	//void drink(object);
	void open(int, int);
	void close(int, int);
	//void use(object, int, int);
	//void equip(object);
	//void unequip(object);
	//void transition(stair);
	//void switch_map(map, map, int, int );
};

#endif
