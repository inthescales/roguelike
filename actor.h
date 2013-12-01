#ifndef ACTOR_H

#define ACTOR_H

#include "enums.h"
#include "condition.h"

#include <curses.h>
#include <stdlib.h>
#include <string>
#include <utility> //pair
#include <vector>

using std::string;

class object;
class tile;

class actor {

	public:

	// basics
	string name; //special name
	short x, y; //position
	int type; //actor class
	condition * conditions;

	// stats
	short HP, maxHP;
	short level;
	
	// inventory
	std::vector<object*> inventory;
	object * equipped_item[ES_MAX];
	long gold; //TODO - change this to stacked items list eventually?
	
	//ai
	int aitype;
	int state;

	// Functions:
	
	actor(short code);
	chtype get_img();
	string get_name();
	
	void move(std::pair<int,int>);
	void attack(std::pair<int,int>);
	void attack(actor *);
	void get_item(object *);
	bool remove_object(object * item);
	
	bool pick_up(object *, tile *);
	bool drop(object *, tile*);
	bool equip(object *, int);
	bool unequip(object *);
	bool eat(object *);
	bool drink(object *);
	bool read(object *);
	bool use(object *);
	//void open(int, int);
	//void close(int, int);
	
	void print(string, string);
	//void transition(stair);
	//void switch_map(map, map, int, int );
};

#endif
