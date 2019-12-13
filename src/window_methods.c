#include "..\include\window_methods.h"
#include "simple_logger.h"
#include "game.h"
#include "gf3d_ui.h"

void testClick(UIElement* self, UIElement* mouseEle)
{
	slog("Text box clicked!");
	mouseEle->hidden = !(mouseEle->hidden);
	self->hidden = !(self->hidden);
}

void startButtonClick(UIElement* self, UIElement* mouseEle)
{
	mainMenuToLevelOne();
	mouseEle->hidden;
}
