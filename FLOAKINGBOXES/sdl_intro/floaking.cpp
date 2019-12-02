#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL2-2.0.10/include/SDL.h"

#pragma comment(lib,"SDL2-2.0.10/lib/x86/SDL2.lib")
#pragma comment(lib,"SDL2-2.0.10/lib/x86/SDL2main.lib")
#pragma comment(linker,"/subsystem:console")

#pragma warning(disable:4996)

SDL_Renderer* renderer = NULL;
struct TwoVector
{
	float x, y;
};

struct Square
{
	TwoVector v, p;
	float w, h;
};

int square_size = 5;
int screen_width = 800;
int screen_height = 600;

int collide(Square* a, Square* b)
{
	if (a->p.x + a->w < b->p.x) return 0;
	if (a->p.x > b->p.x + b->w) return 0;
	if (a->p.y + a->h < b->p.y) return 0;
	if (a->p.y > b->p.y + b->h) return 0;
	return 1;
}

TwoVector Vector_Add(TwoVector v1, TwoVector v2)
{
	TwoVector v;

	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	//v.z = v1.z + v2.z;

	return v;
}

TwoVector Vector_Sub(TwoVector v1, TwoVector v2)
{
	TwoVector v;

	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	//v.z = v1.z - v2.z;

	return v;
}

double Abs_value(TwoVector v1)
{
	TwoVector v;
	v.x = v1.x*v1.x;
	v.y = v1.y*v1.y;
	double v2 = v.x + v.y;
	return sqrt(v2);
	
}

TwoVector Vector_Div(TwoVector v1, int A)
{
	TwoVector v;

	v.x = v1.x / A;
	v.y = v1.y / A;
	//v.z = v1.z / A;

	return v;
}

float Distance(TwoVector a, TwoVector b)
{
	float Distance = 0;
	 Distance = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
	 return Distance;

}

TwoVector Normalize(TwoVector a)
{
	float N = 0;
	N = sqrt(a.x*a.x + a.y*a.y);
	TwoVector v = Vector_Div(a, N);
	return v;
}

TwoVector Alignment(Square* b, Square bj, int n)
{

	TwoVector v = { 0 }; // "perceived center of mass"
	int neighborcount = 0;
	for (int i = 0; i < n; i++)
	{
		if (b[i].p.x != bj.p.x && b[i].p.y != bj.p.y)
		{
			if (Distance(b[i].p, bj.p) < 10)
			{
				v = Vector_Add(v, bj.v);
				neighborcount++;
			}
		}
	
		
	}
	if (neighborcount == 0) { return v; }

	v = Vector_Div(v, neighborcount);
	v = Normalize(v);

	return v;
}

TwoVector Cohesion(Square* b, Square bj, int n)
{

	TwoVector v = { 0 }; // "perceived center of mass"
	int neighborcount = 0;
	for (int i = 0; i < n; i++)
	{
		if (b[i].p.x != bj.p.x && b[i].p.y != bj.p.y)
		{
			if (Distance(b[i].p, bj.p) < 10)
			{
				v = Vector_Add(v, bj.p);
				neighborcount++;
			}
		}


	}
	if (neighborcount == 0)return v;

	v = Vector_Div(v, neighborcount);
	v = Vector_Sub(v, bj.p);
	v = Normalize(v);

	return v;
}
TwoVector Separation(Square* b, Square bj, int n)
{
	TwoVector v = { 0 }; // "perceived center of mass"
	int neighborcount = 0;
	for (int i = 0; i < n; i++)
	{
		if (b[i].p.x != bj.p.x && b[i].p.y != bj.p.y)
		{
			if (Distance(b[i].p, bj.p) < 10)
			{
				v = Vector_Add(v, Vector_Sub(bj.p, b[i].p));
				neighborcount++;
			}
		}


	}
	if (neighborcount == 0)return v;

	v = Vector_Div(v, neighborcount);
	v.x *= -1;
	v.y *= -1;
	v = Normalize(v);

	return v;

}





int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	srand(time(0));

// 	int screen_width = 800;
// 	int screen_height = 600;
	SDL_Window* window = SDL_CreateWindow("Floaking", 100, 100, screen_width, screen_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	const int max_n_squares = 500;
	static Square b[max_n_squares];
	
	float temperature = 1.0;
	float decay = 0.9995;

	
	float movement_amount = 400.0f;

	for (int i = 0; i < max_n_squares; ++i)
	{
		b[i].p.x = rand() % (screen_width - square_size);
		b[i].p.y = rand() % (screen_height - square_size);
		b[i].v.x = -1 + rand() % 5;
		b[i].v.y = -1 + rand() % 5;
		b[i].h = square_size;
		b[i].w = square_size;
		
	}

	int train = 1;

	
	for (;;)
	{
		for (int ii = 0; ii < max_n_squares; ii++)
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
			int Xmin = 0, Xmax = 800, Ymin = 0, Ymax = 600;


			

			TwoVector v1, v2, v3;

			v1 = Alignment(b, b[ii], max_n_squares);
 			v2 = Cohesion(b, b[ii], max_n_squares);
			v3 = Separation(b, b[ii], max_n_squares);
			
			
			b[ii].v = Vector_Add(b[ii].v, Vector_Add(v3, Vector_Add(v1, v2)));
			b[ii].p = Vector_Add(b[ii].p, b[ii].v);

			

// 			b[ii].v = Vector_Add(b[ii].v, v1);
// 			b[ii].p = Vector_Add(b[ii].p, b[ii].v);

			if (b[ii].p.x < Xmin) b[ii].v.x *= -1;
			else if (b[ii].p.x > Xmax) b[ii].v.x *= 1;
			if (b[ii].p.y < Ymin) b[ii].v.y *= -1;
			else if (b[ii].p.y > Ymax) b[ii].v.y *= 1;

			

			/*
			draw
			clear screen once
			*/
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);



			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			for (int i = 0; i < max_n_squares; ++i)
			{
				SDL_Rect rect = { b[i].p.x, b[i].p.y, square_size, square_size };
				SDL_RenderFillRect(renderer, &rect);
			}


			/*
			once per frame!
			*/
			SDL_RenderPresent(renderer);

		}
	}




	getchar();


	return 0;
}