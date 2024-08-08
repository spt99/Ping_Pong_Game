#define is_clicked(b) input->buttons[b].is_clicked
#define pressed(b) (input->buttons[b].changed && input->buttons[b].is_clicked)
#define released(b) (input->buttons[b].changed && !input->buttons[b].is_clicked)


//float player_pos_x = 0.f;
//float player_pos_y = 0.f;
//float player_p = 0.f;
float player_p_1 = 0.f;
float player_dp_1 = 0.f;
float player_p_2 = 0.f;
float player_dp_2 = 0.f;
float arena_half_size_x = 85;
float arena_half_size_y = 45;
float player_half_size_y = 12;
float player_half_size_x = 2.5;
float ball_p_x; 
float ball_p_y;
float ball_dp_x= 2.f;
float ball_dp_y;
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
	player_ddp_1 -= *player_dp_1 + 1000.f;
	// s = ut + at^2/2
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
			if (is_clicked(BUTTON_UP)) player_ddp_1 += 2000000000000000;
			if (is_clicked(BUTTON_DOWN)) player_ddp_1 -= 2000000000000000;
		}
		else 
		{
			if (ball_p_y > player_p_1)player_ddp_1 += 2000000000000000;
			if (ball_p_y < player_p_1)player_ddp_1 -= 2000000000000000;
		}


		float player_ddp_2 = 0.f;
		float speed_1 = 50.f;
		float speed_2 = 50.f; //5000000.f;
		if (is_clicked(BUTTON_UP))
		{
			player_ddp_1 += 2000000000000000;
		}
		if (is_clicked(BUTTON_DOWN))
		{
			player_ddp_1 -= 2000000000000000;
		}
		if (is_clicked(BUTTON_W))
		{
			player_ddp_2 += 2000000000000000;
		}
		if (is_clicked(BUTTON_S))
		{
			player_ddp_2 -= 2000000000000000;
		}


		simulate_player(&player_p_1, &player_dp_1, player_ddp_1, dt);
		simulate_player(&player_p_2, &player_dp_2, player_ddp_2, dt);


		/*if (is_clicked(BUTTON_DOWN))
		{
			player_ddp -= speed*dt;
		}*/
		//dt = 0.166;
		// ball_movement
		//float dt_for_ball = dt * 4;
		float idt = 0.05;
		ball_p_x += ball_dp_x * idt;
		ball_p_y += ball_dp_y * idt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);
		//collision of ball

		if (ball_p_x + ball_half_size > 80 - player_half_size_x &&
			ball_p_x - ball_half_size < 80 + player_half_size_x &&
			ball_p_y + ball_half_size > player_p_1 - player_half_size_y &&
			ball_p_y + ball_half_size < player_p_1 + player_half_size_y)
		{
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			//ball_dp_y = player_dp_1* 0.0000001f*0.75f;
			ball_dp_y = ((ball_p_y - player_p_1) + player_dp_1) * 0.00000001f;
		}
		else if (ball_p_x + ball_half_size > -80 - player_half_size_x &&
			ball_p_x - ball_half_size < -80 + player_half_size_x &&
			ball_p_y + ball_half_size > player_p_2 - player_half_size_y &&
			ball_p_y + ball_half_size < player_p_2 + player_half_size_y)
		{
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = ((ball_p_y - player_p_2) + player_dp_1) * 0.0000001f * 0.75f;
		}

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

		//display_score
		draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);
		/*float at_x = -80;

		for(int i = 0; i < player_1_score; i++)
		{
			draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
			at_x += 2.5f;
		}

		float at_x_2 = 80;

		for (int i = 0; i < player_2_score; i++)
		{
			draw_rect(at_x_2, 47.f, 1.f, 1.f, 0xaaaaaa);
			at_x_2 -= 2.5f;
		}*/
		//draw_rect(player_pos_x, player_pos_y, 1, 1, 0xffff52);


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