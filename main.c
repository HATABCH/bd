#include "object_module.h"
#include "visualisation.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_PATH "arial.ttf" // Путь к файлу шрифта

int main() {
  // Инициализация данных
  ObjectArray *arr = create_object_array(5);
  add_object(arr, "Ibragim", 30, 65);
  add_object(arr, "Ramazan", 20, 60.2);
  add_object(arr, "Dmitriy", 35, 81.1);

  printf("Initial objects:\n");
  print_objects(arr, stdout);

  // Инициализация SDL и TTF
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() == -1) {
    fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  // Загрузка шрифта
  TTF_Font *font = TTF_OpenFont(FONT_PATH, 16);
  if (!font) {
    fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Создание окна
  SDL_Window *window = SDL_CreateWindow(
      "Data Visualization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Создание рендерера
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Основной цикл приложения
  int quit = 0;
  int chart_type = 0; // 0 - bar, 1 - pie
  int field = 1;      // 1 - age, 2 - weight

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }

      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_1:
          chart_type = 0;
          break;
        case SDLK_2:
          chart_type = 1;
          break;
        case SDLK_q:
          field = 1;
          break;
        case SDLK_w:
          field = 2;
          break;
        case SDLK_ESCAPE:
          quit = 1;
          break;
        }
      }
    }

    // Очистка экрана
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Отрисовка выбранной диаграммы
    if (chart_type == 0) {
      render_bar_chart(renderer, font, arr, field);
    } else {
      render_pie_chart(renderer, font, arr, field);
    }

    // Обновление экрана
    SDL_RenderPresent(renderer);
  }

  // Сохранение данных
  save_objects_to_file(arr, "data.txt");

  // Загрузка данных для демонстрации
  ObjectArray *arr2 = create_object_array(5);
  load_objects_from_file(arr2, "data.txt");
  printf("\nLoaded data:\n");
  print_objects(arr2, stdout);

  // Освобождение ресурсов
  free_object_array(arr);
  free_object_array(arr2);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
