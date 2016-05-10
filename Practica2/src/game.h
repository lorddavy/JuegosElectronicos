/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "entity.h"
#include "vehicle.h"

class Game
{
public:
	
	static Game* getInstance(SDL_Window* window = NULL) {
		if (instance == NULL)
			instance = new Game(window);
		return instance;
	};

	//Entidad del jugador
	Vehicle* player;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;
    
    float time;

	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame
	bool mouse_locked; //tells if the mouse is locked (not seen)
	
	Camera* camera; //our global camera
	int cameraType; //Camera type 0->free 1->player
	//Camera* current_camera;
	//Camera* free_camera;
	//Camera* player_camera;

	Game(SDL_Window* window);
	void init( void );
	void render( void );
	void update( double dt );

	void onKeyPressed( SDL_KeyboardEvent event );
	void onMouseButton( SDL_MouseButtonEvent event );
    void onResize( SDL_Event e );
    
	void setWindowSize(int width, int height);

	Entity* createEntity(const char* name); //FACTORY

private:
	static Game* instance;

};


#endif 