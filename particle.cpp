#include "entity.h"

#include <iostream>
#include <math.h>

/*
Particle::Particle(int x, int y, int lifespan, int imageID, int subimage){
	this->x = x;
	this->y = y;
	this->maxAge = lifespan;
	this->imageID = imageID;
	this->baseSubimage = subimage;
	this->subimage = subimage;
}
*/

Particle::Particle(int x, int y, int vx, int vy, int imageID, int subimage){
	this->x = x;
	this->y = y;
	this->vx = vx;
	this->vy = vy;
	this->imageID = imageID;
	this->baseSubimage = subimage;
	this->subimage = subimage;
}

void Particle::update(){
	++this->age;
	if (maxAge > 0){
		if (this->age > maxAge){
			Entity::removeEntity(this);
			return;
		}
	}else{
		if(this->age > animationSpeed){ //age needs a limiter to prevent overflow, and animationSpeed is already used to determine when the animation should change
			this->age = 0;
		}
	}
	
	x += vx;
	y += vy;
	
	updatePhysics();
	
	if(this->age % animationSpeed == 0){
		subimage = baseSubimage + (rand() % numberSubimages);
	}
	
	return;
}

void Particle::setLifespan(int lifespan){
	maxAge = lifespan;
	
	return;
}

void Particle::updatePhysics(){
	const int FLOAT_SPEED = 5, FLOAT_BUFFER = 8, FLOAT_SPEED_VARIATION = 3;
	
	//Update velocity based on gravity
	if(age < BURST_SPEED_DECAY_LENGTH){
		//Quickly move outward after creation, decaying in speed.
		vx >= 0 ? sqrt(vx) : sqrt(abs(vx)) * -1;
		vy >= 0 ? sqrt(vy) : sqrt(abs(vy)) * -1;
	}else if(age < BURST_SPEED_DECAY_LENGTH + FLOAT_BUFFER){
		//suspend the particles briefly before floating downward
		vx = 0;
		vy = 0;
	}else{
		//Allow a slow float downward
		vx = 0;
		vy = floor((float)(age % (FLOAT_SPEED + (rand() % FLOAT_SPEED_VARIATION))) / (float)(FLOAT_SPEED - 1));
	}
	
	return;
}