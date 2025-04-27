#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include "potentials.h"
#include "grids.h"

REAL SpacingCoulombGrid;
static INT_VECTOR3 NumberOfCoulombGridPoints;
static VECTOR SizeGrid;
static VECTOR ShiftGrid;
static VECTOR DeltaCoulombGrid;

void readCoulombGrid(char *buffer)
{
  int i,j,k,m;
  VECTOR unit_cell_size;
  INT_VECTOR3 number_of_unit_cells;
  REAL ewald_precision;
  FILE *FilePtr;

  fprintf(stderr,"Reading Coulomb grid\n");
  fprintf(stderr,"Opening: %s\n",buffer);

  if(!(FilePtr=fopen(buffer,"r")))
  {
    fprintf(stderr,"Error:  file %s does not exist.\n",buffer);
    exit(1);
  }

  fread(&SpacingCoulombGrid,sizeof(REAL),1,FilePtr);
  fread(&NumberOfCoulombGridPoints,sizeof(INT_VECTOR3),1,FilePtr);
  fread(&SizeGrid,sizeof(VECTOR),1,FilePtr);
  fread(&ShiftGrid,sizeof(VECTOR),1,FilePtr);
  fread(&DeltaCoulombGrid,sizeof(VECTOR),1,FilePtr);
  fread(&unit_cell_size,sizeof(VECTOR),1,FilePtr);
  fread(&number_of_unit_cells,sizeof(INT_VECTOR3),1,FilePtr);
  fread(&ewald_precision,sizeof(REAL),1,FilePtr);

  printf("Reading: %s\n",buffer);

  float ****Grid;

  Grid=(float****)calloc(NumberOfCoulombGridPoints.x+1,sizeof(float***));
  for(i=0;i<=NumberOfCoulombGridPoints.x;i++)
  {
    Grid[i]=(float***)calloc(NumberOfCoulombGridPoints.y+1,sizeof(float**));
    for(j=0;j<=NumberOfCoulombGridPoints.y;j++)
    {
      Grid[i][j]=(float**)calloc(NumberOfCoulombGridPoints.z+1,sizeof(float*));
      for(k=0;k<=NumberOfCoulombGridPoints.z;k++)
        Grid[i][j][k]=(float*)calloc(8,sizeof(float));
    }
  }

  for(m=0;m<8;m++)
  {
    for(i=0;i<=NumberOfCoulombGridPoints.x;i++)
    {
      for(j=0;j<=NumberOfCoulombGridPoints.y;j++)
      {
        for(k=0;k<=NumberOfCoulombGridPoints.z;k++)
        {
          fread(&Grid[i][j][k][m],1,sizeof(float),FilePtr);
        }
      }
    }
  }

  FILE *file;
  char str[256];
  char dir[256];
  char output[256];
  char *cwd = getcwd(NULL,0);

  strcpy(str,buffer);
  strcat(str,".txt");

  sprintf(dir,"%s/outputsCoulomb",cwd);
  mkdir(dir,S_IRWXU);

  printf("Writing: %s\n",str);
  sprintf(output,"%s/%s",dir,str);
  file = fopen(output,"w");

  fprintf(file,"Grid spacing: %f Å\n\n",SpacingCoulombGrid);
  fprintf(file,"Number of grid points (x, y, z)\n");
  fprintf(file,"%d %d %d\n\n",NumberOfCoulombGridPoints.x+1,NumberOfCoulombGridPoints.y+1,NumberOfCoulombGridPoints.z+1);
  fprintf(file,"Size of grid (x, y, z) [Å]\n");
  fprintf(file,"%f %f %f\n\n",SizeGrid.x,SizeGrid.y,SizeGrid.z);
  fprintf(file,"Shift (x, y, z) [Å]\n");
  fprintf(file,"%f %f %f\n\n",ShiftGrid.x,ShiftGrid.y,ShiftGrid.z);
  fprintf(file,"Potentials [%d][%d][%d]\n",NumberOfCoulombGridPoints.x+1,NumberOfCoulombGridPoints.y+1,NumberOfCoulombGridPoints.z+1);

  for(i=0;i<=NumberOfCoulombGridPoints.x;i++)
  {
    for(j=0;j<=NumberOfCoulombGridPoints.y;j++)
    {
      for(k=0;k<=NumberOfCoulombGridPoints.z;k++)
      {
        fprintf(file,"%f\t",Grid[i][j][k][0]);
      }
    fprintf(file,"\n");
    }
    fprintf(file,"\n");
  }

  fclose(file);
  fclose(FilePtr);
  printf("Done \n\n");
}

int main(int argc, char *argv[])
{
	readCoulombGrid(argv[1]);
	return 0;
}