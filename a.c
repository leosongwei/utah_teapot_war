#include <GL/glut.h>
#include <GL/glc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void create_flare(int x, int y, int r_begin, int r_end, int life_time);
void game_reset();

int teapot_location_x = -350;
int teapot_location_y = 0;
int teapot_alivep = 1;

int keys[256];

/* ------------------bullet----------------------*/
#define MAX_NUM_BULLET 512
struct bullet{
	float x; float y;
	float v_x; float v_y;
	int damage;
	int evilp;
	int alivep;
};

struct bullet null_bullet =
{ 0, 0, 0, 0, 0, 0, 0};

struct bullet vector_bullet[MAX_NUM_BULLET];
int counter_bullet = 0;

void create_bullet
(float x, float y, float v_x, float v_y, int damage, int evilp)
{
	int i=0;
	while( (vector_bullet[i].alivep != 0)
			&& i<MAX_NUM_BULLET ){
		i++;
	}
	vector_bullet[i].x = x;
	vector_bullet[i].y = y;
	vector_bullet[i].v_x = v_x;
	vector_bullet[i].v_y = v_y;
	vector_bullet[i].damage = damage;
	vector_bullet[i].evilp = evilp;
	vector_bullet[i].alivep = 1;
}

float opn=0;
void openfire_player()
{
	float opa;
	if(opn<2.0){
		opn+=0.1;
		opa=2*sin(M_PI*opn);
	}else{
		opn=0.0;
	}

	float rand_y = (float)(rand()%40)/20.0 - 1.0;
	create_bullet(teapot_location_x+90, teapot_location_y+25,
			30, rand_y + opa,
			5, 0);
}

int keeping_player_firingp=0;
void check_fire_status_player()
{
	/*
	if(keys['j']){
		openfire_player();
		keeping_player_firingp=1;
	}else{
		keeping_player_firingp=0;
	}
	*/

	//if(keeping_player_firingp=1){
	if(keys['j']){
		openfire_player();
	}
	glutTimerFunc(20, check_fire_status_player, 0);
}

void clear_bullet_all()
{
	for(int i=0; i<MAX_NUM_BULLET; i++){
		vector_bullet[i] = null_bullet;
	}
}

void clear_bullet_evil()
{
	for(int i=0; i<MAX_NUM_BULLET; i++){
		if(vector_bullet[i].evilp){
			vector_bullet[i] = null_bullet;
		}
	}
}

void moving_all_bullet()
{
	struct bullet *bullet_current;
	for(int i=0; i<MAX_NUM_BULLET; i++){
		if(vector_bullet[i].alivep){
			bullet_current = &vector_bullet[i];
			// Fucking C language
			(*bullet_current).x += (*bullet_current).v_x;
			(*bullet_current).y += (*bullet_current).v_y;

			// Shutdown bullet
			if( ((*bullet_current).x >= 480) ||
				((*bullet_current).x <= -410) ||
				((*bullet_current).y >= 250) ||
				((*bullet_current).y <= -210) ){
				(*bullet_current).alivep=0;
			}
		}
	}
}

void show_bullet(struct bullet *bullet_current){
	if(!((*bullet_current).alivep)){
		return;
	}

	float x = (*bullet_current).x;
	float y = (*bullet_current).y;
	float v_x = (*bullet_current).v_x;
	float v_y = (*bullet_current).v_y;

	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(x, y, 0);
	glVertex3f(x-v_x, y-v_y, 0);
}

void show_bullet_all(){
	glBegin(GL_LINES);
	for(int i=0;i<MAX_NUM_BULLET;i++){
		show_bullet( &(vector_bullet[i]) );
	}
	glEnd();
}

/* ------------------ bullet END ----------------------*/

/* ------------------ Enemy ---------------------------*/

#define MAX_NUM_ENEMY 64

struct enemy{
	float x;
	float y;
	float v_x;
	float v_y;
	int type;
	int alivep;
	int hp;
};

struct enemy null_enemy={0,0, 0,0, 0,0, 0};
struct enemy vector_enemy[MAX_NUM_ENEMY];

void clear_enemy(){
	for(int i=0;i<MAX_NUM_ENEMY;i++){
		vector_enemy[i]=null_enemy;
	}
}

void create_enemy
(float x, float y, float v_x, float v_y, int type, int hp)
{
	int i=0;
	while( (vector_enemy[i].alivep != 0)
			&& i<MAX_NUM_ENEMY ){
		i++;
	}
	vector_enemy[i].x = x;
	vector_enemy[i].y = y;
	vector_enemy[i].v_x = v_x;
	vector_enemy[i].v_y = v_y;
	vector_enemy[i].type = type;
	vector_enemy[i].alivep = 1;
	vector_enemy[i].hp = hp;
}

void create_enemy_randomly(){
	int y_rand = rand()%480 - 260;
	create_enemy(500, y_rand,
			-6, 0,
			0, 80);
}

int game_time=0;
void enemy_generator(){
	if(game_time<120){
		game_time += 5;
	}

	int gen_arry[50];
	for(int i=0;i<50;i++) gen_arry[i]=0;

	int i=0;
	int c=0;
	int fip=0;
	int limit=5+game_time/3;
	while(i<=200){
		int slot=rand()%50;
		if(gen_arry[slot]==0){
			gen_arry[slot]=1;
			c++;
		}

		i++;

		if(c>=limit){
			fip=1;
			break;
		}
	}

	/* Must make the algorithm convergence*/
	if(fip==0){
		int to_do = limit-c;
		for(int i=0;i<50;i++){
			if(to_do<=0){
				break;
			}
			if(gen_arry[i]==0){
				gen_arry[i]=1;
				to_do--;
			}
		}
	}

	for(int i=0;i<50;i++){
		if(gen_arry[i]==1){
			glutTimerFunc(i*100, create_enemy_randomly, 0);
		}
	}


	glutTimerFunc(5000, enemy_generator, 0);
}

void moving_all_enemy()
{
	for(int i=0; i<MAX_NUM_ENEMY; i++){
		if(vector_enemy[i].alivep){
			vector_enemy[i].x += vector_enemy[i].v_x;
			vector_enemy[i].y += vector_enemy[i].v_y;

			// Shutdown enemy
			if( (vector_enemy[i].x >= 510) ||
				(vector_enemy[i].x <= -510) ||
				(vector_enemy[i].y >= 280) ||
				(vector_enemy[i].y <= -280) )
			{
				vector_enemy[i].alivep=0;
			}
		}
	}
}

void show_enemy(struct enemy *enemy_current){
	if(!((*enemy_current).alivep)){
		return;
	}

	float x = (*enemy_current).x;
	float y = (*enemy_current).y;

	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glutWireCube(100);
	glPopMatrix();
}

void show_enemy_all(){
	for(int i=0;i<MAX_NUM_ENEMY;i++){
		show_enemy( &vector_enemy[i]);
	}
}

int kill_counter=0;
void check_enemy_health_all(){
	for(int i=0;i<MAX_NUM_ENEMY;i++){
		if((vector_enemy[i].hp<=0)&&(vector_enemy[i].alivep)){
			create_flare(vector_enemy[i].x,
					vector_enemy[i].y,
					50, 100, 300);
			vector_enemy[i].alivep=0;
			kill_counter++;
		}
	}
}

/* ------------------ Enemy END -----------------------*/

/* ------------------ Collision -----------------------*/

int collidedp(int x1, int y1, int x2, int y2, int xa, int ya, int xb, int yb)
{
	if(
			xa<x2 &&
			xb>x1 &&
			ya<y2 &&
			yb>y1){
		return 1;
	}

	return 0;
}

void collision_enemy_and_bullet ( struct enemy *enemy_current ){
	int enemy_size_2 = 50;
	int x1 = (*enemy_current).x-enemy_size_2;
	int y1 = (*enemy_current).y-enemy_size_2;
	int x2 = (*enemy_current).x+enemy_size_2;
	int y2 = (*enemy_current).y+enemy_size_2;
	for(int i=0; i<MAX_NUM_BULLET; i++){
		if(vector_bullet[i].alivep){
			int xa = vector_bullet[i].x-vector_bullet[i].v_x;
			int ya = vector_bullet[i].y-vector_bullet[i].v_y;
			int xb = vector_bullet[i].x;
			int yb = vector_bullet[i].y;

			if( collidedp(x1, y1, x2, y2, xa, ya, xb, yb) ){
				(*enemy_current).hp -= vector_bullet[i].damage;
				vector_bullet[i].alivep = 0;
				create_flare(xb, yb, 5, 40, 100);
			}
		}
	}
}

void collision_enemy_and_teapot( struct enemy *enemy_current ){
	int teapot_size_x_2 = 50;
	int teapot_size_y_2 = 40;
	int x1= teapot_location_x - teapot_size_x_2;
	int y1= teapot_location_y - teapot_size_y_2;
	int x2= teapot_location_x + teapot_size_x_2;
	int y2= teapot_location_y + teapot_size_y_2;

	int enemy_size_2 = 50;
	int xa = (*enemy_current).x-enemy_size_2;
	int ya = (*enemy_current).y-enemy_size_2;
	int xb = (*enemy_current).x+enemy_size_2;
	int yb = (*enemy_current).y+enemy_size_2;

	if( collidedp(x1, y1, x2, y2, xa, ya, xb, yb) ){
		teapot_alivep=0;
		create_flare(teapot_location_x, teapot_location_y, 30, 300, 500);
		glutTimerFunc(500, game_reset, 0);
	}
}

void check_collision_all(){
	for(int i=0; i<MAX_NUM_ENEMY; i++){
		if(vector_enemy[i].alivep){
			collision_enemy_and_bullet( &vector_enemy[i] );
			collision_enemy_and_teapot( &vector_enemy[i] );
		}
	}
}

/* ------------------ Collision END -------------------*/

/* ------------------ Flare ---------------------------*/
#define MAX_NUM_FLARE 128
struct flare{
	int x;
	int y;
	int r_begin;
	int r_end;
	int life_time;
	int v_r;
	int alivep;
};

struct flare null_flare={0, 0, 0, 0, 0, 0, 0};
struct flare vector_flare[MAX_NUM_FLARE];

void clear_flare(){
	for(int i=0;i<MAX_NUM_FLARE;i++){
		vector_flare[i]=null_flare;
	}
}

void create_flare(int x, int y, int r_begin, int r_end, int life_time){
	int i=0;
	while( (vector_flare[i].alivep != 0)
			&& i<MAX_NUM_FLARE ){
		i++;
	}
	vector_flare[i].x = x;
	vector_flare[i].y = y;
	vector_flare[i].r_begin = r_begin;
	vector_flare[i].r_end = r_end;
	vector_flare[i].life_time = life_time;
	vector_flare[i].v_r = (r_end-r_begin)/(life_time/20); // increase r_begin by step
	vector_flare[i].alivep = 1;
}

void expand_flare(struct flare *flare_current){
	(*flare_current).r_begin += (*flare_current).v_r;
	(*flare_current).life_time -= 20; // refresh time

	/* shutdown flare */
	if( ((*flare_current).life_time)<=0 ){
		(*flare_current).alivep = 0;
	}
}

void expand_flare_all(){
	for(int i=0;i<MAX_NUM_FLARE;i++){
		if(vector_flare[i].alivep){
			expand_flare( &vector_flare[i] );
		}
	}
}

void show_flare(struct flare flare_current){
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glTranslatef(flare_current.x, flare_current.y, 0);
		glutSolidSphere(flare_current.r_begin, 20, 10);
	glPopMatrix();
}

void show_flare_all(){
	for(int i=0;i<MAX_NUM_FLARE;i++){
		if(vector_flare[i].alivep){
			show_flare(vector_flare[i]);
		}
	}
}
/* ------------------ Flare End -----------------------*/

void game_init(void){
	// Clear Memory
	clear_bullet_all();
	clear_enemy();
	clear_flare();


	check_fire_status_player();
	enemy_generator();
}

void game_reset(){
	clear_bullet_all();
	clear_enemy();
	clear_flare();

	teapot_location_x = -350;
	teapot_location_y = 0;
	teapot_alivep = 1;
	game_time = 0;
	kill_counter = 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(	0, 0, 2000,
			0, 0, 0,
			0, 1, 0);

	if(teapot_alivep){
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(teapot_location_x, teapot_location_y, 0);
		glutWireTeapot(50);
		glPopMatrix();

		/*
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(teapot_location_x, teapot_location_y, 0);
		glutWireCube(100);
		glPopMatrix();
		*/
	}

	show_bullet_all();
	show_enemy_all();
	show_flare_all();

	glutSwapBuffers();
}

void init(void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
}

void teapot_location_limiter(float speed_x, float speed_y){
	/*
	if( (teapot_location_x>=-380) && (teapot_location_x <= 380) ){
		teapot_location_x += speed_x;
	}
	if( (teapot_location_y>=-180) && (teapot_location_y <= 200) ){
		teapot_location_y += speed_y;
	}
	*/

	if( (speed_x>0) && (teapot_location_x>380) ) speed_x = 0;
	if( (speed_x<0) && (teapot_location_x<-380)) speed_x = 0;

	if( (speed_y>0) && (teapot_location_y>180) ) speed_y = 0;
	if( (speed_y<0) && (teapot_location_y<-180)) speed_y = 0;

	teapot_location_x += speed_x;
	teapot_location_y += speed_y;
}

void move_teapot(void)
{
	float teapot_moving_speed = 9;
	float teapot_moving_speed_s = 7;

	if( keys['w'] && keys['a'] ){
		teapot_location_limiter(-teapot_moving_speed_s, +teapot_moving_speed_s);
		//teapot_location_x -= teapot_moving_speed_s;
		//teapot_location_y += teapot_moving_speed_s;
	}else if( keys['w'] && keys['d'] ){
		teapot_location_limiter(+teapot_moving_speed_s, +teapot_moving_speed_s);
		//teapot_location_x += teapot_moving_speed_s;
		//teapot_location_y += teapot_moving_speed_s;
	}else if( keys['s'] && keys['a'] ){
		teapot_location_limiter(-teapot_moving_speed_s, -teapot_moving_speed_s);
		//teapot_location_x -= teapot_moving_speed_s;
		//teapot_location_y -= teapot_moving_speed_s;
	}else if( keys['s'] && keys['d'] ){
		teapot_location_limiter(+teapot_moving_speed_s, -teapot_moving_speed_s);
		//teapot_location_x += teapot_moving_speed_s;
		//teapot_location_y -= teapot_moving_speed_s;
	}
	//single
	else if(keys['w']){
		teapot_location_limiter(0, +teapot_moving_speed);
		//teapot_location_y += teapot_moving_speed;
	}else if(keys['s']){
		teapot_location_limiter(0, -teapot_moving_speed);
		//teapot_location_y -= teapot_moving_speed;
	}else if(keys['a']){
		teapot_location_limiter(-teapot_moving_speed, 0);
		//teapot_location_x -= teapot_moving_speed;
	}else if(keys['d']){
		teapot_location_limiter(+teapot_moving_speed, 0);
		//teapot_location_x += teapot_moving_speed;
	}else{
		return;
	}
}

void keyboard_down(unsigned char key, int x, int y)
{
	switch(key){
		case 'w':
			keys['w']=1;
			break;
		case 's':
			keys['s']=1;
			break;
		case 'a':
			keys['a']=1;
			break;
		case 'd':
			keys['d']=1;
			break;
		case 'j':
			keys['j']=1;
			break;
	}
}

void keyboard_up(unsigned char key, int x, int y)
{
	switch(key){
		case 'w':
			keys['w']=0;
			break;
		case 's':
			keys['s']=0;
			break;
		case 'a':
			keys['a']=0;
			break;
		case 'd':
			keys['d']=0;
			break;
		case 'j':
			keys['j']=0;
			break;
	}
}

void refresh(int x)
{
	{ // Teapot
		move_teapot();
	}

	{ // Bullets
		moving_all_bullet();
	}

	{ // Enemy
		moving_all_enemy();
		check_enemy_health_all();
	}

	{ // Game
		check_collision_all();
		expand_flare_all();
	}

	glutPostRedisplay();

	glutTimerFunc(20, refresh, 0);
	//printf("hi\n");
}

void reshape_func(int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	/*
	glLoadIdentity();
	gluOrtho2D(0,800,0,400);
	glMatrixMode(GL_MODELVIEW);
	*/


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-400,400,-200,200,
			1800, 2200);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (800, 400);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Utah Teapot War");
	init ();
	glutDisplayFunc(display);
	glutTimerFunc(20, refresh, 0);
	glutKeyboardUpFunc(keyboard_up);
	glutKeyboardFunc(keyboard_down);
	glutReshapeFunc(reshape_func);

	game_init();

	glutMainLoop();
	return 0;
}

