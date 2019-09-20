#include <cstdlib>
#include <cmath>
#include <iostream>

#include "entity.h"

Emitter::Emitter(int x, int y, int vx, int vy, int imageID, int subimage){
	this->x = x;
	this->y = y;
	this->vx = vx;
	this->vy = vy;
	this->imageID = imageID;
	this->baseSubimage = subimage;
	this->subimage = subimage;
	
}

void Emitter::update(){
	//std::cout << "emitter updating" << std::endl;
	++this->age;
	if (maxAge > 0){
		if (this->age > maxAge){
			//remove the entity
			emit();
			Entity::removeEntity(this);
			//return;
		}
	}else{
		if(this->age > animationSpeed){ //age needs a limiter to prevent overflow, and animationSpeed is already used to determine when the animation should change
			this->age = 0;
		}
	}
	
	x += vx;
	y += vy;
	
	if(this->age % animationSpeed == 0){
		subimage = baseSubimage + (rand() % numberSubimages);
	}
	
	return;
}

void Emitter::emit(){
	(*emitFunction)(this);
	
	return;
}

int Emitter::getX(){
	return x;
}

int Emitter::getY(){
	return y;
}

void Emitter::setLifespan(int lifespan){
	this->maxAge = lifespan;
	
	return;
}

void Emitter::setEmitFunction(void (*emitFunction)(Emitter *source)){
	this->emitFunction = emitFunction;
	
	return;
}

void emitCircle(Emitter *source){
	//std::cout << "emitting firework" << std::endl;
	const int SPREAD = 15, MIN_SPREAD = 1, MIN_PARTICLES = 40, MAX_PARTICLES = 50, LIFESPAN_VARIATION = 12;
	int x = source->getX(), y = source->getY(), lifespan = (rand() % (2 * 60)) + (2 * 60);
	
	for(int i = (rand() % (MAX_PARTICLES - MIN_PARTICLES) + MIN_PARTICLES); i > 0; --i){
		//std::cout << "particle " << i << std::endl;
		int distance = (rand() % (SPREAD - MIN_SPREAD) + MIN_SPREAD), theta = (rand() % 360);
		//Entity::makeEntity(EntityType::ET_Particle, x + (distance * cos(theta)), y + (distance * sin(theta)), 2 * 60, 0, 0);
		Entity *newParticle = Entity::makeEntity(EntityType::ET_Particle, x, y, (distance * cos(theta)), (distance * sin(theta)), 0, rand() % 2);
		//std::cout << "entity pushed" << std::endl;
		newParticle->setLifespan(lifespan + (rand() % LIFESPAN_VARIATION));
	}
	
	return;
}