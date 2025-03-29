#include "object_module.h"
#include "visualisation.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_PATH "arial.ttf"

void process_console_input(ObjectArray *arr, int *chart_type, int *field);
void handle_command(ObjectArray *arr, const char *command, int *chart_type,
                    int *field);

int main() {
  ObjectArray *arr = create_object_array(5);
  add_object(arr, "Ibragim", 30, 65);
  add_object(arr, "Ramazan", 40, 100.2);
  add_object(arr, "Dmitriy", 35, 81.1);

  printf("=== Data Visualization Console ===\n");
  printf("Available commands:\n");
  printf("  add [name] [age] [weight]  - Add new object\n");
  printf("  remove [name]              - Remove object\n");
  printf("  save [filename]            - Save data to file\n");
  printf("  load [filename]            - Load data from file\n");
  printf("  chart [bar|pie]            - Change chart type\n");
  printf("  field [age|weight]         - Change data field\n");
  printf("  list                       - Show all objects\n");
  printf("  help                       - Show this help\n");
  printf("  exit                       - Quit program\n");

  // Инициализация SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() == -1) {
    fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, 16);
  if (!font) {
    fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

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

  int quit = 0;
  int chart_type = 0; // 0 - bar, 1 - pie
  int field = 1;      // 1 - age, 2 - weight

  // Основной цикл
  while (!quit) {
    // Обработка SDL событий
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        quit = 1;

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

    // Обработка консольного ввода
    process_console_input(arr, &chart_type, &field);

    // Отрисовка
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    if (chart_type == 0) {
      render_bar_chart(renderer, font, arr, field);
    } else {
      render_pie_chart(renderer, font, arr, field);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

  // Очистка ресурсов
  free_object_array(arr);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}

void process_console_input(ObjectArray *arr, int *chart_type, int *field) {
  char buffer[256] = {0};

#ifdef _WIN32
  if (_kbhit()) {
    if (fgets(buffer, sizeof(buffer), stdin)) {
      buffer[strcspn(buffer, "\n")] = 0;
      handle_command(arr, buffer, chart_type, field);
    }
  }
#else
  struct timeval tv = {0, 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      buffer[strcspn(buffer, "\n")] = 0;
      handle_command(arr, buffer, chart_type, field);
    }
  }
#endif
}

void handle_command(ObjectArray *arr, const char *command, int *chart_type,
                    int *field) {
  char cmd[20] = {0};
  char arg1[50] = {0}, arg2[50] = {0}, arg3[50] = {0};
  int num_args = sscanf(command, "%19s %49s %49s %49s", cmd, arg1, arg2, arg3);

  if (strcmp(cmd, "add") == 0 && num_args >= 4) { // Теперь 4 аргумента
    int age;
    double weight;
    if (sscanf(arg2, "%d", &age) == 1 && sscanf(arg3, "%lf", &weight) == 1) {
      if (add_object(arr, arg1, age,
                     weight)) { // Проверяем возвращаемое значение
        printf("Added: %s (Age: %d, Weight: %.1f)\n", arg1, age, weight);
      } else {
        printf("Error adding object!\n");
      }
    }

  } else if (strcmp(cmd, "remove") == 0 && num_args >= 2) {
    int index = atoi(arg1);
    if (index >= 0 && index < arr->size) {
      remove_object(arr, index);
      printf("Removed index: %d\n", index);
    } else {
      printf("Invalid index: %d\n", index);
    }
  } else if (strcmp(cmd, "save") == 0 && num_args >= 2) {
    save_objects_to_file(arr, arg1);
    printf("Data saved to: %s\n", arg1);
  } else if (strcmp(cmd, "load") == 0 && num_args >= 2) {
    if (load_objects_from_file(arr, arg1)) {
      printf("Data loaded from: %s\n", arg1);
    } else {
      printf("Failed to load: %s\n", arg1);
    }
  } else if (strcmp(cmd, "chart") == 0 && num_args >= 2) {
    if (strcmp(arg1, "bar") == 0)
      *chart_type = 0;
    else if (strcmp(arg1, "pie") == 0)
      *chart_type = 1;
    printf("Chart type set to: %s\n", arg1);
  } else if (strcmp(cmd, "field") == 0 && num_args >= 2) {
    if (strcmp(arg1, "age") == 0)
      *field = 1;
    else if (strcmp(arg1, "weight") == 0)
      *field = 2;
    printf("Field set to: %s\n", arg1);
  } else if (strcmp(cmd, "sort") == 0 && num_args >= 2) {
    int sort_field = -1;
    int ascending = 1; // По умолчанию сортировка по возрастанию

    if (strcmp(arg1, "name") == 0)
      sort_field = 0;
    else if (strcmp(arg1, "age") == 0)
      sort_field = 1;
    else if (strcmp(arg1, "weight") == 0)
      sort_field = 2;

    if (num_args >= 3) {
      if (strcmp(arg2, "asc") == 0)
        ascending = 1;
      else if (strcmp(arg2, "desc") == 0)
        ascending = 0;
      else {
        printf("Invalid sort direction. Use asc/desc\n");
        return;
      }
    }

    if (sort_field != -1) {
      sort_objects(arr, sort_field, ascending);
      printf("Sorted by %s (%s)\n", arg1, ascending ? "asc" : "desc");
    } else {
      printf("Invalid sort field. Use name/age/weight\n");
    }
  } else if (strcmp(cmd, "list") == 0) {
    printf("Current objects:\n");
    print_objects(arr, stdout);
  } else if (strcmp(cmd, "help") == 0) {
    printf("Available commands:\n");
    printf("  add [name] [age] [weight]         - Add new object\n");
    printf("  remove [index]                    - Remove object\n");
    printf("  save [filename]                   - Save data to file\n");
    printf("  load [filename]                   - Load data from file\n");
    printf("  chart [bar|pie]                   - Change chart type\n");
    printf("  field [age|weight]                - Change data field\n");
    printf("  list                              - Show all objects\n");
    printf("  sort [name/age/weight] [asc/desc] - Sort objects\n");
    printf("  help                              - Show this help\n");
    printf("  exit                              - Quit program\n");
  } else if (strcmp(cmd, "exit") == 0) {
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
  } else {
    printf("Unknown command. Type 'help' for available commands.\n");
  }
}
