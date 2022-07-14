#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename( FILE *fp, int state);

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c);

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state);

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition( int state1,	int state2);

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state);

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state);

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state);

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state);

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int level, int state);

// 방문한 상태들을 차례로 화면에 출력
static void print_states( int visited[], int count);

// recursive function
static void dfs_main( int state, int goal_state, int level, int visited[]);

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]);

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num);

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num);

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int level = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( init_state, goal_state, level, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging)
	print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}



static void print_statename( FILE *fp, int state){
	int p=0, w=0, g=0, c=0;
	get_pwgc(state, &p,&w,&g,&c);
	fprintf(fp,"<%d%d%d%d>\n", p,w,g,c);
}

static void get_pwgc( int state, int *p, int *w, int *g, int *c){
	*p = (state & PEASANT) >>3;
	*w = (state & WOLF) >>2;
	*g = (state & GOAT) >>1;
	*c = (state & CABBAGE);
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state){
	if(state==3 || state ==6 || state == 7 || state == 8 || state==9 || state == 12)
	  return 1;
	else
		return 0;
}


// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition( int state1,	int state2){
	int cnt = 0;
	if(is_dead_end(state2))
		return 0;
	int before[4], after[4];
	get_pwgc(state1, before, before+1, before+2, before+3);
	get_pwgc(state2, after, after+1, after+2, after+3);
	if(before[0]==after[0]) return 0;
	for(int i=1;i<4; i++){
		if(before[i]!=after[i]){
			cnt ++;
			if(cnt > 1) return 0;
			if(before[0]!=before[i]) return 0;
		}
	}	
	return 1;
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state){
	int newstate = state;
	if(state & PEASANT) newstate = newstate - PEASANT;
	else newstate = newstate + PEASANT;
	return newstate;
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state){
	int newstate = state;
	if(state & PEASANT) newstate = newstate -PEASANT;
	else newstate = newstate + PEASANT;
	if(state&WOLF) newstate = newstate - WOLF;
	else newstate += WOLF;

	if(!is_possible_transition(state, newstate)) return -1;
	return newstate;
}
// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state){
	int newstate = state;
	if(state & PEASANT) newstate = newstate -PEASANT;
	else newstate = newstate + PEASANT;
	if(state&GOAT) newstate = newstate - GOAT;
	else newstate += GOAT;

	if(!is_possible_transition(state, newstate)) return -1;
	return newstate;
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state){
	int newstate = state;
	if(state & PEASANT) newstate = newstate -PEASANT;
	else newstate = newstate + PEASANT;
	if(state&CABBAGE) newstate = newstate - CABBAGE;
	else newstate += CABBAGE;

	if(!is_possible_transition(state, newstate)) return -1;
	return newstate;
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int level, int state){
	for(int i = 0; i<level; i++){
		if(state == visited[i])
			return 1;
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
static void print_states( int visited[], int count){
	for(int i = 0; i<count; i++){
		int p=0, w=0, g=0, c=0;
		get_pwgc(visited[i], &p,&w,&g,&c);
		printf("<%d%d%d%d>\n", p,w,g,c);
	}
}

// recursive function
static void dfs_main( int state, int goal_state, int level, int visited[]){
	int p=0, w=0, g=0, c=0;
	get_pwgc(state, &p,&w,&g,&c);
	visited[level] = state;
	printf("cur state is <%d%d%d%d> (level %d)\n",p,w,g,c, level);
	if(state == goal_state) {
		 printf("\nGoal-state found!\n");
		 print_states(visited, level+1);
		 return;
	}
	else{
		int pstate = changeP(state);
		int pwstate = changePW(state);
		int pcstate = changePC(state);
		int pgstate = changePG(state);
		
		get_pwgc(pstate, &p,&w,&g,&c); 
		if(is_possible_transition(state, pstate)){
			if(is_visited(visited,level, pstate)){
				printf("\tnext state <%d%d%d%d> has been visited\n", p,w,g,c);
			}
			else{
				dfs_main(pstate, goal_state, level+1, visited);		
				get_pwgc(state, &p,&w,&g,&c); 
				printf("back to <%d%d%d%d> (level %d)\n" ,p,w,g,c, level);
			}
		}
		else {
			printf("\tnext state <%d%d%d%d> is dead-end\n", p,w,g,c);
		}


		get_pwgc(pwstate, &p,&w,&g,&c); 
		if(pwstate != -1){
			if(is_visited(visited, level, pwstate)){
				printf("\tnext state <%d%d%d%d> has been visited\n", p,w,g,c);
			}
			else{
				dfs_main(pwstate, goal_state, level+1, visited);		
				get_pwgc(state, &p,&w,&g,&c); 
				printf("back to <%d%d%d%d> (level %d)\n", p,w,g,c, level);
			}
		}
		else {
			printf("\tnext state <%d%d%d%d> is dead-end\n", (state & PEASANT) ? (state - PEASANT) >> 3: (state + PEASANT) >> 3, (state & WOLF) ? 0 : 1, (state & GOAT) >> 1, (state & CABBAGE));
		}


		get_pwgc(pgstate, &p,&w,&g,&c); 
		if(pgstate != -1){
			if(is_visited(visited,level, pgstate)){
				printf("\tnext state <%d%d%d%d> has been visited\n", p,w,g,c);
			}
			else{
				dfs_main(pgstate, goal_state, level+1, visited);		
				get_pwgc(state, &p,&w,&g,&c); 
				printf("back to <%d%d%d%d> (level %d)\n", p,w,g,c, level);
			}

		}
		else {
			printf("\tnext state <%d%d%d%d> is dead-end\n",(state & PEASANT) ? 0 : 1, (state & WOLF) >> 2, (state & GOAT) ? 0 : 1, (state & CABBAGE) >> 0);
		}

		
		get_pwgc(pcstate, &p,&w,&g,&c); 
		if(pcstate != -1){
			if(is_visited(visited,level, pcstate)){
				printf("\tnext state <%d%d%d%d> has been visited\n", p,w,g,c);
			}
			else{
				dfs_main(pcstate, goal_state, level+1, visited);		
				get_pwgc(state, &p,&w,&g,&c); 
				printf("back to <%d%d%d%d> (level %d)\n", p,w,g,c, level);
			}
		}
		else {
			printf("\tnext state <%d%d%d%d> is dead-end\n",(state & PEASANT) ? 0: 1, (state & WOLF) >> 2, (state & GOAT) >> 1, (state & CABBAGE)?0:1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]){
	int i, j;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(is_dead_end(i) == 0 && is_possible_transition(i,j))
				graph[i][j] = 1;
			else
				graph[i][j] = 0;
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num){
	int i, j;
	for(i=0;i<16; i++){
		for(j=0;j<16;j++){
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num){
	FILE *file = fopen(filename, "w");
    fprintf(file, "*Vertices 16\n");
    for(int i = 1; i <= num; i++){
        fprintf(file, "%d ", i);
        print_statename(file, i - 1);
    }
    fprintf(file, "*Edges\n");
    for(int i = 0; i < num; i++){
        for(int j = i + 1; j < num; j++){
            if(graph[i][j]) fprintf(file, "%3d%3d\n", i + 1, j + 1);
        }
    }
    fclose(file);
}

