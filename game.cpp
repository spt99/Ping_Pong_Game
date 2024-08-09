#define is_clicked(b) input->buttons[b].is_clicked
#define pressed(b) (input->buttons[b].changed && input->buttons[b].is_clicked)
#define released(b) (input->buttons[b].changed && !input->buttons[b].is_clicked)


//float player_pos_x = 0.f;
//float player_pos_y = 0.f;
//float player_p = 0.f;
float player_p_1;
float player_dp_1;
float player_p_2;
float player_dp_2;
float arena_half_size_x = 85;
float arena_half_size_y = 45;
float player_half_size_y = 12;
float player_half_size_x = 2.5;
float ball_p_x; 
float ball_p_y;
float ball_dp_x= 70;
float ball_dp_y = 0;
float ball_half_size=1.f;

float player_1_score = 0;
float player_2_score = 0;

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
};
int hot_button;
bool enemy_is_ai;

Gamemode current_game_mode = GM_MENU;

static void simulate_player(float * player_p_1, float * player_dp_1, float player_ddp_1, float dt)
{
	//added friction
	player_ddp_1 -= *player_dp_1*0.5f;
	// s = s0+  ut + at^2/2
	*player_p_1 = *player_p_1 + (*player_dp_1 * dt) + (player_ddp_1 * dt * dt * 0.5f);
	//v = u +at;
	*player_dp_1 = *player_dp_1 + player_ddp_1 * dt;


	//equation for collision check

	if (*player_p_1 + player_half_size_y > arena_half_size_y)
	{
		*player_p_1 = arena_half_size_y - player_half_size_y;
		*player_dp_1 = 0;
	}
	else if (*player_p_1 - player_half_size_y < -arena_half_size_y)
	{
		*player_p_1 = -arena_half_size_y + player_half_size_y;
		*player_dp_1 = 0;
	}
}




static void simulate_game(Input* input, float dt)
{
	
	clear_screen(0xff5500);
	//arena
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
	
	if (current_game_mode == GM_GAMEPLAY)
	{
		//moving rectangles
		float player_ddp_1 = 0.f;
 		if(!enemy_is_ai)
		{
			if (is_clicked(BUTTON_UP)) player_ddp_1 += 700;
			if (is_clicked(BUTTON_DOWN)) player_ddp_1 -= 700;
		}
		else 
		{
			//if (ball_p_y > player_p_1*1.f)player_ddp_1 += 500;
			//if (ball_p_y < player_p_1*1.f)player_ddp_1 -= 500;
			player_ddp_1 = (ball_dp_y - player_p_1) * 100;
			if (player_ddp_1 > 700) player_ddp_1 = 700;
			if (player_ddp_1 < - 700) player_ddp_1 = -700;

		}


		float player_ddp_2 = 0.f;
		float speed_1 = 50.f;
		float speed_2 = 50.f; //5000000.f;
		
		if (is_clicked(BUTTON_W))
		{
			player_ddp_2 += 700;
		}
		if (is_clicked(BUTTON_S))
		{
			player_ddp_2 -= 700;
		}


		simulate_player(&player_p_1, &player_dp_1, player_ddp_1, dt);
		simulate_player(&player_p_2, &player_dp_2, player_ddp_2, dt);

		// simulate ball 
		{
			//drwaing a running ball 
			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;


			//collision of ball

			// 1. collison with player 1, 80 is where the palyer1 starts.
			// player 1 is left palyer, so ball left side should never insert into left_player_1 rigth most side
			// ball left side (ball_p_x + ball_half_size)
			// player_1 rigth most side, i.e.80 - player_half_size_x
			if (ball_p_x + ball_half_size > 80 - player_half_size_x && // this condition when player1 collide the ball
				ball_p_x - ball_half_size < 80 + player_half_size_x && // this condition so that if panel moves up in that position no collition
				ball_p_y + ball_half_size > player_p_1 - player_half_size_y &&
				ball_p_y + ball_half_size < player_p_1 + player_half_size_y)
			{
				ball_p_x = 80 - player_half_size_x - ball_half_size;
				ball_dp_x *= -1; // invert ball velocity
				// player_dp_1 * 0.75f; = ball_dp_y means that the y velocity player hits the ball influences ball velocity too.
				//(ball_p_y - player_p_1)*2 means differnec in ball hirtgh and player hirgth also gives a factor for ball_y_velocity
				//combine both
				ball_dp_y = (ball_p_y - player_p_1) + player_dp_1 * 0.75f;
			}
			// collison with player 2 
			else if (ball_p_x + ball_half_size > -80 - player_half_size_x &&
				ball_p_x - ball_half_size < -80 + player_half_size_x &&
				ball_p_y + ball_half_size > player_p_2 - player_half_size_y &&
				ball_p_y + ball_half_size < player_p_2 + player_half_size_y)
			{
				ball_p_x = -80 + player_half_size_x + ball_half_size;
				ball_dp_x *= -1;
				ball_dp_y = (ball_p_y - player_p_2) + player_dp_1 * 0.75f;
			}

			// collision with arena upper and lower wall.
			if (ball_p_y + ball_half_size > arena_half_size_y)
			{
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;
			}
			else if (ball_p_y - ball_half_size < -arena_half_size_y)
			{
				ball_p_y = -arena_half_size_y + ball_half_size;
				ball_dp_y *= -1;
			}

			// to add score and when ba; goes out of arena update ball position and velocity starting from center.
			if (ball_p_x + ball_half_size > arena_half_size_x)
			{
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_1_score++;
			}
			else if (ball_p_x - ball_half_size < -arena_half_size_x)
			{
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_2_score++;
			}
		}

		//display_score
		draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);
		
		// rendering : drawing ball player1, player 2

		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);
		draw_rect(80, player_p_1, player_half_size_x, player_half_size_y, 0xff0000);
		draw_rect(-80, player_p_2, player_half_size_x, player_half_size_y, 0xff0000);
	}
	else if (current_game_mode == GM_MENU) {
		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT))
		{
			hot_button = !hot_button;
		}
		
		if (pressed(BUTTON_ENTER))
		{
			current_game_mode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}
		draw_text("PING PONG GAME", -55, 40, 1.4, 0xb22222);
		draw_text("CHOOSE PLAY MODE", -35, 20, 0.7, 0xb22222);

		draw_text("SINGLE PLAYER", -60, -25, 0.6, 0xff0000);
		draw_text("MULTIPLAYER", 20, -25, 0.6, 0xff0000);
		draw_rect(40, -10, 10, 10, 0xcccccc);
		draw_rect(-40, -10, 10, 10, 0xcccccc);
		if (hot_button == 0) {
			
			draw_rect(-40, -10, 9,9, 0x191970);
		}
		else {
			draw_rect(40, -10, 9, 9, 0x191970);
			
		}
		
	

	}

}