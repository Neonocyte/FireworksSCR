#ifndef ENTITYBASE
#define ENTITYBASE

#include <vector>
#include <algorithm>
#include <iostream>

enum class EntityType{
	ET_Unknown = 0,
	ET_Emitter,
	ET_Particle,
	ET_BurstEM
};

class Emitter;
class Particle;

void emitCircle(Emitter *source);

class Entity{
	public:
		static std::vector<Entity *> entityVector;
		
		template <typename ...Args>
		static Entity *makeEntity(EntityType type, Args... args);
		
		static void removeEntity(Entity *entity){
			removalVector.push_back(entity);
			//entityVector.erase(std::remove(entityVector.begin(), entityVector.end(), entity), entityVector.end());
		}
		
		static void executeRemovals(){
			for (Entity *removed : removalVector){
				entityVector.erase(std::remove(entityVector.begin(), entityVector.end(), removed), entityVector.end());
				delete removed;
			}
			removalVector.clear();
		}
	
		virtual void update(){};
		virtual void setLifespan(int lifespan){maxAge = lifespan;};
		virtual int getImageID(){return imageID;};
		virtual int getSubimage(){return subimage;};
		virtual int getX(){return x;};
		virtual int getY(){return y;};

	protected:
		int x, y, imageID, subimage = 0, age = 0, maxAge = -1;
	private:
		const static int MIN_BURST_AGE = 2 * 60, MAX_BURST_AGE = 3 * 60;
		static std::vector<Entity *> removalVector;
};

class Emitter : public Entity{
	public:
		Emitter(int x, int y, int vx, int vy, int imageID, int subimage);
		void update() override;
		void emit();
		void setLifespan(int lifespan);
		void setEmitFunction(void (*emitFunction)(Emitter *source));
		int getX();
		int getY();
	private:
		int numberSubimages = 4, animationSpeed = 10;
		int vx, vy, baseSubimage, emitRadius;
		void (*emitFunction)(Emitter *source) = emitCircle;
};

class Particle : public Entity{
	public:
		//Particle(int x, int y, int lifespan, int imageID, int subimage);
		Particle(int x, int y, int vx, int vy, int imageID, int subimage);
		//Particle(int a, int b, int c, int d, int e, int f){}; //Dummy to placate template compile errors. Should not be called ever.
		void update() override;
		void setLifespan(int lifespan);
	private:
		const int BURST_SPEED_DECAY_LENGTH = 5; //Specific to this fireworks example; remove and modify updatePhysics() for more general use.
	
		int numberSubimages = 2, animationSpeed = 30;
		int vx, vy, baseSubimage;
		
		void updatePhysics();
};



template <typename ...Args>
Entity *Entity::makeEntity(EntityType type, Args... args){
	Entity *newEntity = nullptr;
	switch(type){
		case EntityType::ET_Emitter:
			newEntity = new Emitter(std::forward<Args>(args)...);
			//entityVector.push_back(new Emitter(std::forward<Args>(args)...));
			break;
		case EntityType::ET_Particle:
			{
					newEntity = new Particle(std::forward<Args>(args)...);
					//entityVector.push_back(new Particle(std::forward<Args>(args)...));
					//std::cout << "new particle made" << std::endl;
			}
			break;
		case EntityType::ET_BurstEM:
			{
				Emitter *newEmitter = new Emitter(std::forward<Args>(args)...);
				newEmitter->setLifespan(rand() % (MAX_BURST_AGE - MIN_BURST_AGE) + MIN_BURST_AGE);
				newEmitter->setEmitFunction(emitCircle);
				newEntity = newEmitter;
			}
			break;
		case EntityType::ET_Unknown:
		default:
			return nullptr;
	}
	if(newEntity != nullptr){
		entityVector.push_back(newEntity);
	}
	return newEntity;
}

#endif