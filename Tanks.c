#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
const sfVector2f basepos={952,1000};

typedef enum{
	PLAYER,
	ENEMY,
	FULLWALL,
	HALFWALL,
	QWALL,
	BULLET
}objtype; 

typedef enum{
	UP,
	RIGHT,
	DOWN,
	LEFT,
}vector;

struct movingObject{
	objtype type;
	vector direction;
	sfSprite* sprite;
};
sfSprite* AddSprite( sfIntRect rect, sfTexture *texture){
	sfSprite *sprite;
	sprite = sfSprite_create();
	sfSprite_setTexture(sprite, texture, sfTrue);
	sfSprite_setTextureRect(sprite, rect);
	sfSprite_setScale(sprite, {5,5});
	return sprite;
}
int intersection(movingObject* s1, movingObject* s2, sfFloatRect *intersect){
	int i;
	sfFloatRect r1={0}, r2={0};
	r1=sfSprite_getGlobalBounds(s1->sprite);
	if(r1.left<0||r1.top<0||r1.left>1920-r1.width||r1.top>1080-r1.height) return 2;
	r2=sfSprite_getGlobalBounds(s2->sprite);
	i=sfFloatRect_intersects( &r1, &r2, intersect);
	return i; 
};

sfVector2f DrawWall(sfRenderWindow* window, sfVector2f startPos, vector vector, struct movingObject *strBrWall, int lenght, int *Wall_arr_count)//1=horizontal,0=Vertical. Wall_arr_count number of last painted WAll sprite
{
	sfVector2f pos;
	pos=startPos;
	lenght = lenght+(*Wall_arr_count);
	for((*Wall_arr_count); (*Wall_arr_count)<lenght ;(*Wall_arr_count)++){
		sfSprite_setPosition(strBrWall[*Wall_arr_count].sprite, pos);
		sfRenderWindow_drawSprite(window,  strBrWall[*Wall_arr_count].sprite, NULL);
		if(vector==RIGHT)pos.x+=40;
		if(vector==UP)pos.y-=40;
		if(vector==LEFT)pos.x-=40;
		if(vector==DOWN)pos.y+=40;
	}
	return pos;
};
movingObject Shooting(movingObject* ARRbullets, movingObject* Player){
	int i;
	movingObject Bullet;
	sfFloatRect PlayerPlace, BullPlace;
	sfVector2f BullPos;
	PlayerPlace=sfSprite_getGlobalBounds(Player->sprite);
	for(i=0;i<4;i++){
		if(Player->direction==ARRbullets[i].direction)Bullet=ARRbullets[i];
	}
	BullPlace=sfSprite_getGlobalBounds(Bullet.sprite);
	if(Player->direction==UP){
		BullPos.x=PlayerPlace.left+(PlayerPlace.width/2-BullPlace.width/2-2);
		BullPos.y=PlayerPlace.top-1-BullPlace.height;
	}
	if(Player->direction==RIGHT){
		BullPos.x=PlayerPlace.left+PlayerPlace.width+1;
		BullPos.y=PlayerPlace.top+(PlayerPlace.height/2-BullPlace.height/2-2);
	}
	if(Player->direction==DOWN){
		BullPos.x=PlayerPlace.left+(PlayerPlace.width/2-BullPlace.width/2-2);
		BullPos.y=PlayerPlace.top+PlayerPlace.height+1;
	}
	if(Player->direction==LEFT){
		BullPos.x=PlayerPlace.left-1-BullPlace.width;
		BullPos.y=PlayerPlace.top+(PlayerPlace.height/2-BullPlace.height/2)-2;
	}
	sfSprite_setPosition(Bullet.sprite,BullPos);
	return Bullet;
};

void Bullmove(movingObject *Bullet, float speed){
	if(Bullet->direction==RIGHT) sfSprite_move ((*Bullet).sprite, {speed,0});		
	if(Bullet->direction==UP) sfSprite_move (Bullet->sprite, {0,-speed});
	if(Bullet->direction==DOWN) sfSprite_move (Bullet->sprite, {0,speed});	
	if(Bullet->direction==LEFT) sfSprite_move (Bullet->sprite, {-speed,0});	
};

sfVector2f Objmove(movingObject *obj, float objSpeed){
	sfFloatRect bound;
	sfVector2f pos;
	bound=sfSprite_getGlobalBounds(obj->sprite);
	pos.x=bound.left;
	pos.y=bound.top;
	if(obj->direction==UP) pos.y = bound.top - objSpeed;
	if(obj->direction==RIGHT) pos.x = bound.left + objSpeed;
	if(obj->direction==DOWN) pos.y = bound.top + objSpeed;
	if(obj->direction==LEFT) pos.x = bound.left - objSpeed;
	sfSprite_setPosition(obj->sprite,pos);
	return pos;
};
movingObject copyObj(movingObject Obj){
	movingObject copiedObj;
	copiedObj=Obj;
	copiedObj.sprite=sfSprite_copy(Obj.sprite);
	return copiedObj;
};
movingObject AddObj(vector vec, sfIntRect rect, sfTexture* texture, objtype type){
	movingObject newObj;
	newObj.direction=vec;
	newObj.sprite=AddSprite(rect, texture);
	newObj.type=type;
	return newObj;
};
int tankIntrsc(movingObject *tank, movingObject *terrain, sfVector2f *tankPos, float tankSpeed, sfFloatRect *intersect){
	if(intersection(tank, terrain, intersect)){
		if(tank->direction==UP) tankPos->y+=tankSpeed;
		if(tank->direction==DOWN) tankPos->y-=tankSpeed;
		if(tank->direction==RIGHT) tankPos->x-=tankSpeed;
		if(tank->direction==LEFT) tankPos->x+=tankSpeed;
		if(intersect!=NULL && intersect->height<=20 && intersect->width <= 20){
			if(tank->direction==UP || tank->direction==DOWN){
				if(tankPos->x==intersect->left) tankPos->x+=intersect->width;
				else tankPos->x-=intersect->width;
			}
			if(tank->direction==RIGHT || tank->direction==LEFT){
				if(tankPos->y == intersect->top) tankPos->y+=intersect->height;
				else tankPos->y-=intersect->height;
			}
		}
		sfSprite_setPosition(tank->sprite, *tankPos);
		return 1;
	}
	return 0;
}
vector Relativity(movingObject enemy, movingObject player){
	vector relativity;
	sfVector2f enpos, plpos;
	float distX, distY, distX2, distY2;
	enpos = sfSprite_getPosition(enemy.sprite);
	plpos = sfSprite_getPosition(player.sprite);
	distX=enpos.x-plpos.x;
	distY=enpos.y-plpos.y;
	distX2=enpos.x-basepos.x;
	distY2=enpos.y-basepos.y;
	if(fabsf(distX) + fabsf(distY) > fabsf(distX2 ) + fabsf(distY2)) {	
		distX=distX2;
		distY=distY2;
	}
	if( fabsf(distX) > fabsf(distY) ){
		if( distX<0 ) relativity=RIGHT;
		if( distX>0 ) relativity=LEFT;
	}
	else{
		if( distY<0 ) relativity=DOWN;
		if( distY>0 ) relativity=UP;
	}
	return relativity;
}
void EnemyRotation(movingObject *enemy, movingObject *rotationSpr, float *count, movingObject *player, float time, int coef){
	float random;
	float n1=25, n2=50, n3=75;
	vector relativity = Relativity(*enemy, *player);
	if(*count==0){ 
		if(relativity==UP) n1+=coef, n2+=coef*0.666, n3+=coef*0.333;
		if(relativity==RIGHT) n1-=coef, n2+=coef*0.666, n3+=coef*0.333;
		if(relativity==DOWN) n1-=coef, n2-=coef*0.666, n3+=coef*0.333;
		if(relativity==LEFT) n1-=coef*0.333, n2-=coef*0.666, n3-=coef;
		random=rand()%101;
		if(random >= 0 && random < n1) *enemy=copyObj(rotationSpr[0]); 
		if(random >= n1 && random < n2) *enemy=copyObj(rotationSpr[1]); 
		if(random >= n2 && random < n3) *enemy=copyObj(rotationSpr[2]); 
		if(random >= n3 && random <=100) *enemy=copyObj(rotationSpr[3]);
		};
	(*count)+=1*time;
	if((*count)>2000) (*count) = 0;
};
void Explosion( movingObject *ExplSpr, sfVector2f position, float ExplTime, sfRenderWindow *window){
	if(ExplTime>0.01 && ExplTime <= 0.13) {sfSprite_setPosition(ExplSpr[0].sprite, position); sfRenderWindow_drawSprite(window, ExplSpr[0].sprite, NULL);;}
	if(ExplTime>0.13 && ExplTime <= 0.26) {sfSprite_setPosition(ExplSpr[1].sprite, position); sfRenderWindow_drawSprite(window, ExplSpr[1].sprite, NULL);;}
	if(ExplTime>0.26 && ExplTime <= 0.39) {sfSprite_setPosition(ExplSpr[2].sprite, position); sfRenderWindow_drawSprite(window, ExplSpr[2].sprite, NULL);;}
}
int DobleWallHit(movingObject* wall, movingObject* bullet, int *WI/*wallindex*/, movingObject* Hlfwalls, sfSprite* sprEmpty){
	int i, j;
	for(j=0;j<4;j++) { if(bullet->direction==(Hlfwalls[j].direction)) break; }
	
	if(WI[1]<0){
		if(wall[WI[0]].type==FULLWALL) wall[WI[0]]=copyObj(Hlfwalls[j]);
		else wall[WI[0]].sprite=sprEmpty;
		bullet->sprite=sprEmpty;
	}	
	else{
		if( wall[WI[0]].type==FULLWALL && wall[WI[1]].type==FULLWALL){
			wall[WI[0]] = copyObj(Hlfwalls[j]);
			wall[WI[1]] = copyObj(Hlfwalls[j]);
			bullet->sprite=sprEmpty;
		}
		else if( wall[WI[0]].type==HALFWALL && wall[WI[1]].type==HALFWALL){
			if(wall[WI[0]].direction!=bullet->direction && wall[WI[1]].direction==bullet->direction){wall[WI[0]].sprite=sprEmpty; bullet->sprite=sprEmpty;}	
			else if(wall[WI[1]].direction!=bullet->direction && wall[WI[0]].direction==bullet->direction){wall[WI[1]].sprite=sprEmpty; bullet->sprite=sprEmpty;}	
			else {wall[WI[0]].sprite=sprEmpty; wall[WI[1]].sprite=sprEmpty;}		
		}
		else{	
			if(wall[WI[0]].type==FULLWALL) {
				wall[WI[0]] = copyObj(Hlfwalls[j]);
				if(wall[WI[1]].direction!=bullet->direction) wall[WI[1]].sprite=sprEmpty;
			}
			if(wall[WI[1]].type==FULLWALL ) {
				wall[WI[1]] = copyObj(Hlfwalls[j]);
				if(wall[WI[0]].direction!=bullet->direction) wall[WI[1]].sprite=sprEmpty;
			}
		}
		bullet->sprite=sprEmpty;
	}
	WI[0]=-10;
	WI[1]=-10;
}




int main(){
 srand(time(0));
 sfVideoMode mode = {1920, 1080, 32};
 sfRenderWindow* window;
 sfTexture* texture;
 sfSprite *BrWall_spr, *sprEmpty;
// sfFont* font;
// sfText* text;
// sfMusic* music;
 sfEvent event;
 int i, walls=200, Wall_arr_count=0, texX=288,j,texY=64, coefficient=0, wallindex[2];
 float plSpeed=0.3, enemySpeed=0.1, turnCount[3]={0,0,0}, plShotFrequency=10, EnShotFrequency[3]={10,10,10}, ExplTime=10, EnBullExplTime[3]={10,10,10};
 float BulletSpeed=0.8;
 sfVector2f pos={240,240}, EnemyPos[3], StEnemyPos[3], PlayerPos={822,1000}, StartPlayerPos={822,1000}, ExplPos, EnBullExplPos[3];
 sfFloatRect PlayerPlace={0}, intersect, StEnemyArea[3];
 movingObject Player, Bullet, BulletsEnemy[3], VBullet[4], Enemy[3], EnemySpr[4], strBrWall[200], Hlfwalls[4];
 movingObject PlUp, PlRight, PlLeft, PlDown, ExplSpr[3], Base, BaseDestroyed, GameOver;
 vector BullDirection;
 bool shot; 
 sfKeyCode pressedKey;
 sfClock *clock;
 
 
 clock=sfClock_create();

 
 window = sfRenderWindow_create(mode, "SFML window", sfFullscreen | sfResize | sfClose, NULL);
 	if (!window)
 	return 1;
 texture = sfTexture_createFromFile("BattleCitySpriteSheet.png", NULL);
 	if (!texture)
	return 2;

/*Create_srpite*/
GameOver=AddObj(UP, {288,184,32,16}, texture, PLAYER);
sfSprite_setPosition(GameOver.sprite,{1920/2-16*5,1080/2-8*5});
Base=AddObj(UP, {304,32,16,16}, texture, PLAYER);
sfSprite_setPosition(Base.sprite,{952,1000});
BaseDestroyed=AddObj(UP, {320,32,16,16}, texture, PLAYER);
sfSprite_setPosition(BaseDestroyed.sprite,{952,1000});
StEnemyPos[0]=EnemyPos[0]={0,0};
StEnemyPos[1]=EnemyPos[1]={880,0};
StEnemyPos[2]=EnemyPos[2]={1840,0};
StEnemyArea[0]={0,0,80,80};
StEnemyArea[1]={880,0,80,80};
StEnemyArea[2]={1840,0,80,80};
j=256;
for(i=0;i<3;i++){
	ExplSpr[i]= AddObj(UP, {j,128,16,16}, texture, PLAYER);
	sfSprite_setOrigin(ExplSpr[i].sprite, {8,8});
	j=j+16;}
{
 PlUp = AddObj(UP, {0,0,16,16}, texture, PLAYER);
 PlRight = AddObj(RIGHT, {96,0,16,16}, texture, PLAYER);
 PlLeft = AddObj(LEFT, {32,0,16,16}, texture, PLAYER);
 PlDown = AddObj(DOWN, {64,0,16,16}, texture, PLAYER);
 Player = PlUp;
 BrWall_spr = AddSprite( {256,64,8,8}, texture);
 sprEmpty = AddSprite( {NULL}, NULL);
 sfSprite_setPosition(sprEmpty,{2000,2000});
 VBullet[0]=AddObj(UP, {323,102,3,4}, texture, BULLET);
 VBullet[1]=AddObj(RIGHT, {346,102,4,3}, texture, BULLET);
 VBullet[2]=AddObj(DOWN, {339,102,3,4}, texture, BULLET);
 VBullet[3]=AddObj(LEFT, {330,102,4,3}, texture, BULLET);
 Bullet.sprite=sprEmpty;
 
 for(i=0;i<walls;i++){
 	strBrWall[i].type=FULLWALL;
	(strBrWall[i].sprite) = (sfSprite_copy(BrWall_spr));
 }
 for(i=0;i<4;i++){
 	int pxX=8,pxY=8;
	Hlfwalls[i].sprite = AddSprite( {texX,texY,pxX,pxY}, texture);
 	Hlfwalls[i].direction=(vector) i;
 	Hlfwalls[i].type = HALFWALL;
 	texX+=8;
 	if(texX==296)texX=264;
 }
texX=224;
for(i=1;i<4;i++){
	EnemySpr[i].sprite=AddSprite({texX,0,16,16},texture);
	EnemySpr[i].direction=(vector)i;
	EnemySpr[i].type=ENEMY;
	texX-=32;
}
	EnemySpr[0].sprite=AddSprite({texX,0,16,16},texture);
	EnemySpr[0].direction=UP;
	EnemySpr[0].type=ENEMY;
}
for(i=0;i<3;i++){
	Enemy[i]=EnemySpr[2];
	Enemy[i].sprite=sfSprite_copy(EnemySpr[2].sprite);
	sfSprite_setPosition(Enemy[i].sprite,StEnemyPos[i]);
	BulletsEnemy[i].sprite=sprEmpty;
}


 while (sfRenderWindow_isOpen(window))
 	{ 
 float time = sfTime_asMicroseconds(sfClock_getElapsedTime(clock));	
 sfClock_restart(clock);
 plShotFrequency+=time/1000000;//time into secnods
 ExplTime+=time/1000000;//250000
 for(i=0;i<3;i++){
 	EnShotFrequency[i]+=time/1000000;
 	EnBullExplTime[i]+=time/1000000;
 }
 time=time/1000;

/* Process events */
sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);
 	while (sfRenderWindow_pollEvent(window, &event))
	{
		if (event.type == sfEvtClosed||event.key.code == sfKeyEscape) sfRenderWindow_close(window);
		if(event.type==sfEvtKeyPressed) pressedKey = event.key.code;	 // Catching last holded key to move in last holded ditection
		if(event.type==sfEvtKeyReleased) {
			if(sfKeyboard_isKeyPressed(sfKeyA) ) pressedKey = sfKeyA;
			if(sfKeyboard_isKeyPressed(sfKeyD) ) pressedKey = sfKeyD;
			if(sfKeyboard_isKeyPressed(sfKeyS) ) pressedKey = sfKeyS;
			if(sfKeyboard_isKeyPressed(sfKeyW) ) pressedKey = sfKeyW;
		}
	}
	if(sfKeyboard_isKeyPressed(sfKeySpace) && Bullet.sprite==sprEmpty && plShotFrequency > 0.7) {shot=1; plShotFrequency=0;}

	if(sfKeyboard_isKeyPressed(sfKeyD)||sfKeyboard_isKeyPressed(sfKeyA)||sfKeyboard_isKeyPressed(sfKeyW)||sfKeyboard_isKeyPressed(sfKeyS)){
		if(sfKeyboard_isKeyPressed(sfKeyA) && pressedKey==sfKeyA) Player = PlLeft;
		if(sfKeyboard_isKeyPressed(sfKeyD) && pressedKey==sfKeyD) Player = PlRight;
		if(sfKeyboard_isKeyPressed(sfKeyS) && pressedKey==sfKeyS) Player = PlDown;
		if(sfKeyboard_isKeyPressed(sfKeyW) && pressedKey==sfKeyW) Player = PlUp;
			sfSprite_setPosition(Player.sprite,PlayerPos);
			PlayerPos = Objmove(&Player, time*plSpeed);
			for(i=0;i<Wall_arr_count;i++) {if(tankIntrsc(&Player, &strBrWall[i], &PlayerPos, time*plSpeed, &intersect)) break;}
			for(i=0;i<3;i++) {if(tankIntrsc(&Player, &Enemy[i], &PlayerPos, time*plSpeed,  NULL)) break; }
		}
	
 /* Clear the screen */
 sfRenderWindow_clear(window, sfBlack);
 sfSprite_setPosition(Player.sprite,PlayerPos);
 sfRenderWindow_drawSprite(window, Player.sprite, NULL);
//Player shooting
if(shot&&Bullet.sprite==sprEmpty && Player.sprite !=sprEmpty){
	Bullet=copyObj(Shooting(VBullet, &Player));
	shot=0;
}
sfRenderWindow_drawSprite(window, Bullet.sprite, NULL);
Bullmove(&Bullet, time*BulletSpeed);

sfRenderWindow_drawSprite(window, Base.sprite, NULL);

wallindex[0]=-1;
wallindex[1]=-1;
for(i=0;i<Wall_arr_count;i++){
	if( intersection(&Bullet, &strBrWall[i], NULL)==2) Bullet.sprite=sprEmpty;
	if( intersection(&Bullet, &strBrWall[i], &intersect)==1){
		ExplPos.x=intersect.left;
		ExplPos.y=intersect.top;
		ExplTime=0.011;
		if( wallindex[0]<0 ) { wallindex[0]=i; continue; }			
		if( wallindex[1]<0 ) wallindex[1]=i;
	}
}
if(wallindex[0]>=0) DobleWallHit( strBrWall, &Bullet, wallindex, Hlfwalls, sprEmpty);

for(i=0;i<3;i++){
	if(Bullet.sprite!=sprEmpty && intersection(&Bullet, &Enemy[i], &intersect)) {
		ExplPos.x=intersect.left;
		ExplPos.y=intersect.top;
		ExplTime=0.011;
		Enemy[i].sprite=sprEmpty;
		Bullet.sprite=sprEmpty;
		EnemyPos[i]=StEnemyPos[i];
		coefficient++;
	}
	if(Bullet.sprite!=sprEmpty && intersection(&Bullet, &BulletsEnemy[i], &intersect)){
		Bullet.sprite=sprEmpty;
		BulletsEnemy[i].sprite=sprEmpty;
	}
}
sfFloatRect Pl=sfSprite_getGlobalBounds(Player.sprite);
sfFloatRect EA;
bool k;
{
for(i=0;i<3;i++){
	if(Enemy[i].sprite==sprEmpty){
		if(!sfFloatRect_intersects( &StEnemyArea[i], &Pl, NULL)) {
			for(j=0;j<3;j++){
				if(j==i) continue;
				EA=sfSprite_getGlobalBounds(Enemy[j].sprite);
				k=sfFloatRect_intersects( &StEnemyArea[i], &EA, NULL) ;
				if(k==sfTrue) break;
			}
			if(k==sfFalse){
				Enemy[i]=copyObj(EnemySpr[2]);
				sfSprite_setPosition(Enemy[i].sprite,StEnemyPos[i]);
			}
		}
	}
	if(Enemy[i].sprite!=sprEmpty){
		sfRenderWindow_drawSprite(window, Enemy[i].sprite, NULL);
		EnemyRotation(&Enemy[i], EnemySpr, &turnCount[i], &Player, time, coefficient);	
		sfSprite_setPosition(Enemy[i].sprite,EnemyPos[i]);
		EnemyPos[i] = Objmove(&Enemy[i],time*enemySpeed);
		for(j=0;j<Wall_arr_count;j++) {if(tankIntrsc(&Enemy[i], &strBrWall[j], &EnemyPos[i], time*enemySpeed,  &intersect)) {turnCount[i]=0;  break;}}
		for(j=0;j<3;j++){
			if(j==i)continue;
			if(tankIntrsc(&Enemy[i], &Enemy[j], &EnemyPos[i], time*enemySpeed,  NULL)) {turnCount[i]=0; break;}
			}
		tankIntrsc(&Enemy[i], &Player, &EnemyPos[i], time*enemySpeed,  NULL);
//		tankIntrsc(&Enemy[i], &Player, &EnemyPos[i], time*enemySpeed, NULL); //Без дублированиня функции танки иногда застревают друг в друге пока не отъедут назад(вероятно изза того что игрок движеся немного дальше за ход)
	}
	}
}
//Enemy shooting
for(i=0;i<3;i++){
	if(Enemy[i].sprite!=sprEmpty){
		int firing=0;
		int firechance=rand()%10000;
		if(EnemyPos[i].x+40 < PlayerPos.x+80 && EnemyPos[i].x+40 > PlayerPos.x+0 && fabs(EnemyPos[i].x-PlayerPos.x)<50) firing = 1;
		if(EnemyPos[i].y+40 < PlayerPos.y+80 && EnemyPos[i].y+40 > PlayerPos.y+0 && fabs(EnemyPos[i].y-PlayerPos.y)<50) firing = 1;
		if(EnemyPos[i].x+40 < basepos.x+80 && EnemyPos[i].x+40 > basepos.x+0 && fabs(EnemyPos[i].x-basepos.x)<100) firing = 1;
		if(EnemyPos[i].y+40 < basepos.y+80 && EnemyPos[i].y+40 > basepos.y+0 && fabs(EnemyPos[i].y-basepos.y)<100) firing = 1;
		if( (firechance < 1+coefficient || 1 == firing) && BulletsEnemy[i].sprite==sprEmpty && EnShotFrequency[i]>1){
			BulletsEnemy[i]=copyObj(Shooting(VBullet, &Enemy[i]));
			EnShotFrequency[i]=0;
			}
		if(Enemy[i].sprite==sprEmpty) BulletsEnemy[i].sprite=sprEmpty;
		sfRenderWindow_drawSprite(window, BulletsEnemy[i].sprite, NULL);
		Bullmove(&BulletsEnemy[i], time*BulletSpeed);
		}
	}
for(i=0;i<3;i++){
	for(j=0;j<Wall_arr_count;j++){
		if( intersection(&BulletsEnemy[i], &strBrWall[j], &intersect)==2) BulletsEnemy[i].sprite=sprEmpty;
		if( intersection(&BulletsEnemy[i], &strBrWall[j], &intersect)==1){
			EnBullExplPos[i].x=intersect.left;
			EnBullExplPos[i].y=intersect.top;
			EnBullExplTime[i]=0.011;
			if( wallindex[0]<0 ) { wallindex[0]=j; continue; }			
			if( wallindex[1]<0 ) wallindex[1]=j;	
		}
	}
	if(wallindex[0]>=0) DobleWallHit( strBrWall, &BulletsEnemy[i], wallindex, Hlfwalls, sprEmpty);
}
for(i=0;i<3;i++){
	if(BulletsEnemy[i].sprite!=sprEmpty && intersection(&BulletsEnemy[i], &Player, &intersect)){
		EnBullExplPos[i].x=intersect.left;
		EnBullExplPos[i].y=intersect.top;
		EnBullExplTime[i]=0.011;
		Player.sprite=sprEmpty;
		BulletsEnemy[i].sprite=sprEmpty;
		PlayerPos=StartPlayerPos;
	}
	if(BulletsEnemy[i].sprite!=sprEmpty && intersection(&BulletsEnemy[i], &Base, &intersect)){
		EnBullExplPos[i].x=intersect.left;
		EnBullExplPos[i].y=intersect.top;
		EnBullExplTime[i]=0.011;
		Base=BaseDestroyed;	
	}
}
//WALLS
{
Wall_arr_count=0;	
pos=DrawWall(window, {300,300}, RIGHT, strBrWall , 10, &Wall_arr_count);
pos=DrawWall(window, pos, DOWN, strBrWall , 10, &Wall_arr_count);
pos=DrawWall(window, {1920/2-(8*6),1080-(8*5)}, UP, strBrWall , 2, &Wall_arr_count);
pos=DrawWall(window, pos, RIGHT, strBrWall , 3, &Wall_arr_count);
pos=DrawWall(window, pos, DOWN, strBrWall , 3, &Wall_arr_count);
//DrawWall(window, {140,140}, UP, strBrWall , 3, &Wall_arr_count);
pos=DrawWall(window, {1920-300,300}, LEFT, strBrWall , 10, &Wall_arr_count);
pos=DrawWall(window, pos, DOWN, strBrWall , 10, &Wall_arr_count);
DrawWall(window, {1920/2-(8*6),400}, RIGHT, strBrWall , 4, &Wall_arr_count);
}
 sfSprite_setPosition(Player.sprite,PlayerPos);
 sfRenderWindow_drawSprite(window, Player.sprite, NULL);
 /* Update the window */
 for(i=0;i<3;i++){
	Explosion(ExplSpr, EnBullExplPos[i], EnBullExplTime[i], window);
 }
 Explosion(ExplSpr, ExplPos, ExplTime, window);
 
 if(Base.sprite==BaseDestroyed.sprite) {
 	sfRenderWindow_drawSprite(window, GameOver.sprite, NULL);
 	plSpeed=0;
 }
  
 sfRenderWindow_display(window);
 }

 /* Cleanup resources */
// sfMusic_destroy(music);
// sfText_destroy(text);
// sfFont_destroy(font);
 sfSprite_destroy(Player.sprite);
 sfSprite_destroy(PlRight.sprite);
 sfSprite_destroy(PlLeft.sprite);
 sfSprite_destroy(PlDown.sprite);
 sfSprite_destroy(PlUp.sprite);
 sfSprite_destroy(Bullet.sprite);
 sfSprite_destroy(BrWall_spr);
for(i=0;i<200;i++){
	  sfSprite_destroy(strBrWall[i].sprite);
}
for(i=0;i<4;i++){
	  sfSprite_destroy(EnemySpr[i].sprite);
}
for(i=0;i<3;i++){
	  sfSprite_destroy(Enemy[i].sprite);
}
for(i=0;i<4;i++){
	  sfSprite_destroy(VBullet[i].sprite);
}
for(i=0;i<4;i++){
	  sfSprite_destroy(Hlfwalls[i].sprite);
}
 sfTexture_destroy(texture);
 sfRenderWindow_destroy(window);
 return 0;
}


