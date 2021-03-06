#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <gc/gc.h>
#define malloc(n) GC_MALLOC(n)

#define frame_time 20

/*
#define enemy_size_2 50
#define teapot_size_x_2 50
#define teapot_size_y_2 40
*/
#define enemy_size_2 20
#define teapot_size_x_2 15
#define teapot_size_y_2 12

int window_width;
int window_height;

int game_pause_p=0;

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
	struct bullet *next;
};

struct bullet null_bullet =
{ 0, 0, 0, 0, 0, 0, 0};

struct bullet *bullet_lst;

int counter_bullet = 0;

void create_bullet
(float x, float y, float v_x, float v_y, int damage, int evilp)
{
	struct bullet *new_bullet = (struct bullet *)malloc(sizeof(struct bullet));
	new_bullet->x = x;
	new_bullet->y = y;
	new_bullet->v_x = v_x;
	new_bullet->v_y = v_y;
	new_bullet->damage = damage;
	new_bullet->evilp = evilp;
	new_bullet->alivep = 1;
	new_bullet->next = bullet_lst;
	bullet_lst = new_bullet;

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
	create_bullet(teapot_location_x + 1.2*teapot_size_x_2,
			teapot_location_y + 0.8*teapot_size_y_2,
			30, rand_y + opa,
			5, 0);
}

void check_fire_status_player()
{
	if(keys['j']){
		openfire_player();
	}
}

void delete_bullet(struct bullet *p)
{
	struct bullet head;
	head.next = bullet_lst;
	struct bullet *prev = &head;
	while(prev){
		if(prev->next == p){
			prev->next = p->next;
			break;
		}
		prev = prev->next;
	}
	bullet_lst = head.next;
}

void clear_bullet()
{
	bullet_lst = NULL;
}

void moving_all_bullet()
{
	struct bullet *p = bullet_lst;
	struct bullet *tmp = NULL;
	while(p){
		p->x += p->v_x;
		p->y += p->v_y;
		/*Shutdown bullet*/
		if( (p->x >= 480) ||
				(p->x <= -410) ||
				(p->y >= 250) ||
				(p->y <= -210) ){
			tmp = p->next;
			delete_bullet(p);
			p = tmp;
		}else{
			p = p->next;
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

	glVertex3f(x, y, 0);
	glVertex3f(x-v_x, y-v_y, 0);
}

void show_bullet_all(){
	struct bullet *p = bullet_lst;

	glLineWidth(2);
	const GLfloat flare_bullet_color[] = { 1, 1, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, flare_bullet_color);
	glBegin(GL_LINES);
		while(p){
			show_bullet(p);
			p = p->next;
		}
	glEnd();
	glLineWidth(1);
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
	struct enemy *next;
};

struct enemy null_enemy={0,0, 0,0, 0,0, 0, NULL};

struct enemy *enemy_lst = NULL;

void clear_enemy(){
	enemy_lst = NULL;
}

void delete_enemy(struct enemy *p)
{
	struct enemy head;
	head.next = enemy_lst;
	struct enemy *prev = &head;
	while(prev){
		if(prev->next == p){
			prev->next = p->next;
			break;
		}else{
			prev = prev->next;
		}
	}
	enemy_lst = head.next;
}

void create_enemy
(float x, float y, float v_x, float v_y, int type, int hp)
{
	struct enemy *new_enemy = (struct enemy*)malloc(sizeof(struct enemy));
	new_enemy->x = x;
	new_enemy->y = y;
	new_enemy->v_x = v_x;
	new_enemy->v_y = v_y;
	new_enemy->type = type;
	new_enemy->alivep = 1;
	new_enemy->hp = hp;
	new_enemy->next = enemy_lst;
	enemy_lst = new_enemy;
}

void create_enemy_randomly(){
	int y_rand = rand()%480 - 260;
	int type_rand = rand()%7;
	float enemy_speed = -4;
	create_enemy(500, y_rand,
			enemy_speed, 0,
			type_rand, 80);
}

int game_time=0;
int gen_arry[50];
void enemy_generator(){
	if(game_time<120){
		game_time += 5;
	}

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
}

void check_enemy_generator(){
	static int time_accumulator = 5000;
	time_accumulator +=20;

	if(time_accumulator>=5000){
		time_accumulator=0;
		enemy_generator();
	}
}

void generate_enemy(){
	static int time_accumulator = 0;
	static int i=0;

	time_accumulator += 20;
	if(time_accumulator>=100){
		time_accumulator = 0;
		if(i>=50) i = 0;

		if(gen_arry[i]){
			create_enemy_randomly();
		}
		i++;
	}
}

void moving_all_enemy()
{
	struct enemy *p = enemy_lst;
	struct enemy *tmp = NULL;
	while(p){
		//move
		p->x += p->v_x;
		p->y += p->v_y;

		//shutdown enemy
		if( 		(p->x >= 510) ||
				(p->x <= -510) ||
				(p->y >= 280) ||
				(p->y <= -280) )
		{
			tmp = p->next;
			delete_enemy(p);
			p = tmp;
		} else {
			p = p->next;
		}
	}
}

float* enemy_color_get(int type){
	float enemy_color0[] = { 0.1, 0.1, 0.5, 1.0 };
	float enemy_color1[] = { 0.1, 0.5, 0.1, 1.0 };
	float enemy_color2[] = { 0.1, 0.5, 0.5, 1.0 };
	float enemy_color3[] = { 0.5, 0.1, 0.1, 1.0 };
	float enemy_color4[] = { 0.5, 0.1, 0.5, 1.0 };
	float enemy_color5[] = { 0.5, 0.5, 0.1, 1.0 };
	float enemy_color6[] = { 0.5, 0.5, 0.5, 1.0 };
	float *enemy_color_v[7];
	enemy_color_v[0]=enemy_color0;
	enemy_color_v[1]=enemy_color1;
	enemy_color_v[2]=enemy_color2;
	enemy_color_v[3]=enemy_color3;
	enemy_color_v[4]=enemy_color4;
	enemy_color_v[5]=enemy_color5;
	enemy_color_v[6]=enemy_color6;

	return enemy_color_v[type];
}

void show_enemy(struct enemy *enemy_current){
	float x = (*enemy_current).x;
	float y = (*enemy_current).y;

	GLfloat color_vector[4];
	float *color_vector_template=enemy_color_get((*enemy_current).type);
	for(int i=0;i<4;i++){
		color_vector[i]=color_vector_template[i];
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_vector);

	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidCube(enemy_size_2 * 2);
	glPopMatrix();
}

void show_enemy_all(){
	struct enemy *p = enemy_lst;
	while(p){
		show_enemy(p);
		p = p->next;
	}
}

int kill_counter=0;
void check_enemy_health_all(){
	struct enemy *p = enemy_lst;
	struct enemy *tmp = NULL;
	while(p){
		if(p->hp <= 0){
			create_flare(p->x,
					p->y,
					50, 100, 300);
			tmp = p->next;
			delete_enemy(p);
			p = tmp;
			kill_counter++;
		}else{
			p = p->next;
		}
	}
}

/* ------------------ Enemy END -----------------------*/

/* ------------------ Enemy Bullet --------------------*/

struct enemy_bullet{
	float x;
	float y;
	float v_x;
	float v_y;
	int type;
	struct enemy_bullet *next;
};

struct enemy_bullet *enemy_bullet_lst = NULL;

void delete_enemy_bullet(struct enemy_bullet *p)
{
	struct enemy_bullet head;
	head.next = enemy_bullet_lst;
	struct enemy_bullet *prev = &head;
	while(prev){
		if(prev->next == p){
			prev->next = p->next;
			break;
		}
		prev = prev->next;
	}
	enemy_bullet_lst = head.next;
}

void clear_enemy_bullet(){
	enemy_bullet_lst = NULL;
}

void create_enemy_bullet
(float x, float y, float v_x, float v_y, int type)
{
	struct enemy_bullet *new =
		(struct enemy_bullet*)
		malloc(sizeof(struct enemy_bullet));
	new->x = x;
	new->y = y;
	new->v_x = v_x;
	new->v_y = v_y;
	new->type = type;
	new->next = enemy_bullet_lst;
	enemy_bullet_lst = new;
}

void enemy_fire_bullet_all()
{
	struct enemy *p = enemy_lst;
	while(p){
		if(p->type == 2){
			float x = p->x;
			float y = p->y;
			float a = teapot_location_x;
			float b = teapot_location_y;
			int xs = 1; int ys = 1;
			if(a-x < 0){ xs = -1; }
			if(b-y < 0){ ys = -1; }

			float tan = (b-y)/(a-x);
			float angle = atanf(tan);
			float v = 5;
			float v_x = fabs(cosf(angle)) * xs * v;
			float v_y = fabs(sinf(angle)) * ys * v;

			/* 普通惯性系狙击弹 */
			create_enemy_bullet
				(p->x,
				 p->y,
				 v_x + p->v_x, // 注意这里的区别！
				 v_y,
				 p->type);
		}else if(p->type == 6){
			float x = p->x;
			float y = p->y;
			float a = teapot_location_x;
			float b = teapot_location_y;
			int xs = 1; int ys = 1;
			if(a-x < 0){ xs = -1; }
			if(b-y < 0){ ys = -1; }

			float tan = (b-y)/(a-x);
			float angle = atanf(tan);
			float v = 2;
			float v_x = fabs(cosf(angle)) * xs * v;
			float v_y = fabs(sinf(angle)) * ys * v;

			/* 惯性系错觉自机狙！ */
			create_enemy_bullet
				(p->x,
				 p->y,
				 v_x,
				 v_y,
				 p->type);
		}
		p = p->next;
	}
}

void moving_all_enemy_bullet()
{
	struct enemy_bullet *p = enemy_bullet_lst;
	struct enemy_bullet *tmp = NULL;
	while(p){
		p->x += p->v_x;
		p->y += p->v_y;
		/*Shutdown bullet*/
		if( (p->x >= 480) ||
				(p->x <= -410) ||
				(p->y >= 250) ||
				(p->y <= -210) ){
			tmp = p->next;
			delete_enemy_bullet(p);
			p = tmp;
		}else{
			p = p->next;
		}
	}
}

void show_enemy_bullet(struct enemy_bullet *p){
	float x = p->x;
	float y = p->y;

	GLfloat color_vector[4];
	float *color_vector_template=enemy_color_get(p->type);
	for(int i=0;i<4;i++){
		color_vector[i]=color_vector_template[i];
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_vector);

	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidSphere(4, 10, 10);
	glPopMatrix();
}

void show_enemy_bullet_all()
{
	struct enemy_bullet *p = enemy_bullet_lst;
	while(p){
		show_enemy_bullet(p);
		p = p->next;
	}
}

/* ------------------ Enemy Bullet END ----------------*/

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
	int x1 = (*enemy_current).x-enemy_size_2;
	int y1 = (*enemy_current).y-enemy_size_2;
	int x2 = (*enemy_current).x+enemy_size_2;
	int y2 = (*enemy_current).y+enemy_size_2;
	struct bullet *p = bullet_lst;
	struct bullet *tmp = NULL;
	while(p){
		int xa = p->x - p->v_x;
		int ya = p->y - p->v_y;
		int xb = p->x;
		int yb = p->y;

		if( collidedp(x1, y1, x2, y2, xa, ya, xb, yb) ){
			(*enemy_current).hp -= p->damage;
			tmp = p->next;
			delete_bullet(p);
			create_flare(xb - 24, yb, 5, 40, 100);
			p = tmp;
		}else{
			p = p->next;
		}
	}
}

void collision_enemy_and_teapot( struct enemy *enemy_current ){
	int x1= teapot_location_x - teapot_size_x_2;
	int y1= teapot_location_y - teapot_size_y_2;
	int x2= teapot_location_x + teapot_size_x_2;
	int y2= teapot_location_y + teapot_size_y_2;

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

void collision_enemy_bullet_and_teapot(){
	int x = teapot_location_x;
	int y = teapot_location_y;

	int bx; int by;
	float distance_power_2;

	struct enemy_bullet *p = enemy_bullet_lst;
	while(p){
		bx = p->x;
		by = p->y;
		distance_power_2 = powf(bx-x, 2) + powf(by-y, 2);
		if(distance_power_2 <= powf(teapot_size_x_2/2, 2)){
			teapot_alivep=0;
			create_flare(teapot_location_x, teapot_location_y, 30, 300, 500);
			glutTimerFunc(500, game_reset, 0);
		}
		p = p->next;
	}
}

void check_collision_all(){
	struct enemy *p = enemy_lst;
	while(p){
		collision_enemy_and_teapot(p);
		collision_enemy_and_bullet(p);
		p = p->next;
	}
	collision_enemy_bullet_and_teapot();
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
	struct flare *next;
};

struct flare null_flare={0, 0, 0, 0, 0, 0, 0};
struct flare vector_flare[MAX_NUM_FLARE];

struct flare *flare_lst = NULL;

void clear_flare(){
	flare_lst = NULL;
}

void delete_flare(struct flare *p){
	struct flare head;
	head.next = flare_lst;
	struct flare *prev = &head;
	while(prev){
		if(prev->next == p){
			prev->next = p->next;
			break;
		}else{
			prev = prev->next;
		}
	}
	flare_lst = head.next;
}

void create_flare
(int x, int y, int r_begin, int r_end, int life_time){
	struct flare *new =
		(struct flare*)malloc(sizeof(struct flare));

	new->x = x;
	new->y = y;
	new->r_begin = r_begin;
	new->r_end = r_end;
	new->life_time = life_time;
	// increase r_begin by step
	new->v_r = (r_end-r_begin)/(life_time/20);
	new->alivep = 1;
	new->next = flare_lst;
	flare_lst = new;
}

void expand_flare_all(){
	struct flare *p = flare_lst;
	struct flare *tmp = NULL;
	while(p){
		// expand flare
		p->r_begin += p->v_r;
		p->life_time -= 20;

		// shutdown flare
		if(p->life_time <= 0){
			tmp = p->next;
			delete_flare(p);
			p = tmp;
		}else{
			p = p->next;
		}
	}
}

void show_flare(struct flare *flare_current){
	glPushMatrix();
		glTranslatef(flare_current->x, flare_current->y, 0);
		glutSolidSphere(flare_current->r_begin, 20, 10);
	glPopMatrix();
}

void show_flare_all(){
	struct flare *p = flare_lst;
	const GLfloat flare_bullet_color[] = { 1, 1, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, flare_bullet_color);
	while(p){
		show_flare(p);
		p = p->next;
	}
}
/* ------------------ Flare End -----------------------*/

/* ------------------ Game ----------------------------*/
void game_reset(){
	clear_bullet();
	clear_enemy();
	clear_flare();
	clear_enemy_bullet();

	for(int i=0;i<50;i++){
		gen_arry[i] = 0;
	}

	teapot_location_x = -350;
	teapot_location_y = 0;
	teapot_alivep = 1;
	game_time = 0;
	kill_counter = 0;
}

void check_game_pause(){
	static int stick_time = 0;
	static int key_dead_time = 0;

	if(key_dead_time>=0){
		key_dead_time -= frame_time;
	}

	if(keys['p'] && (key_dead_time<=0)){
		stick_time+=frame_time;
		if(stick_time>=50){
			game_pause_p = !game_pause_p;
			stick_time = 0;
			key_dead_time= 300;
		}
	}
}

/* ------------------ Game ----------------------------*/

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat light0_position[] = { 500 , 500, 1000, 0 };
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glViewport(0,0, window_width, window_height);
	glLoadIdentity();
	gluLookAt(	0, 0, 2000,
			0, 0, 0,
			0, 1, 0);

	const GLfloat teapot_color[] = { 0.7, 0.2, 0.2, 1.0 };
	if(teapot_alivep){
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, teapot_color);
		glTranslatef(teapot_location_x, teapot_location_y, 0);
		glutSolidTeapot(teapot_size_x_2);
		glPopMatrix();
	}

	show_bullet_all();
	show_flare_all();
	show_enemy_all();
	show_enemy_bullet_all();

	glutSwapBuffers();
}

void teapot_location_limiter(float speed_x, float speed_y){
	if( (speed_x>0) && (teapot_location_x>380) ) speed_x = 0;
	if( (speed_x<0) && (teapot_location_x<-380)) speed_x = 0;

	if( (speed_y>0) && (teapot_location_y>180) ) speed_y = 0;
	if( (speed_y<0) && (teapot_location_y<-180)) speed_y = 0;

	teapot_location_x += speed_x;
	teapot_location_y += speed_y;
}

void move_teapot(void)
{
	float teapot_moving_speed = 6;
	float teapot_moving_speed_s = 4.5;

	if( keys['w'] && keys['a'] ){
		teapot_location_limiter(-teapot_moving_speed_s, +teapot_moving_speed_s);
	}else if( keys['w'] && keys['d'] ){
		teapot_location_limiter(+teapot_moving_speed_s, +teapot_moving_speed_s);
	}else if( keys['s'] && keys['a'] ){
		teapot_location_limiter(-teapot_moving_speed_s, -teapot_moving_speed_s);
	}else if( keys['s'] && keys['d'] ){
		teapot_location_limiter(+teapot_moving_speed_s, -teapot_moving_speed_s);
	}
	//single
	else if(keys['w']){
		teapot_location_limiter(0, +teapot_moving_speed);
	}else if(keys['s']){
		teapot_location_limiter(0, -teapot_moving_speed);
	}else if(keys['a']){
		teapot_location_limiter(-teapot_moving_speed, 0);
	}else if(keys['d']){
		teapot_location_limiter(+teapot_moving_speed, 0);
	}else{
		return;
	}
}

void keyboard_down(unsigned char key, int x, int y)
{
	keys[key]=1;
}

void keyboard_up(unsigned char key, int x, int y)
{
	keys[key]=0;
}

int enemy_fire_timer = 0;
void refresh(int x)
{
	if(!game_pause_p){
		{ // Teapot
			move_teapot();
			check_fire_status_player();
		}

		{ // Bullets
			moving_all_bullet();
		}

		{ // Enemy
			enemy_fire_timer += 20;
			if(enemy_fire_timer>=1000){
				enemy_fire_bullet_all();
				enemy_fire_timer = 0;
			}
			moving_all_enemy();
			moving_all_enemy_bullet();
			check_enemy_health_all();
			generate_enemy();
			check_enemy_generator();
		}

		{ // Game
			check_collision_all();
			expand_flare_all();
		}
	}

	check_game_pause();

	glutPostRedisplay();

	glutTimerFunc(20, refresh, 0);
}

void reshape_func(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-400,400,-200,200,
			1800, 2200);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

void gl_init(){
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (800, 400);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Utah Teapot War");
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape_func);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	GLfloat light_color[] = {0.8, 0.8, 0.8, 0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_color);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	gl_init();

	glutTimerFunc(20, refresh, 0);
	glutKeyboardUpFunc(keyboard_up);
	glutKeyboardFunc(keyboard_down);

	game_reset();

	glutMainLoop();
	return 0;
}

