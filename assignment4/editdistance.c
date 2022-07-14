#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
static void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8]);

// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를 다음과 같은 기호로 표시한다. 삽입:I, 삭제:D, 교체:S, 일치:M, 전위:T
void print_matrix( int *op_matrix, int col_size, char *str1, char *str2, int n, int m);

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2);

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings
	
	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str);
	
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	fprintf( stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf( stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf( stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf( stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}


// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를 다음과 같은 기호로 표시한다. 삽입:I, 삭제:D, 교체:S, 일치:M, 전위:T
void print_matrix( int *op_matrix, int col_size, char *str1, char *str2, int n, int m){

	for(int i=n; i>0; i--){
		printf("%c\t", *(str1+i-1));
		for(int j=1; j<col_size; j++){
			int num = *(op_matrix + col_size*i + j);
			if(num&SUBSTITUTE_OP) {printf("S"); num-=SUBSTITUTE_OP;}
			if(num&MATCH_OP) {printf("M"); num-=MATCH_OP;}
			if(num&INSERT_OP) {printf("I"); num -= INSERT_OP; }
			if(num&DELETE_OP) {printf("D");num-=DELETE_OP;}
			if(num&TRANSPOSE_OP) {printf("T"); num-=TRANSPOSE_OP;}
			printf("\t");
		} 	
		printf("\n");
	}
	printf("\t");
	for(int i = 0; i<m; i++){
		printf("%c\t",*(str2+i));
	}
	printf("\n");


}

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
static void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8]){
	static int cnt = 0;
	if(level == 0) cnt = 0;
	if(n==0 && m==0) {
		cnt++;
		printf("\n[%d] ==============================\n",cnt);
		print_alignment(align_str, level-1);

		return;
	}
//	printf("==%d===\n",*(op_matrix+col_size*(n-1)+m-1));
	if(*(op_matrix+col_size*(n)+m) & MATCH_OP || *(op_matrix+col_size*n+m) & SUBSTITUTE_OP) {
		align_str[level][0] = str1[n-1];
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = str2[m-1];
		align_str[level][5] = '\0';
//		printf("여긴 M또는 S\n");
//		printf("n = %d, m=%d, level = %d\n\n", n, m, level);
		backtrace_main(op_matrix, col_size, str1, str2, n-1, m-1, level+1, align_str);
	}
	if(*(op_matrix+col_size*(n)+m) & INSERT_OP) {
		align_str[level][0] = '*';
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = str2[m-1];
		align_str[level][5] = '\0';
//		printf("여긴 I\n");
//		printf("n = %d, m=%d, level = %d\n\n", n, m, level);
		backtrace_main(op_matrix, col_size, str1, str2, n, m-1, level+1, align_str);
	}
	if(*(op_matrix+col_size*(n)+m) & DELETE_OP) {
		align_str[level][0] = str1[n-1];
		align_str[level][1] = ' ';
		align_str[level][2] = '-';
		align_str[level][3] = ' ';
		align_str[level][4] = '*';
		align_str[level][5] = '\0';
//		printf("여긴 D\n");
//		printf("n = %d, m=%d, level = %d\n\n", n, m, level);
		backtrace_main(op_matrix, col_size, str1, str2, n-1, m, level+1, align_str);
	}
	if(*(op_matrix+col_size*(n)+m) & TRANSPOSE_OP) {
		align_str[level][0] = str1[n-2];
		align_str[level][1] = str1[n-1];
		align_str[level][2] = ' ';
		align_str[level][3] = '-';
		align_str[level][4] = ' ';
		align_str[level][5] = str1[n-1];
		align_str[level][6] = str1[n-2];
		align_str[level][7] = '\0';
//		printf("여긴 T\n");
//		printf("n = %d, m=%d, level = %d\n\n", n, m, level);
		backtrace_main(op_matrix, col_size, str1, str2, n-2, m-2, level+1, align_str);
	}
	return;
}


// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2){
	int n = strlen(str1);
	int m = strlen(str2);
	int col_size = m+1;
	int op_matrix[900]={0,}; 
	int cost_matrix[30][30] = {0,};
	for(int i = 0; i<= n; i++){
		cost_matrix[i][0]=i;
		op_matrix[i*col_size] += DELETE_OP;
	}
	for(int j = 0;j <= m; j++){
		cost_matrix[0][j] = j;
		op_matrix[j] += INSERT_OP;
	}
	for(int i = 1; i<=n; i++){
		for(int j = 1; j<=m; j++){
			if(i<2 || j<2){
				if(str1[i-1]==str2[j-1]) {
				cost_matrix[i][j] = __GetMin3(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1],cost_matrix[i][ j-1]+INSERT_COST);
				}
				else cost_matrix [i][j] = __GetMin3(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1]+SUBSTITUTE_COST,cost_matrix[i][ j-1]+INSERT_COST);
			}
			else{
				if(str1[i-1]==str2[j-1]) {
					if(str1[i-1]==str2[j-2] && str1[i-2]==str2[j-1])
						cost_matrix[i][j] = __GetMin4(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1],cost_matrix[i][j-1]+INSERT_COST, cost_matrix[i-2][j-2]+TRANSPOSE_COST);
					else
						cost_matrix[i][j] = __GetMin3(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1],cost_matrix[i][j-1]+INSERT_COST);
				}
				else if (str1[i-1]==str2[j-2] && str1[i-2]==str2[j-1])
					cost_matrix [i][j] = __GetMin4(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1]+SUBSTITUTE_COST,cost_matrix[i][ j-1]+INSERT_COST,cost_matrix[i-2][j-2]+TRANSPOSE_COST);
				else
					cost_matrix [i][j] = __GetMin3(cost_matrix[i-1][j]+DELETE_COST, cost_matrix[i-1][j-1]+SUBSTITUTE_COST,cost_matrix[i][ j-1]+INSERT_COST);

			}

			if(cost_matrix[i][j]==cost_matrix[i-1][j-1] && str1[i-1]==str2[j-1]){
				op_matrix[i*col_size + j] += MATCH_OP;
			}
			if(cost_matrix[i][j]==cost_matrix[i-1][j-1]+1){
				op_matrix[i*col_size + j] += SUBSTITUTE_OP;
			}
			if(cost_matrix[i][j]==cost_matrix[i][j-1]+1){
				op_matrix[i*col_size + j] += INSERT_OP;
			}
			if(cost_matrix[i][j]==cost_matrix[i-1][j]+1){
				op_matrix[i*col_size + j] += DELETE_OP;
			}
			if(i>=2 && j>=2 && cost_matrix[i][j]==cost_matrix[i-2][j-2]+1 && str1[i-1]==str2[j-2] && str1[i-2]==str2[j-1]){
				op_matrix[i*col_size + j] += TRANSPOSE_OP;
			}
		}
	}

/*	for(int i=n;i>=0;i--){
		for(int j=0;j<=m; j++){
			printf("%d\t",cost_matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n\n\n");
	for(int i=n-1; i>=0; i--){
		printf("%c\t", *(str1+i));
		for(int j=0; j<col_size; j++){
			printf("%d\t",*(op_matrix + col_size*i + j));
		} 	
		printf("\n");
	}
	printf("\t");
	for(int i =0; i<col_size; i++){
		printf("%c\t",*(str2+i));
	}
	printf("\n\n\n\n");

*/

	print_matrix(op_matrix, m+1, str1, str2, n, m);
	backtrace(op_matrix, m+1, str1, str2, n, m);
	return cost_matrix[n][m];
}