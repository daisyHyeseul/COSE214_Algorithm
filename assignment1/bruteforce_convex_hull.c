#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
#include <time.h> //time

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point){
	printf("#points\n");
	for(int i = 0; i<num_point; i++){
		printf("points(%d, %d)\n",points[i].x,points[i].y);
	}
}

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line){
	printf("#line segments\n");
	for(int i = 0; i<num_line; i++){
		printf("segments(%d, %d, %d, %d)\n",lines[i].from.x,lines[i].from.y,lines[i].to.x,lines[i].to.y);
	}
}


// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of line segments that forms the convex hull
// return value : set of line segments that forms the convex hull
t_line *convex_hull( t_point *points, int num_point, int *num_line){
	int i, j;
	*num_line = 0;
	t_line *lines = (t_line*) malloc(num_point * sizeof( t_line));;
	for(i=0;i<num_point;i++){
		for(j=i+1; j<num_point; j++){
		//	printf("come in! %d,%d / %d, %d\n", points[i].x, points[i].y, points[j].x, points[j].y);
			int a = points[j].y-points[i].y;
			int b = points[i].x-points[j].x;
			int c = points[i].x * points[j].y - points[i].y*points[j].x;
			int flag = 0;
			for(int m=0; m<num_point; m++){
				if(a*points[m].x + b*points[m].y > c){
					if(flag == 0 || flag == 1)
						flag = 1;
					else{
						flag = 4;
						break;
					}
				}
				else if(a*points[m].x + b*points[m].y == c)
					flag = 0;
				else if(a*points[m].x + b*points[m].y < c){
					if(flag == 0 || flag == 2)
						flag =2 ;
					else{
						flag = 4; 
						break;
					}
				}
						
			}
			if(flag != 4){
				lines[*num_line].from = points[i];
				lines[*num_line].to=points[j];
				*num_line += 1;
			//	printf("created line! %d,%d / %d, %d\n", points[i].x, points[i].y, points[j].x, points[j].y);
			}
		}
	}
	return lines;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line; // number of lines
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
		
	t_line *lines;

	// making n points
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	lines = convex_hull( points, num_point, &num_line);

	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
		
	print_footer();
	
	free( points);
	free( lines);
	
	return 0;
}
