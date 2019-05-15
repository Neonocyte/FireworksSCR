#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <string>

#include "entity.h"
#include "Images/fireworksV2_8x8.xpm"

const int BASE_SCREEN_WIDTH = 640, BASE_SCREEN_HEIGHT = 480, MS_PER_FRAME = 15, MAX_TEXTURES = 1, SUBIMAGE_SIZE = 8;
int screenWidth = BASE_SCREEN_WIDTH, screenHeight = BASE_SCREEN_HEIGHT;

SDL_Window* mainWindow = NULL;
SDL_Surface* mainSurface = NULL;
SDL_Renderer* mainRenderer = nullptr;
SDL_Event mainEvent;

SDL_Texture* loadedTextures[MAX_TEXTURES];

bool running = true;

int init(int argc, char** argv);
void close();
void initLoad();
void update();
void spawnFireworks();
void draw();
SDL_Texture* loadTexture(std::string path);

#if DEBUG == 1
void initTest();
#endif

int main(int argc, char** argv){
	if(init(argc, argv)){
		//Get rid of lingering events that could exit the program early
		while(SDL_PollEvent(&mainEvent) != 0){}
		
		while(running){
			auto start = std::chrono::high_resolution_clock::now();
			const Uint8* curKeyStates;
			
			//Check for events that could exit the program
			while(SDL_PollEvent(&mainEvent) != 0){
				if(mainEvent.type == SDL_QUIT or mainEvent.type == SDL_KEYDOWN or mainEvent.type == SDL_MOUSEMOTION){
					running = false;
				}
			}
			curKeyStates = SDL_GetKeyboardState(NULL);
			if(curKeyStates[SDL_SCANCODE_ESCAPE]){
				running = false;
			}
			
			update();
			draw();
			
			
			
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::milli> loopDuration = end - start;
			
			int delay = MS_PER_FRAME - (loopDuration.count());
			
			
			if(delay > 0){
				SDL_Delay(MS_PER_FRAME - (loopDuration.count()));
			}else{
				//Slowdown Detection for debugging; delay system occasionally introduces slowdown somehow
				#if DEBUG == 1
				std::cout << "Slowdown Detected" << std::endl;
				#endif
			}
			
		}
	}
	
	close();

	return 0;
}

int init(int argc, char** argv){
	int minWidth = 0, minHeight = 0, maxWidth = 0, maxHeight = 0, numDisplays = 0;
	SDL_DisplayMode *displayMode;
	SDL_Rect *displayBounds;
	
	
	srand(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	
	//Determine how program was launched
	
	if(argc > 1){
		if(argv[1][0] == '-'){
			
			switch(argv[1][1]){
				case 'c':
				case 'C':
					//config mode
					return 0;
					break;
				case 'p':
				case 'P':
					//preview mode
					return 0;
					break;
				case 's':
				case 'S':
				default:
					//screensaver mode
					break;
			}
		}
	}
	
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << "Could not initialize SDL. SDL_Error: " << SDL_GetError() << std::endl;
		
	}else{
		numDisplays = SDL_GetNumVideoDisplays();
		if (numDisplays < 1){
			std::cout << "Could not determine the number of displays. SDL_ERROR: " << SDL_GetError() << std::endl;
		}else{
			std::cout << numDisplays << std::endl;
			
			for(int i = 0; i < numDisplays; ++i){
				if(SDL_GetDisplayBounds(i, displayBounds) == 0){
					std::cout << "x: " << displayBounds->x << std::endl;
					if(maxWidth < displayBounds->x + displayBounds->w){
						maxWidth = displayBounds->x + displayBounds->w;
					}else if(minWidth > displayBounds->x){
						minWidth = displayBounds->x;
					}
					
					if(maxHeight < displayBounds->y + displayBounds->h){
						maxHeight = displayBounds->y + displayBounds->h;
					}else if(minHeight > displayBounds->y){
						minHeight = displayBounds->y;
					}
				}
			}
			
			screenWidth = (abs(minWidth) + maxWidth);
			screenHeight = (abs(minHeight) + maxHeight);
			std::cout << "fullWidth: " << (abs(minWidth) + maxWidth) << std::endl;
			std::cout << "fullHeight: " << (abs(minHeight) + maxHeight) << std::endl;
			
			//Below correctly sets window size across all monitors. Commented out for testing.
			mainWindow = SDL_CreateWindow("", minWidth, minHeight, abs(minWidth) + maxWidth, abs(minHeight) + maxHeight, SDL_WINDOW_BORDERLESS);
				
			//mainWindow = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
			
			if(mainWindow == NULL){
				std::cout << "Could not create a window. SDL_ERROR: " << SDL_GetError() << std::endl;
				
			}else{
				
				//update screen size
				//if(SDL_GetCurrentDisplayMode(0, displayMode) == 0){
					//screenWidth = displayMode->w;
					//screenHeight = displayMode->h;
				//}
				
				mainSurface = SDL_GetWindowSurface(mainWindow);
				SDL_FillRect(mainSurface, NULL, SDL_MapRGB(mainSurface->format, 0xFF, 0xFF, 0xFF));
				SDL_UpdateWindowSurface(mainWindow);
				
				mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
				
				if(mainRenderer == nullptr){
					std::cout << "Renderer could not initialize. SDL Error: " << std::string(SDL_GetError()) << std::endl;
					
				}else{
					SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0x00);
					
					int imgFlags = IMG_INIT_PNG;
					
					if(!(IMG_Init(imgFlags) & imgFlags)){
						std::cout << "SDL_image could not initialize. SDL_image Error: " << IMG_GetError() << std::endl;
					}
				}
				
				SDL_ShowCursor(SDL_DISABLE);
				
			}
		}
	}
	
	initLoad();
	
	#if DEBUG == 1
		SDL_SetWindowFullscreen(mainWindow, 0);
		screenWidth = BASE_SCREEN_WIDTH;
		screenHeight = BASE_SCREEN_HEIGHT;
		initTest();
	#endif
	
	return 1;
}

void close(){
	SDL_DestroyWindow(mainWindow);
	
	SDL_Quit();
	
	return;
}

void initLoad(){
	
	//Convert the xpm image fireworks[] from fireworksV2_8x8.xpm
	SDL_Texture* optimizedTexture = NULL;
	SDL_Surface* loadedSurface = IMG_ReadXPMFromArray(fireworks);
	
	if(loadedSurface == nullptr){
		std::cout << "Could not load images. SDL_image Error: " << IMG_GetError() << std::endl;
	}else{
		optimizedTexture = SDL_CreateTextureFromSurface(mainRenderer, loadedSurface);
		
		if(optimizedTexture == nullptr){
			std::cout << "Could not optimize images. SDL Error: " << SDL_GetError() << std::endl;
		}
	}
	
	SDL_FreeSurface(loadedSurface);
	
	loadedTextures[0] = optimizedTexture;
	
	//Due to the switch to xpm, loadTexture is no longer used.
	//loadedTextures[0] = loadTexture("./images/fireworksV2_8x8.png");
	
	return;
}

void update(){
	int count = 0;
	//std::cout << "entityVector size: " << Entity::entityVector.size() << std::endl;
	for(auto entity : Entity::entityVector){
		//std::cout << "updating entity " << count << " of elements " << Entity::entityVector.size() << std::endl;
		if (Entity::entityVector[count] == nullptr){
			std::cout << "entity " << count << " not found" << std::endl;
		}else{
			entity->update();
			//std::cout << "entity found" << std::endl;
		}
		
		++count;
	}
	
	Entity::executeRemovals();
	
	spawnFireworks();
	
	return;
}

void spawnFireworks(){
	static int msToNextSpawn = 60, framesToNextSpawn = msToNextSpawn / MS_PER_FRAME; //framesToNextSpawn assumes the program runs at MS_PER_FRAME constantly; used so that in the case of slowdown, excessive fireworks are not spawned
	//static auto lastSpawn = std::chrono::high_resolution_clock::now();
	int spawnInterval = 3800, spawnVariation = 1000; //milliseconds
	
	//Alternate implementation, not used due to compounding issues when slowdown occurs.
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<float, std::milli> timeSinceLastSpawn = currentTime - lastSpawn;
	//std::cout << timeSinceLastSpawn.count() << std::endl;
	/*if(msToNextSpawn <= timeSinceLastSpawn.count()){
		lastSpawn = std::chrono::high_resolution_clock::now();
		msToNextSpawn = spawnInterval + (rand() % spawnVariation);
		
		Entity::makeEntity(EntityType::ET_BurstEM, rand() % screenWidth, screenHeight, 0, -2, 0, 10);
		
		//TODO: Fixed? make it so that program does not crash when two ET_BurstEM exist at the same time.
		//Entity::makeEntity(EntityType::ET_BurstEM, rand() % screenWidth, screenHeight, 0, -2, 0, 10);
	}*/
	
	if(framesToNextSpawn <= 0){
		framesToNextSpawn = (spawnInterval + (rand() % spawnVariation)) / MS_PER_FRAME;
		
		Entity::makeEntity(EntityType::ET_BurstEM, rand() % screenWidth, screenHeight, 0, -2, 0, 10);
	}else{
		--framesToNextSpawn;
	}
	
	return;
}

void draw(){	
	SDL_RenderClear(mainRenderer);
	

	
	
	SDL_Rect srcrect, dstrect;
	
	#if DEBUG == 1
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = 40;
	srcrect.h = 40;

	dstrect.x = 30;
	dstrect.y = 30;
	dstrect.w = 40;
	dstrect.h = 40;
	
	SDL_RenderCopy(mainRenderer, loadedTextures[0], &srcrect, &dstrect);
	#endif
	
	//End test code
	
	//TODO:get subimage from entity instead of using test SDL_Rects
	for(auto entity : Entity::entityVector){
		if(entity->getImageID() < MAX_TEXTURES){
			//TODO: remove magic numbers - the 5s; query or store image size?
			srcrect.x = SUBIMAGE_SIZE * (entity->getSubimage() % 5);
			srcrect.y = SUBIMAGE_SIZE * floor(entity->getSubimage() / 5);
			srcrect.w = SUBIMAGE_SIZE;
			srcrect.h = SUBIMAGE_SIZE;
			
			dstrect.x = entity->getX();
			dstrect.y = entity->getY();
			dstrect.w = SUBIMAGE_SIZE;
			dstrect.h = SUBIMAGE_SIZE;
			
			SDL_RenderCopy(mainRenderer, loadedTextures[entity->getImageID()], &srcrect, &dstrect);
		}
	}
	
	SDL_RenderPresent(mainRenderer);
	
	return;
}


SDL_Texture* loadTexture(std::string path){
	//Not used anymore, due to the need to convert images to an xpm format.
	//Loading images from the relative directory does not work when used as a screensaver, as the root directory changes
	
	SDL_Texture* optimizedTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	
	if(loadedSurface == nullptr){
		std::cout << "Could not load image " << path << ". SDL_image Error: " << IMG_GetError() << std::endl;
	}else{
		optimizedTexture = SDL_CreateTextureFromSurface(mainRenderer, loadedSurface);
		
		if(optimizedTexture == nullptr){
			std::cout << "Could not optimize image " << path << ". SDL Error: " << SDL_GetError() << path << std::endl;
		}
	}
	
	SDL_FreeSurface(loadedSurface);
	
	return optimizedTexture;
}

#if DEBUG == 1
void initTest(){
	Entity::makeEntity(EntityType::ET_Emitter, 50, 50, 0, 1, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 3 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 4 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 5 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 6 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 7 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 8 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
	Entity::makeEntity(EntityType::ET_BurstEM, BASE_SCREEN_WIDTH / 9 * 2, BASE_SCREEN_HEIGHT, 0, -2, 0, 10);
}
#endif