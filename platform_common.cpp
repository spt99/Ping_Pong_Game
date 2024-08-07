struct Button_State {
	bool is_clicked;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_W,
	BUTTON_S,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_COUNT,
	BUTTON_ENTER,

};

struct Input
{
	Button_State buttons[BUTTON_COUNT];
};