#include "ConsoleView.h"
#include "ConsoleController.h"
#include "ConsoleModel.h"
#include "graphics/Graphics.h"
#include "ConsoleCommand.h"
#include "gui/interface/Label.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/Engine.h"
#include "SimulationConfig.h"
#include <deque>
#include <SDL.h>

ConsoleView::ConsoleView():
	ui::Window(ui::Point(0, 0), ui::Point(WINDOWW, 150)),
	commandField(nullptr)
{
	commandField = new ui::Textbox(ui::Point(0, Size.Y-16), ui::Point(Size.X, 16), "");
	commandField->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	commandField->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	commandField->SetActionCallback({ [this] { commandField->SetDisplayText(c->FormatCommand(commandField->GetText())); } });
	AddComponent(commandField);
	FocusComponent(commandField);
	commandField->SetBorder(false);
}

void ConsoleView::DoKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt)
{
	if ((ui::Engine::Ref().GraveExitsConsole && scan == SDL_SCANCODE_GRAVE && key != '~') || key == SDLK_ESCAPE || key == SDLK_AC_BACK)
	{
		if (!repeat)
			doClose = true;
		return;
	}
	switch(key)
	{
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		c->EvaluateCommand(commandField->GetText());
		commandField->SetText("");
		commandField->SetDisplayText("");
		break;
	case SDLK_DOWN:
		c->NextCommand();
		break;
	case SDLK_UP:
		if (editingNewCommand)
		{
			newCommand = commandField->GetText();
		}
		c->PreviousCommand();
		break;
	default:
		Window::DoKeyPress(key, scan, repeat, shift, ctrl, alt);
		break;
	}
}

void ConsoleView::DoTextInput(String text)
{
	if (text == "~")
		doClose = false;
	if (!doClose)
		Window::DoTextInput(text);
}

void ConsoleView::NotifyPreviousCommandsChanged(ConsoleModel * sender)
{
	for (size_t i = 0; i < commandList.size(); i++)
	{
		RemoveComponent(commandList[i]);
		delete commandList[i];
	}
	commandList.clear();
	std::deque<ConsoleCommand> commands = sender->GetPreviousCommands();
	int currentY = Size.Y - 32;
	if(commands.size())
		for(int i = commands.size()-1; i >= 0; i--)
		{
			if(currentY <= 0)
				break;
			ui::Label * tempLabel = new ui::Label(ui::Point(Size.X/2, currentY), ui::Point(Size.X/2, 16), commands[i].ReturnValue);
			tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
			tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
			commandList.push_back(tempLabel);
			AddComponent(tempLabel);
			tempLabel = new ui::Label(ui::Point(0, currentY), ui::Point(Size.X/2, 16), commands[i].Command);
			tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
			tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
			commandList.push_back(tempLabel);
			AddComponent(tempLabel);
			currentY-=16;
		}
}

void ConsoleView::NotifyCurrentCommandChanged(ConsoleModel * sender)
{
	bool oldEditingNewCommand = editingNewCommand;
	editingNewCommand = sender->GetCurrentCommandIndex() >= sender->GetPreviousCommands().size();
	if (!oldEditingNewCommand && editingNewCommand)
	{
		commandField->SetText(newCommand);
	}
	else
	{
		commandField->SetText(sender->GetCurrentCommand().Command);
	}
	commandField->SetDisplayText(c->FormatCommand(commandField->GetText()));
}


void ConsoleView::OnDraw()
{
	Graphics * g = GetGraphics();
	g->BlendFilledRect(RectSized(Position, Size), 0x000000_rgb .WithAlpha(110));
	g->BlendLine(Position + Vec2{ 0, Size.Y-16 }, Position + Size - Vec2{ 0, 16 }, 0xFFFFFF_rgb .WithAlpha(160));
	g->BlendLine(Position + Vec2{ 0, Size.Y }, Position + Size, 0xFFFFFF_rgb .WithAlpha(200));
}

void ConsoleView::OnTick()
{
	if (doClose)
	{
		c->CloseConsole();
		doClose = false;
	}
}

ConsoleView::~ConsoleView()
{
}

