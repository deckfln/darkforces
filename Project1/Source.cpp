#include <iostream>

#include "myApp.h"
#include "myDarkForces.h"

int main()
{
	myDarkForces myapp("LearnOpenGL", 640, 400);
	//myApp myapp("LearnOpenGL", 800, 600);

	myapp.run();
	return 0;
}
