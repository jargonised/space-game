#include "primlib.h"
#include <stdlib.h>
#include <time.h>

#define num_targets 3

struct target{
  int x;
  int y;
  int x_speed;
  int y_speed;
  int should_move_left; //czy za prawą krawędź
  int should_move_up; //czy za połowę ekranu w dół
  int is_hit;
  float explosion_scale;
  int color;
};


void draw_cannon(int x_cannon, int y_cannon, double angle, double delta_angle);

void draw_target(struct target *t);
void move_target(struct target *t);

void draw_bullet(int x_bullet, int y_bullet, int x_cannon);

void draw_explosion(int x, int y, float explosion_scale);



int main() {
  if (gfx_init()) //musi być wywołane przed jakimikolwiek operacjami graficznymi
    exit(3);

  int x_cannon = gfx_screenWidth() / 2;
  int y_cannon = gfx_screenHeight();
  double angle = 90.0 * (M_PI/180.0);

  int x_bullet;
  int y_bullet;

  double bullet_distance; //odległość pocisku
  int is_shooting = 0; //czy strzelamy
  double fire_angle; //kąt strzelania
  int is_bullet_fired = 0;


// CEL
  struct target t[num_targets];
//////////////////////////////////////////////////////////
  t[0].x = 20;
  t[0].y = 200;
  t[0].x_speed = 3;
  t[0].y_speed = 0;
  t[0].should_move_left = 0;
  t[0].should_move_up = 0;
  t[0].is_hit = 0;
  t[0].explosion_scale = 0.0;
  t[0].color = MAGENTA;
//////////////////////////////////////////////////////////
  t[1].x = 20;
  t[1].y = 220;
  t[1].x_speed = 2;
  t[1].y_speed = 1;
  t[1].should_move_left = 0;
  t[1].should_move_up = 0;
  t[1].is_hit = 0;
  t[1].explosion_scale = 0.0;
  t[1].color = BLACK;
//////////////////////////////////////////////////////////
  t[2].x = 20;
  t[2].y = 240;
  t[2].x_speed = 4;
  t[2].y_speed = 3;
  t[2].should_move_left = 0;
  t[2].should_move_up = 0;
  t[2].is_hit = 0;
  t[2].explosion_scale = 0.0;
  t[2].color = GREEN;


  //float explosion_scale = 0.0;

  while(1) //zawsze się wykona
  {
 // rysowanie nowej klatki
    srand((unsigned int)time (0)); //losowanie od czasu zegara


    double delta_angle = 2.0 * (M_PI/180.0); //szerokość lufy
  
    
   
    int x_distance_hypot;
    int y_distance_hypot;


// TŁO - rysowanie
// lewy górny róg w (0, 0)
// lewy prawy róg w (gfx_screenWidth() - 1, gfx_screenHeight() - 1)
    gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLUE);

/*    
// DZIAŁO - rysowanie podstawy
// środek okręgu w (gfx_screenWidth() / 2, gfx_screenHeight())
// promień = 100
    gfx_filledCircle(gfx_screenWidth() / 2, gfx_screenHeight(), 100, YELLOW);
    
// LUFA - rysowanie
// współrzędne trójkąta
    gfx_filledTriangle(gfx_screenWidth() / 2, gfx_screenHeight() , gfx_screenWidth() / 2 + x1_barrel, gfx_screenHeight() - y1_barrel,
                       gfx_screenWidth() / 2 + x2_barrel, gfx_screenHeight() - y2_barrel, YELLOW);
*/

// DZIAŁO
    draw_cannon(x_cannon, y_cannon, angle, delta_angle);

// CEL - rysowanie

    for(int i = 0; i < num_targets; ++i)
    {
     draw_target(&(t[i]));
    }
   
// POCISK względem ukł. kart. (top left corner)
    
    int x_bullet_tlc = x_cannon + x_bullet;
    int y_bullet_tlc = gfx_screenHeight() - y_bullet;


    if(is_shooting /*&& !is_serie*/) //nie rysowanie pozycji pocisku po trafieniu i eksplozji //pyt3
    {
      x_bullet = bullet_distance * cos(fire_angle);
      y_bullet = bullet_distance * sin(fire_angle);
      draw_bullet(x_bullet, y_bullet, x_cannon);
    }

    gfx_updateScreen();

// koniec rysowania nowej klatki
    
    if(is_shooting)
    {
      for(int i = 0; i < num_targets; ++i)
      {
        x_distance_hypot = x_bullet_tlc - t[i].x;
        y_distance_hypot = y_bullet_tlc - t[i].y;

        if(hypot((x_distance_hypot), (y_distance_hypot)) <= 70)
        {
          t[i].is_hit = 1;
          is_shooting = 0;
        }
      }
    }

    for(int i = 0; i < num_targets; ++i)
    {
      if(t[i].is_hit)
      {
        bullet_distance += 0.0;
        
        if(t[i].explosion_scale < 3.0) //maksymalny rozmiar eksplozji //pyt1
          t[i].explosion_scale += 0.1; //powiększenie eksplozji
        else
        {
          t[i].x = 0;
          t[i].is_hit = 0;
          t[i].explosion_scale = 0.0;
        }
      }
    }
    
    for(int i = 0; i < num_targets; ++i)
    {
      if(is_shooting && !t[i].is_hit)
        bullet_distance += 3.0; //5.0
    }
    

    if((y_bullet_tlc) > 1) //czy się mieścimy na wysokość
    {
      if((x_bullet_tlc < (gfx_screenWidth() + 1)) && is_shooting) //czy się mieścimy do prawej krawędzi
      {
        if((x_bullet_tlc > -1) && is_shooting) //czy się mieścimy do lewej krawędzi
          is_bullet_fired = 1; //mieścimy się w całym oknie
        else
          is_bullet_fired = 0;
      }
      else
        is_bullet_fired = 0;
    }
    else //gdy się nie mieścimy
      is_bullet_fired = 0; //zerujemy stan wystrzału, by strzelić ponownie


    for (int i = 0; i < num_targets; ++i)
      move_target(&(t[i]));


    if(gfx_isKeyDown(SDLK_RIGHT) && x_cannon < (gfx_screenWidth() + 1))
      x_cannon += 3;
     //angle -= 1.0 * (M_PI/180.0);
    if(gfx_isKeyDown(SDLK_LEFT) && x_cannon > 1)
      x_cannon -= 3;
     //angle += 1.0 * (M_PI/180.0);
    if(gfx_isKeyDown(SDLK_e) && x_cannon < (gfx_screenWidth() + 1))
      angle -= 1.0 * (M_PI/180.0);
    if(gfx_isKeyDown(SDLK_q) && x_cannon > 1)
      angle += 1.0 * (M_PI/180.0);
    if(gfx_isKeyDown(SDLK_SPACE) && !is_bullet_fired)
    {
     is_shooting = 1;
     bullet_distance = 170.0;
     fire_angle = angle;
     //x_bullet = x_cannon; //początkowy x_bullet
    }
    SDL_Delay(10);
  };
  return 0;
}

void draw_cannon(int x_cannon, int y_cannon, double angle, double delta_angle)
{
    int x1_barrel = 150 * cos(angle-delta_angle); //x1 LUFY
    int y1_barrel = 150 * sin(angle-delta_angle); //y1 LUFY
    int x2_barrel = 150 * cos(angle+delta_angle); //x2 LUFY
    int y2_barrel = 150 * sin(angle+delta_angle); //y2 LUFY
 
// DZIAŁO - rysowanie podstawy
// środek okręgu w (gfx_screenWidth() / 2, gfx_screenHeight())
// promień = 100
    gfx_filledCircle(x_cannon, y_cannon, 100, YELLOW);
  
// LUFA - rysowanie
// współrzędne trójkąta
    gfx_filledTriangle(x_cannon, y_cannon, x_cannon + x1_barrel, y_cannon - y1_barrel,
                       x_cannon + x2_barrel, y_cannon - y2_barrel, YELLOW); 

}


void draw_target(struct target *t)
{
  if(!t->is_hit){
    gfx_filledRect(t->x - 30, t->y - 40, t->x - 20, t->y - 30,
                   t->color);
    gfx_filledRect(t->x + 30, t->y - 40, t->x + 40, t->y - 30,
                   t->color);
    gfx_filledRect(t->x - 20, t->y - 30, t->x - 10, t->y - 20,
                   t->color);
    gfx_filledRect(t->x + 20, t->y - 30, t->x + 30, t->y - 20,
                   t->color);
    gfx_filledRect(t->x - 30, t->y - 20, t->x + 40, t->y - 10,
                   t->color);
    gfx_filledRect(t->x - 40, t->y - 10, t->x - 20, t->y,
                   t->color);
    gfx_filledRect(t->x - 10, t->y - 10, t->x + 20, t->y,
                   t->color);
    gfx_filledRect(t->x + 30, t->y - 10, t->x + 50, t->y,
                   t->color);
    gfx_filledRect(t->x - 50, t->y, t->x + 60, t->y + 10,
                   t->color);
    gfx_filledRect(t->x - 50, t->y + 10, t->x - 40, t->y + 40,
                   t->color);
    gfx_filledRect(t->x - 30, t->y + 10, t->x + 40, t->y + 30,
                   t->color);
    gfx_filledRect(t->x + 50, t->y + 10, t->x + 60, t->y + 30,
                   t->color);
    gfx_filledRect(t->x - 30, t->y + 30, t->x - 20, t->y + 40,
                   t->color);
    gfx_filledRect(t->x + 30, t->y + 30, t->x + 40, t->y + 40,
                   t->color);
    gfx_filledRect(t->x - 20, t->y + 40, t->x, t->y + 50,
                   t->color);
    gfx_filledRect(t->x + 10, t->y + 40, t->x + 30, t->y + 50,
                   t->color);
   


  }
  else
    draw_explosion(t->x, t->y, t->explosion_scale);
}

void move_target(struct target *t)
{
  if(!t->is_hit)
    {
      t->x_speed = rand() % 3 + 3;
      t->y_speed = rand() % 3 + 3;
      t->x_speed -= rand() % 3;
      t->y_speed -= rand() % 3;

    //////////////////////////////////////////////////////////
      if(t->should_move_left == 0) //czy ruch w lewo
      {
        t->x += t->x_speed; //domyślnie w prawo
      }
      else
      {
        t->x -= t->x_speed;
      }
      //////////////////////////////////////////////////////////
      if(t->should_move_up == 0)//czy ruch w górę
      {
        t->y += t->y_speed;
      }
      else
      {
        t->y -= t->y_speed;
      }
      //////////////////////////////////////////////////////////
      if(t->x > gfx_screenWidth() - 10)
        t->should_move_left = 1; //zmiana kierunku ruchu

      if(t->x < 10)
        t->should_move_left = 0;
      //////////////////////////////////////////////////////////
      if(t->y > (gfx_screenHeight()/2))
        t->should_move_up = 1;

      if(t->y < 10)
        t->should_move_up = 0;
    }
}



void draw_bullet(int x_bullet, int y_bullet, int x_cannon)
{
    int x_bullet_tlc = x_cannon + x_bullet;
    int y_bullet_tlc = gfx_screenHeight() - y_bullet;
    gfx_filledCircle(x_bullet_tlc, y_bullet_tlc, 10, RED); //POCISK
    

}




void draw_explosion(int x, int y, float explosion_scale)
{
  gfx_filledTriangle(x - 7 * explosion_scale, y - 7 * explosion_scale, x + 7 * explosion_scale,
                     y + 7 * explosion_scale, x + 22 * explosion_scale, y - 22 * explosion_scale, RED);
  gfx_filledTriangle(x - 7 * explosion_scale, y - 7 * explosion_scale, x + 7 * explosion_scale,
                     y + 7 * explosion_scale, x - 22 * explosion_scale, y + 22 * explosion_scale, RED);
  gfx_filledTriangle(x - 7 * explosion_scale, y + 7 * explosion_scale, x + 7 * explosion_scale,
                     y - 7 * explosion_scale, x - 22 * explosion_scale, y - 22 * explosion_scale, RED);
  gfx_filledTriangle(x - 7 * explosion_scale, y + 7 * explosion_scale, x + 7 * explosion_scale,
                     y - 7 * explosion_scale, x + 22 * explosion_scale, y + 22 * explosion_scale, RED);

  gfx_filledTriangle(x - 13 * explosion_scale, y, x + 13 * explosion_scale, y, x,
                     y - 35 * explosion_scale, RED);
  gfx_filledTriangle(x - 13 * explosion_scale, y, x + 13 * explosion_scale, y, x,
                     y + 35 * explosion_scale, RED);
  gfx_filledTriangle(x, y - 13 * explosion_scale, x, y + 13 * explosion_scale, x - 35 * explosion_scale,
                     y, RED);
  gfx_filledTriangle(x, y - 13 * explosion_scale, x, y + 13 * explosion_scale, x + 35 * explosion_scale,
                     y, RED);

  gfx_filledTriangle(x - 5 * explosion_scale-1, y - 5 * explosion_scale-1, x + 5 * explosion_scale-1,
                     y + 5 * explosion_scale-1, x + 16 * explosion_scale-1, y - 16 * explosion_scale-1, YELLOW);
  gfx_filledTriangle(x - 5 * explosion_scale-1, y - 5 * explosion_scale-1, x + 5 * explosion_scale-1,
                     y + 5 * explosion_scale-1, x - 16 * explosion_scale-1, y + 16 * explosion_scale-1, YELLOW);
  gfx_filledTriangle(x - 5 * explosion_scale-1, y + 5 * explosion_scale-1, x + 5 * explosion_scale-1,
                     y - 5 * explosion_scale-1, x - 16 * explosion_scale-1, y - 16 * explosion_scale-1, YELLOW);
  gfx_filledTriangle(x - 5 * explosion_scale-1, y + 5 * explosion_scale-1, x + 5 * explosion_scale-1,
                     y - 5 * explosion_scale-1, x + 16 * explosion_scale-1, y + 16 * explosion_scale-1, YELLOW);

  gfx_filledTriangle(x - 9 * explosion_scale-1, y, x + 9 * explosion_scale-1, y, x,
                     y - 25 * explosion_scale-1, YELLOW);
  gfx_filledTriangle(x - 9 * explosion_scale-1, y, x + 9 * explosion_scale-1, y, x,
                     y + 25 * explosion_scale-1, YELLOW);
  gfx_filledTriangle(x, y - 9 * explosion_scale-1, x, y + 9 * explosion_scale-1, x - 25 * explosion_scale-1,
                     y, YELLOW);
  gfx_filledTriangle(x, y - 9 * explosion_scale-1, x, y + 9 * explosion_scale-1, x + 25 * explosion_scale-1,
                     y, YELLOW);
}
