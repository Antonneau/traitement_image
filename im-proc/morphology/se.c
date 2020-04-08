#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <se.h>

enum {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};

void
build_square(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      for(int c = 0; c < 3; c++){
        pnm_set_component(imd, i, j, c, 255);
      }
    }
  }
}

void
build_diamond(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      int offset_l = abs((size/2) - i);
      int offset_r = (size/2) + i;
      if(offset_r >= size)
        offset_r -= (i % (size/2))*2;
      if (i == size-1)
        offset_r = size/2;
      if(j >= offset_l && j <= offset_r){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

void
build_disk(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      float offset = sqrt((size/2 - i) * (size/2 - i) + (size/2 - j) * (size/2 - j));
      if(offset <= size/2){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

void
build_line_v(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      if (j == size/2){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

void
build_line_h(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      if (i == size/2){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

void
build_diag_r(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      if (j == size-i-1){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

void
build_diag_l(pnm imd)
{
  int size = pnm_get_width(imd);
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      if (j == i){
        for(int c = 0; c < 3; c++){
          pnm_set_component(imd, i, j, c, 255);
        }
      }
    }
  }
}

pnm
se(int s, int hs)
{
  int size = 2*hs+1;
  pnm imd = pnm_new(size, size, PnmRawPpm);
  switch(s)
  {
    case SQUARE:
      build_square(imd);
      break;
    case DIAMOND:
      build_diamond(imd);
      break;
    case DISK:
      build_disk(imd);
      break;
    case LINE_V:
      build_line_v(imd);
      break;
    case DIAG_R:
      build_diag_r(imd);
      break;
    case LINE_H:
      build_line_h(imd);
      break;
    case DIAG_L:
      build_diag_l(imd);
      break;
    case CROSS:
      build_diag_r(imd);
      build_diag_l(imd);
      break;
    case PLUS:
      build_line_v(imd);
      build_line_h(imd);
      break;
    default:
      fprintf(stderr,"Usage : se(shape, halfsize)\n");
      exit(EXIT_FAILURE);
  }
  return imd;
}
