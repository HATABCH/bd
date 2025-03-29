#include "visualisation.h"
#include "object_module.h"
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MARGIN 20
#define LEGEND_WIDTH 200
#define CHART_AREA_WIDTH (WINDOW_WIDTH - LEGEND_WIDTH - 3 * MARGIN)
#define CHART_AREA_HEIGHT (WINDOW_HEIGHT - 2 * MARGIN)

static const SDL_Color colors[] = {
    {255, 0, 0, 255},   // Красный
    {0, 255, 0, 255},   // Зеленый
    {0, 0, 255, 255},   // Синий
    {255, 255, 0, 255}, // Желтый
    {255, 0, 255, 255}, // Пурпурный
    {0, 255, 255, 255}  // Голубой
};
#define NUM_COLORS 6

static double get_max_value(const ObjectArray *arr, int field) {
  double max_val = 0;
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    if (val > max_val)
      max_val = val;
  }
  return max_val;
}

static void render_text(SDL_Renderer *renderer, TTF_Font *font, int x, int y,
                        const char *text, SDL_Color color) {
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect rect = {x, y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &rect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

static void draw_legend(SDL_Renderer *renderer, TTF_Font *font,
                        const ObjectArray *arr, int field) {
  SDL_Rect legend_rect = {WINDOW_WIDTH - LEGEND_WIDTH - MARGIN, MARGIN,
                          LEGEND_WIDTH, WINDOW_HEIGHT - 2 * MARGIN};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(renderer, &legend_rect);

  for (int i = 0; i < arr->size; i++) {
    SDL_Color c = colors[i % NUM_COLORS];
    SDL_Rect color_rect = {legend_rect.x + 10, legend_rect.y + 10 + i * 30, 20,
                           20};

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &color_rect);

    char buffer[100];
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    snprintf(buffer, sizeof(buffer), "%s: %.2f", arr->objects[i].name, val);

    render_text(renderer, font, color_rect.x + 25, color_rect.y, buffer,
                (SDL_Color){255, 255, 255, 255});
  }
}

void render_bar_chart(SDL_Renderer *renderer, TTF_Font *font,
                      const ObjectArray *arr, int field) {
  if (!arr || arr->size == 0)
    return;

  double max_val = get_max_value(arr, field);
  if (max_val <= 0)
    return;

  SDL_Rect chart_area = {MARGIN, MARGIN, CHART_AREA_WIDTH, CHART_AREA_HEIGHT};

  // Отрисовка столбцов
  int bar_width = chart_area.w / arr->size;
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    int bar_height = (int)((val / max_val) * chart_area.h);

    SDL_Rect bar = {chart_area.x + i * bar_width,
                    chart_area.y + chart_area.h - bar_height, bar_width - 2,
                    bar_height};

    SDL_Color c = colors[i % NUM_COLORS];
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &bar);
  }

  draw_legend(renderer, font, arr, field);
}

void render_pie_chart(SDL_Renderer *renderer, TTF_Font *font,
                      const ObjectArray *arr, int field) {
  if (!arr || arr->size == 0)
    return;

  // Рассчет общего значения
  double total = 0;
  for (int i = 0; i < arr->size; i++) {
    total += (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
  }
  if (total <= 0)
    return;

  // Параметры круга
  int center_x = WINDOW_WIDTH / 2;
  int center_y = WINDOW_HEIGHT / 2;
  int radius = fmin(WINDOW_WIDTH, WINDOW_HEIGHT) / 2 - MARGIN;

  // Отрисовка секторов
  double start_angle = -90.0; // Начинаем сверху
  for (int i = 0; i < arr->size; i++) {
    double val = (field == 1) ? arr->objects[i].age : arr->objects[i].weight;
    double sweep_angle = 360.0 * (val / total);

    SDL_Color c = colors[i % NUM_COLORS];
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

    // Отрисовка сектора (примитивная реализация)
    for (double angle = start_angle; angle < start_angle + sweep_angle;
         angle += 0.1) {
      double rad = angle * M_PI / 180.0;
      SDL_RenderDrawLine(renderer, center_x, center_y,
                         center_x + radius * cos(rad),
                         center_y + radius * sin(rad));
    }

    start_angle += sweep_angle;
  }

  // Отрисовка легенды
  draw_legend(renderer, font, arr, field);
}
