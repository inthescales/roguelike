#ifndef FEATURE_H

#define FEATURE_H

class feature {

	public:
	
	int status;
	int type;
	
	feature(short);
	void on_open();
	void on_close();

};

#endif
