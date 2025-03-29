// visualization.h
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "object_module.h"
#include <SDL.h>
#include <SDL_ttf.h>

void render_bar_chart(SDL_Renderer *renderer, TTF_Font *font,
                      const ObjectArray *arr, int field);
void render_pie_chart(SDL_Renderer *renderer, TTF_Font *font,
                      const ObjectArray *arr, int field);

#endif
