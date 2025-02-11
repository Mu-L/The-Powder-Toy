#pragma once
#include "gui/interface/Window.h"
#include <vector>

namespace ui
{
	class Button;
	class Textbox;
	class Label;
}

class TagsController;
class TagsModel;
class TagsView: public ui::Window {
	TagsController * c;
	ui::Button * addButton;
	ui::Button * closeButton;
	ui::Label * title;
	ui::Textbox * tagInput;
	std::vector<ui::Component*> tags;
	void addTag();
public:
	TagsView();
	void OnDraw() override;
	void OnTick() override;
	void AttachController(TagsController * c_) { c = c_; }
	void OnKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;
	void NotifyTagsChanged(TagsModel * sender);
};
