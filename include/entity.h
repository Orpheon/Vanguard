#ifndef ENTITY_H
#define ENTITY_H

#include "engine.h"
#include <list>

class Entity
{
	public:
		Entity(std::list<Entity*> *gameentities);
		~Entity();

		void beginstep(Engine engine, double frametime);
		void midstep(Engine engine, double frametime);
		void endstep(Engine engine, double frametime);
		void render();
	protected:
	private:
};

#endif // ENTITY_H
