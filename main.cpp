#include <cstdio>
#include <sys/time.h>
#include <allegro5/allegro.h>

#include "include/engine.h"

long int getmillisec();

int main(int argc, char **argv)
{
	ALLEGRO_DISPLAY *display = NULL;
	const double framelimit = 60;

	if(!al_init())
	{
	  fprintf(stderr, "failed to initialize allegro!\n");
	  return -1;
	}

	display = al_create_display(640, 480);
	if(!display)
	{
	  fprintf(stderr, "failed to create display!\n");
	  return -1;
	}

	al_clear_to_color(al_map_rgb(0,0,0));

	al_flip_display();

    Engine* engine = new Engine();
	long int lasttimeupdated = getmillisec();

	while (true)
	{
		long int current_time = getmillisec();
		double millisecperframe = 1000.0 / framelimit;
		int timediff = current_time - lasttimeupdated;
		while (timediff - millisecperframe >= 0)
		{
			engine->run();
			lasttimeupdated += millisecperframe;
			timediff -= millisecperframe;
		}
	}

	al_destroy_display(display);

	return 0;
}

long int getmillisec()
{
    static struct timeval tp;
    gettimeofday(&tp, 0);
    return ((long) tp.tv_sec) * 1000 + ((long) tp.tv_usec) / 1000;
}
