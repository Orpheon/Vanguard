#ifndef ENGINE_H
#define ENGINE_H


class Engine
{
    public:
        Engine();
        ~Engine();
        void run();

    private:
        long int getmillisec();

		long int lasttimeupdated;
		double millisecperframe;
};

#endif // ENGINE_H
