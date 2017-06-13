#define _USE_MATH_DEFINES
#include <math.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FOV = 64;

int map[10][10] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
	{ 1, 1, 1, 1, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 2, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 0, 1, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 1, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

double sinValues[5400], cosValues[5400];
bool upArrowDown = false;
bool downArrowDown = false;
bool leftArrowDown = false;
bool rightArrowDown = false;
bool run = true;
ALLEGRO_EVENT_QUEUE* event_queue;

void initializeSinCosValues()
{
	for (int i = 0; i < 5400; i++)
	{
		sinValues[i] = sin(i * M_PI / 2700.0);
		cosValues[i] = cos(i * M_PI / 2700.0);
	}
}

void getPlayerCoordinates(int* px, int* py)
{
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (map[y][x] == 2)
			{
				map[y][x] = 0;
				*px = x * 1000;
				*py = y * 1000;
				return;
			}
		}
	}
}

bool init()
{
	return (al_init() && al_install_keyboard() && al_init_primitives_addon());
}

void testKeyboard(int* px, int* py, double* angle)
{
	ALLEGRO_EVENT ev;
	al_wait_for_event(event_queue, &ev);
	if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (ev.keyboard.keycode == ALLEGRO_KEY_UP) upArrowDown = true;
		if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) downArrowDown = true;
		if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) leftArrowDown = true;
		if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) rightArrowDown = true;
		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) run = false;
	}
	else if (ev.type == ALLEGRO_EVENT_KEY_UP)
	{
		if (ev.keyboard.keycode == ALLEGRO_KEY_UP) upArrowDown = false;
		if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) downArrowDown = false;
		if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) leftArrowDown = false;
		if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) rightArrowDown = false;
	}

	if (upArrowDown)
	{
		if (map[(int)((*py - cosValues[(int)*angle] * 50) / 1000.0)][(int)((*px - sinValues[(int)*angle] * 50) / 1000.0)] != 1)
		{
			*px -= sinValues[(int)*angle] * 50;
			*py -= cosValues[(int)*angle] * 50;
		}
	}

	if (downArrowDown)
	{
		if (map[(int)((*py + cosValues[(int)*angle] * 50) / 1000.0)][(int)((*px + sinValues[(int)*angle] * 50) / 1000.0)] != 1)
		{
			*px += sinValues[(int)*angle] * 50;
			*py += cosValues[(int)*angle] * 50;
		}
	}

	if (leftArrowDown) *angle -= 5.0 * 15.0;
	if (rightArrowDown) *angle += 5.0 * 15.0;
	if (*angle < 0.0) *angle += 5400;
	if (*angle > 5499.0) *angle -= 5400;
}

void castRays(int px, int py, double angle)
{
	ALLEGRO_COLOR skyColor = al_map_rgb(60, 177, 240);
	ALLEGRO_COLOR wallColor = al_map_rgb(148, 0, 0);
	ALLEGRO_COLOR floorColor = al_map_rgb(0, 170, 43);

	int step = -1;
	for (double i = -(FOV / 2 - 1) * 15; i <= (FOV / 2) * 15; i++)
	{
		step++;
		double a = angle + i;
		if (a < 0.0) a += 5400.0;
		if (a > 5399.0) a -= 5400.0;
		double x = px;
		double y = py;
		int raylen = 0;
		int m = 0;
		do
		{
			x = x - sinValues[(int)a];
			y = y - cosValues[(int)a];
			m = map[(int)(y / 1000.0)][(int)(x / 1000.0)];
			raylen++;
		} while (m != 1);
		int size = 100000 / raylen;

		al_draw_filled_rectangle(step, 0, step + 1, SCREEN_HEIGHT / 2 - 1 - size, skyColor);
		al_draw_filled_rectangle(step, SCREEN_HEIGHT / 2 - size, step + 1, SCREEN_HEIGHT / 2 + size, wallColor);
		al_draw_filled_rectangle(step, SCREEN_HEIGHT / 2 + 1 + size, step + 1, SCREEN_HEIGHT - 1, floorColor);
	}

	al_flip_display();
}

int main()
{
	if (!init()) return -1;

	ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

	initializeSinCosValues();

	int px, py;
	getPlayerCoordinates(&px, &py);

	double angle = 0.0;

	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	while (run)
	{
		castRays(px, py, angle);

		testKeyboard(&px, &py, &angle);
	}

	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}