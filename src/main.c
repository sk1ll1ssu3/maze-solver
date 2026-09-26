#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK '#'
#define PATH ' '
#define VIS '.'
#define CHECKED 'X'

typedef struct {
  int x, y;
} pair;

char *get_line(FILE *stream) {
  int ind = 0;
  int len = 1;
  char *str = malloc(len * sizeof(char));
  int ch;
  while ((ch = fgetc(stream)) != '\n' && ch != EOF) {
    str[ind++] = ch;
    if (ind >= len) {
      len *= 2;
      str = realloc(str, len * sizeof(char));
    }
  }
  if (ind >= len) {
    ++len;
    str = realloc(str, len * sizeof(char));
  }
  str[ind] = '\0';
  return str;
}

char **get_maze(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    return NULL;
  int i = 0;
  int len = 1;
  char **maze = malloc(len * sizeof(char *));
  char *curr_line = get_line(fp);
  while (strcmp(curr_line, "") != 0) {
    maze[i++] = curr_line;
    if (i >= len) {
      len *= 2;
      maze = realloc(maze, len * sizeof(char *));
    }
    curr_line = get_line(fp);
  }
  if (i >= len) {
    ++len;
    maze = realloc(maze, len * sizeof(char *));
  }
  maze[i] = NULL;
  free(curr_line);
  fclose(fp);
  return maze;
}

int array_len(char **maze) {
  int len = 0;
  while (maze[len] != NULL)
    ++len;
  return len;
}

pair find_start(char **maze, int m, int n) {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (maze[i][j] == 'S')
        return (pair){i, j};
    }
  }
  return (pair){-1, -1};
}

bool find_end(char ***maze, int m, int n, pair p) {
  if (p.x < 0 || p.x >= m || p.y < 0 || p.y >= n ||
      (*maze)[p.x][p.y] == BLOCK || (*maze)[p.x][p.y] == CHECKED ||
      (*maze)[p.x][p.y] == VIS)
    return false;
  if ((*maze)[p.x][p.y] == 'E')
    return true;
  if ((*maze)[p.x][p.y] != 'S')
    (*maze)[p.x][p.y] = VIS;
  int xdir[] = {-1, 0, 1, 0};
  int ydir[] = {0, -1, 0, 1};
  bool did_succeed = false;
  for (int i = 0; i < 4; ++i) {
    did_succeed |= find_end(maze, m, n, (pair){p.x + xdir[i], p.y + ydir[i]});
    if (did_succeed)
      return true;
  }
  if ((*maze)[p.x][p.y] != 'S')
    (*maze)[p.x][p.y] = CHECKED; // reset
  return false;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ./maze_solver FILENAME\n");
    return 1;
  }
  char **maze = get_maze(argv[1]);
  if (!maze) {
    fprintf(stderr, "Error: Could not open the file.\n");
    return 1;
  }
  int m = array_len(maze);
  if (m == 0) {
    fprintf(stderr, "Warning: Maze is empty.\n");
    return 1;
  }
  int n = strlen(maze[0]);
  pair start = find_start(maze, m, n);
  if (start.x == -1 || start.y == -1) {
    fprintf(stderr, "Error: Could not find start.\n");
    return 1;
  }
  find_end(&maze, m, n, start);
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (maze[i][j] == 'X') {
        printf("%c", PATH);
      } else {
        printf("%c", maze[i][j]);
      }
    }
    printf("\n");
  }
  for (int i = 0; i < m; ++i) {
    free(maze[i]);
  }
  free(maze);
  return 0;
}
