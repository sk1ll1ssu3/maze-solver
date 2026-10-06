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
  str[ind] = '\0';
  return str;
}

char **get_maze(char *filename) {
  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if (!fp)
    return NULL;
  int i = 0;
  int len = 1;
  char **maze = malloc(len * sizeof(char *));
  char *curr_line = get_line(fp);
  while (curr_line) {
    if (strcmp(curr_line, "") == 0) {
      free(curr_line);
      break;
    }
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
  fclose(fp);
  return maze;
}

int array_len(char **maze) {
  int len = 0;
  while (maze[len])
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

bool find_end(char **maze, int m, int n, pair p) {
  if (p.x < 0 || p.x >= m || p.y < 0 || p.y >= n || maze[p.x][p.y] == BLOCK ||
      maze[p.x][p.y] == CHECKED || maze[p.x][p.y] == VIS)
    return false;
  if (maze[p.x][p.y] == 'E')
    return true;
  if (maze[p.x][p.y] != 'S')
    maze[p.x][p.y] = VIS;
  int xdir[] = {-1, 0, 1, 0};
  int ydir[] = {0, -1, 0, 1};
  for (int i = 0; i < 4; ++i) {
    if (find_end(maze, m, n, (pair){p.x + xdir[i], p.y + ydir[i]}))
      return true;
  }
  if (maze[p.x][p.y] != 'S')
    maze[p.x][p.y] = CHECKED; // reset
  return false;
}

char *find_arg(int argc, char **argv, const char *specifier) {
  char *argname = NULL;
  for (int argi = 1; argi < argc; ++argi) {
    if (strcmp(argv[argi], specifier) == 0 && argi < argc - 1) {
      argname = argv[argi + 1];
      break;
    }
  }
  return argname;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./maze_solver --filename FILENAME\nYou can use "
                    "--write-solution to write the solution to some file\n");
    return 1;
  }
  char *filename = find_arg(argc, argv, "--filename");
  if (!filename) {
    fprintf(stderr, "Error: Could not parse filename.\n");
    return 1;
  }
  char **maze = get_maze(filename);
  if (!maze) {
    fprintf(stderr, "Error: Could not open the file.\n");
    return 1;
  }
  int m = array_len(maze);
  if (m == 0) {
    fprintf(stderr, "Warning: Maze is empty.\n");
    free(maze);
    return 1;
  }
  int n = strlen(maze[0]);
  pair start = find_start(maze, m, n);
  if (start.x == -1 || start.y == -1) {
    fprintf(stderr, "Error: Could not find start.\n");
    for (int i = 0; i < m; ++i) {
      free(maze[i]);
    }
    free(maze);
    return 1;
  }
  find_end(maze, m, n, start);
  char *output_file = find_arg(argc, argv, "--write-solution");
  FILE *output_fp = NULL;
  if (output_file)
    output_fp = fopen(output_file, "w");
  if (output_file && !output_fp)
    fprintf(stderr, "Warning: Could not open output file so not writing.\n");
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (maze[i][j] == 'X') {
        printf("%c", PATH);
        if (output_fp)
          fprintf(output_fp, "%c", PATH);
      } else {
        printf("%c", maze[i][j]);
        if (output_fp)
          fprintf(output_fp, "%c", maze[i][j]);
      }
    }
    printf("\n");
    if (output_fp)
      fprintf(output_fp, "\n");
  }
  if (output_fp)
    fclose(output_fp);
  for (int i = 0; i < m; ++i) {
    free(maze[i]);
  }
  free(maze);
  return 0;
}
