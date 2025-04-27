#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include "potentials.h"
#include "grids.h"

REAL SpacingVDWGrid;
INT_VECTOR3 NumberOfVDWGridPoints;
static INT_VECTOR3 NumberOfCoulombGridPoints;
static VECTOR SizeGrid;
static VECTOR ShiftGrid;
static VECTOR DeltaVDWGrid;

void readVDWGrid(char *buffer)
{
  int i,j,k,m;
  VECTOR unit_cell_size;
  INT_VECTOR3 number_of_unit_cells;
  FILE *FilePtr;

  fprintf(stderr,"Reading VDW grid\n");
  fprintf(stderr,"Opening: %s\n",buffer);

  if(!(FilePtr=fopen(buffer,"r")))
  {
    fprintf(stderr, "Error:  file %s does not exist.\n",buffer);
    exit(1);
  }

  fread(&SpacingVDWGrid,sizeof(REAL),1,FilePtr);
  fread(&NumberOfVDWGridPoints,sizeof(INT_VECTOR3),1,FilePtr);
  fread(&SizeGrid,sizeof(VECTOR),1,FilePtr);
  fread(&ShiftGrid,sizeof(VECTOR),1,FilePtr);
  fread(&DeltaVDWGrid,sizeof(VECTOR),1,FilePtr);
  fread(&unit_cell_size,sizeof(VECTOR),1,FilePtr);
  fread(&number_of_unit_cells,sizeof(INT_VECTOR3),1,FilePtr);

  printf("Reading: %s\n",buffer);

  float ****Grid;

  Grid=(float****)calloc(NumberOfVDWGridPoints.x+1,sizeof(float***));
  for(i=0;i<=NumberOfVDWGridPoints.x;i++)
  {
    Grid[i]=(float***)calloc(NumberOfVDWGridPoints.y+1,sizeof(float**));
    for(j=0;j<=NumberOfVDWGridPoints.y;j++)
    {
      Grid[i][j]=(float**)calloc(NumberOfVDWGridPoints.z+1,sizeof(float*));
      for(k=0;k<=NumberOfVDWGridPoints.z;k++)
        Grid[i][j][k]=(float*)calloc(8,sizeof(float));
    }
  }

  for(m=0;m<8;m++)
  {
    for(i=0;i<=NumberOfVDWGridPoints.x;i++)
    {
      for(j=0;j<=NumberOfVDWGridPoints.y;j++)
      {
        for(k=0;k<=NumberOfVDWGridPoints.z;k++)
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

  sprintf(dir,"%s/outputsVDW",cwd);
  mkdir(dir,S_IRWXU);

  printf("Writing: %s\n",str);
  sprintf(output,"%s/%s",dir,str);
  file = fopen(output,"w+");

  fprintf(file,"Grid spacing: %f Å\n\n",SpacingVDWGrid);
  fprintf(file,"Number of grid points (x, y, z)\n");
  fprintf(file,"%d %d %d\n\n",NumberOfVDWGridPoints.x+1,NumberOfVDWGridPoints.y+1,NumberOfVDWGridPoints.z+1);
  fprintf(file,"Size of grid (x, y, z) [Å]\n");
  fprintf(file,"%f %f %f\n\n",SizeGrid.x,SizeGrid.y,SizeGrid.z);
  fprintf(file,"Shift (x, y, z) [Å]\n");
  fprintf(file,"%f %f %f\n\n",ShiftGrid.x,ShiftGrid.y,ShiftGrid.z);
  fprintf(file,"Potentials [%d][%d][%d]\n",NumberOfVDWGridPoints.x+1,NumberOfVDWGridPoints.y+1,NumberOfVDWGridPoints.z+1);

  for(m=0;m<8;m++)
  {
    for(i=0;i<=NumberOfVDWGridPoints.x;i++)
    {
      for(j=0;j<=NumberOfVDWGridPoints.y;j++)
      {
        for(k=0;k<=NumberOfVDWGridPoints.z;k++)
        {
          fprintf(file,"%f\t",Grid[i][j][k][m]);
        }
        fprintf(file,"\n");
      }
      fprintf(file,"\n");
    }
    fprintf(file, "\n");
  }

  fclose(file);
  fclose(FilePtr);
  printf("Done\n\n");
}

int main(int argc, char *argv[])
{
  readVDWGrid(argv[1]);
  return 0;
}