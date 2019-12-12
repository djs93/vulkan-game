#include "..\include\window_methods.h"
#include "simple_logger.h"

void testClick(UIElement* self, UIElement* mouseEle)
{
	slog("Text box clicked!");
	mouseEle->hidden = !(mouseEle->hidden);
}
