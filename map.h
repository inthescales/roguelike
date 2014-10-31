#ifndef MAP_H

#define MAP_H

#include <vector>

using std::vector;

class actor;
class object;
class tile;
class timer;

class map {

    private:
    
    vector<timer *> * timer_list;
    
    int event_progress;

	public:

	map(short, short, int, map *);
    
    // The map's contents and structures
	int id;
	short width, height;
	std::vector<std::vector<tile> > tiles;
    //std::vector<stair> stairs;
	//struct roomNote * note;
    
    // Populate map
	void add_stairs( struct feature *, int, int);
	void add_actor(int, int, short);
	void put_actor(int, int, struct actor *);
	void add_object(int, int, short);
	void put_object(int, int, object *);
	void add_feature( int, int, short);
    void put_feature(int, int, feature *);
	
    // Handle timing of events
    void add_timer(timer *);
    void advance_time();
    int time_until_event();
    
};

#endif

/*
	When I'm creating a room, sometimes rooms will want to propagate
	to the next floor down. This stores those reminders until the
	map is actually generated.
	
	SAVE THIS FOR LATER

struct roomNote {
	int x, y;
	int index, type;
	struct roomNote * next;
};
*/
