#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL2-2.0.10/include/SDL.h"

#pragma comment(lib,"SDL2-2.0.10/lib/x86/SDL2.lib")
#pragma comment(lib,"SDL2-2.0.10/lib/x86/SDL2main.lib")
#pragma comment(linker,"/subsystem:console")

#pragma warning(disable:4996)

SDL_Renderer* renderer = NULL;

struct Square
{
	float x, y;
	float w, h;
};

int Collision_Check(Square* a, Square* b)
{
	if (a->x + a->w < b->x) return 0;
	if (a->x > b->x + b->w) return 0;
	if (a->y + a->h < b->y) return 0;
	if (a->y > b->y + b->h) return 0;
	return 1;
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	srand(time(0));

	int screen_width = 800;
	int screen_height = 600;
	SDL_Window* window = SDL_CreateWindow("SIMULATED ANNEALING", 100, 100, screen_width, screen_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	const int nBigSquares = 5;
	Square BigSquares[nBigSquares];
	
// 	for (int i = 0; i < nBigSquares; i++)
// 	{
// 		BigSquares[i].w = 100 + rand() % screen_width/2;
// 		BigSquares[i].h = 100 + rand() % screen_height/2;
// 		BigSquares[i].x = rand() % screen_width - BigSquares[i].w;
// 		BigSquares[i].y = rand() % screen_height - BigSquares[i].h;
// 	}
	
	const int nSquares = 100;
	Square Squares[nSquares];
	Square BestSquares[nSquares];
	for (int i = 0; i < nSquares; i++)
	{
		Squares[i].w = 16;
		Squares[i].h = 16;
		Squares[i].x = rand() % screen_width - Squares[i].w;
		Squares[i].y = rand() % screen_height - Squares[i].h;
		BestSquares[i] = Squares[i];
	}

	float BestCollision = 1000000;
	int SquaresMoving = 2;
	float movement = 8.0;
	float temperature = 1.0;


	for (;;)
	{
		/*
		consume events
		*/
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				printf("GOODBYE\n");
				exit(0);
			}
		}

		/*
		game code here
		*/
		
		
		int nCollisions = 0;
		for (int i = 0; i < nSquares; i++)
		{
			for (int j = i + 1; j < nSquares; j++)
			{
				int c = Collision_Check(&Squares[i], &Squares[j]);
				if (c==1) nCollisions++;
			}
		}
		
		int nOutofScreen = 0;
		for (int i = 0; i < nSquares; i++)
		{
			if (Squares[i].x < 0 || Squares[i].x + Squares[i].w > screen_width) nOutofScreen++;
			if (Squares[i].y < 0 || Squares[i].y + Squares[i].h > screen_height) nOutofScreen++;
		}


		nCollisions += nOutofScreen;
		if (nCollisions > 0)
		{
			for (int i = 0; i < SquaresMoving; i++)
			{
				int k = rand() % nSquares;
				Squares[k].x += movement * (1.0f - 2.0f*rand() / RAND_MAX);
				Squares[k].y += movement * (1.0f - 2.0f*rand() / RAND_MAX);
			}

			if (nCollisions < BestCollision)
			{
				BestCollision = nCollisions;
				for (int i = 0; i < nSquares; i++) BestSquares[i] = Squares[i];
			}
			else
			{
				float p = (float)rand() / RAND_MAX;
				float z = exp((nCollisions - BestCollision) / temperature);
				if (p <= z)
				{
					BestCollision = nCollisions;
					for (int i = 0; i < nSquares; i++) BestSquares[i] = Squares[i];
				}
				else
				{
					nCollisions = BestCollision;
					for (int i = 0; i < nSquares; i++) Squares[i] = BestSquares[i];
				}
			}
		}
		else if (nCollisions == 0)
		{ 
		for (int i = 0; i < nSquares; i++) Squares[i] = Squares[i];
		}
		temperature += 0.9995;
		printf("%d %d %d/n", BestCollision, nCollisions, nOutofScreen);
		/*
		draw
		clear screen once
		*/
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int i = 0; i < nBigSquares; i++)
		{
			{
				SDL_Rect rect = { BigSquares[i].x, BigSquares[i].y, BigSquares[i].w, BigSquares[i].h };
				SDL_RenderFillRect(renderer, &rect);
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		for (int i = 0; i < nSquares; i++)
		{
			{
				SDL_Rect rect = { Squares[i].x, Squares[i].y, Squares[i].w, Squares[i].h };
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		/*
		once per frame!
		*/
		SDL_RenderPresent(renderer);

	}








	return 0;
}