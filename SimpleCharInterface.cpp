//=============================================================
//
// C4 Engine version 3.5.1
// Copyright 1999-2013, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4ConfigData.h"
#include "SimpleChar.h"
#include "SimpleCharInterface.h"

using namespace C4;


namespace
{
	enum
	{
		kServerAliveCount = 10,
		kServerQueryInterval = 1000,

		kChatHistoryCount = 24,
		kMessageLineCount = 6,
		kMessageScrollTime = 3000
	};


	const float kInterfaceScaleHeight = 1088.0F;
}

MainWindow *C4::TheMainWindow = nullptr;
JoinGameWindow *C4::TheJoinGameWindow = nullptr;
//HostGameWindow *C4::TheHostGameWindow = nullptr;

GameWindow::GameWindow(const char *panelName) : Window(panelName)
{
}

GameWindow::~GameWindow()
{
}
//
//StartWindow::StartWindow(const char *panelName, const char *stringName) : GameWindow(panelName)
//{
//	stringTableName = stringName;
//	worldTable = nullptr;
//}
//
//StartWindow::~StartWindow()
//{
//	delete worldTable;
//}
//
//void StartWindow::Preprocess(void)
//{
//	GameWindow::Preprocess();
//	UpdatePreview();
//}
//
//const char *StartWindow::GetSelectedWorldName(unsigned_int32 *stringID) const
//{
//	
//	const Widget *listItem = worldList->GetFirstSelectedListItem();
//	if (listItem)
//	{
//		int32 index = listItem->GetNodeIndex();
//
//		const StringHeader *stringHeader = worldTable->GetRootStringHeader();
//		while (stringHeader)
//		{
//			if (index == 0)
//			{
//				if ((stringHeader->GetFirstSubstring()) && (stringID))
//				{
//					*stringID = stringHeader->GetStringID();
//				}
//
//				return (stringHeader->GetString());
//			}
//
//			index--;
//			stringHeader = stringHeader->GetNextString();
//		}
//	}
//
//	return (nullptr);
//}
//
//void StartWindow::UpdatePreview(void)
//{
//	/*
//	const Widget *listItem = worldList->GetFirstSelectedListItem();
//	if (listItem != worldSelection)
//	{
//		worldSelection = listItem;
//
//		unsigned_int32 id = 0;
//		previewImage->SetTexture(0, GetSelectedWorldName(&id));
//
//		if (id != 0)
//		{
//			titleText->SetText(worldTable->GetString(StringID(id, 'TITL')));
//			authorText->SetText(worldTable->GetString(StringID(id, 'AUTH')));
//		}
//		else
//		{
//			titleText->SetText(nullptr);
//			authorText->SetText(nullptr);
//		}
//	}
//	*/
//}
//
//void StartWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
//{
//	EventType eventType = eventData->eventType;
//
//	if (eventType == kEventWidgetActivate)
//	{
//		
//	}
//
//}
//
//
//HostGameWindow::HostGameWindow() :
//StartWindow("game/Host", "game/MP"),
//Singleton<HostGameWindow>(TheHostGameWindow)
//{
//}
//
//HostGameWindow::~HostGameWindow()
//{
//}
//
//void HostGameWindow::Open(void)
//{
//	if (TheHostGameWindow)
//	{
//		TheInterfaceMgr->SetActiveWindow(TheHostGameWindow);
//	}
//	else
//	{
//		TheGame->AddWindow(new HostGameWindow);
//	}
//}
//
//void HostGameWindow::Preprocess(void)
//{
//	StartWindow::Preprocess();
//
//	gameNameText = static_cast<EditTextWidget *>(FindWidget("Name"));
//	maxPlayersText = static_cast<EditTextWidget *>(FindWidget("Max"));
//	playHereBox = static_cast<CheckWidget *>(FindWidget("Play"));
//
//	gameNameText->SetText(TheEngine->GetVariable("gameName")->GetValue());
//	maxPlayersText->SetText(Text::IntegerToString(TheMessageMgr->GetMaxPlayerCount()));
//}
//
//void HostGameWindow::Move(void)
//{
//	if ((worldList->GetFirstSelectedListItem()) && (gameNameText->GetText()[0] != 0) && (Text::StringToInteger(maxPlayersText->GetText()) > 1))
//	{
//		startButton->Enable();
//	}
//	else
//	{
//		startButton->Disable();
//	}
//
//	StartWindow::Move();
//}
//
//void HostGameWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
//{
//	if ((eventData->eventType == kEventWidgetActivate) && (widget == startButton))
//	{
//		TheGame->ExitCurrentGame();
//
//		const char *gameName = gameNameText->GetText();
//		TheEngine->GetVariable("gameName")->SetValue(gameName);
//		ConfigDataDescription::WriteEngineConfig();
//
//		TheMessageMgr->SetMaxPlayerCount(Text::StringToInteger(maxPlayersText->GetText()));
//		unsigned_int32 flags = 0;//(playHereBox->GetValue() == 1); // ? 1 : 0;//kMultiplayerDedicated : 0;
//
//		ResourceName name(GetSelectedWorldName());
//		delete this;
//
//		TheGame->HostMultiplayerGame(name, flags);
//		MainWindow::ReturnToGame();
//	}
//	else
//	{
//		StartWindow::HandleWidgetEvent(widget, eventData);
//	}
//}
//

JoinGameWindow::JoinGameWindow() :
GameWindow("JoinWindow"),//GameWindow("game/Join"),
Singleton<JoinGameWindow>(TheJoinGameWindow)
{
}


JoinGameWindow::~JoinGameWindow()
{
	/*if (!TheMessageMgr->Multiplayer())
	{
		TheNetworkMgr->Terminate();
	}*/
}


void JoinGameWindow::Open(void)
{
	if (TheJoinGameWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheJoinGameWindow);
	}
	else
	{
		TheGame->AddWindow(new JoinGameWindow);
	}
}
//
//JoinGameWindow::GameWidget::GameWidget(const NetworkAddress& address, const ServerInfoMessage *message) :
//worldName(message->GetWorldName()),
//nameText(Vector2D(206.0F, 13.0F), message->GetGameName(), "font/Gui"),
//playerText(Vector2D(56.0F, 13.0F), (String<31>(message->GetPlayerCount()) += '/') += String<7>(message->GetMaxPlayerCount()), "font/Gui"),
//worldText(Vector2D(136.0F, 13.0F), &worldName[Text::GetDirectoryPathLength(worldName)], "font/Gui")
//{
//	aliveCount = kServerAliveCount;
//	gameAddress = address;
//
//	nameText.SetWidgetPosition(Point3D(2.0F, 0.0F, 0.0F));
//	AddSubnode(&nameText);
//
//	playerText.SetWidgetPosition(Point3D(232.0F, 0.0F, 0.0F));
//	playerText.SetTextAlignment(kTextAlignCenter);
//	AddSubnode(&playerText);
//
//	worldText.SetWidgetPosition(Point3D(320.0F, 0.0F, 0.0F));
//	AddSubnode(&worldText);
//}
//
//JoinGameWindow::GameWidget::~GameWidget()
//{
//}
//
//bool JoinGameWindow::AddressFilter(unsigned_int32 code)
//{
//	return ((code - 0x0030 < 10U) || (code == '.') || (code == ':'));
//}
//

void JoinGameWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	joinButton = static_cast<PushButtonWidget *>(FindWidget("Start"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	addressBox = static_cast<EditTextWidget *>(FindWidget("Address"));
//	gameList = static_cast<ListWidget *>(FindWidget("List"));
//	previewImage = static_cast<ImageWidget *>(FindWidget("Preview"));
//
//	SetFocusWidget(gameList);
//	addressBox->SetFilterProc(&AddressFilter);
//
//	queryTime = 0;
//
//	if (!TheMessageMgr->Multiplayer())
//	{
//		TheNetworkMgr->SetProtocol(kGameProtocol);
//		TheNetworkMgr->SetPortNumber(0);
//		TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
//		TheNetworkMgr->Initialize();
//	}
}
//
//
//void JoinGameWindow::Move(void)
//{
//	if ((queryTime -= TheTimeMgr->GetSystemDeltaTime()) <= 0)
//	{
//		queryTime = kServerQueryInterval;
//		TheMessageMgr->BroadcastServerQuery();
//
//		GameWidget *gameWidget = static_cast<GameWidget *>(gameList->GetFirstListItem());
//		while (gameWidget)
//		{
//			GameWidget *next = static_cast<GameWidget *>(gameWidget->Next());
//
//			if (--gameWidget->aliveCount == 0)
//			{
//				if (gameWidget == gameList->GetFirstSelectedListItem())
//				{
//					previewImage->SetTexture(0, "C4/black");
//				}
//
//				gameList->RemoveListItem(gameWidget);
//				delete gameWidget;
//			}
//
//			gameWidget = next;
//		}
//	}
//
//	GameWidget *listItem = static_cast<GameWidget *>(gameList->GetFirstSelectedListItem());
//	if (listItem)
//	{
//		addressBox->SetText(MessageMgr::AddressToString(listItem->gameAddress, true));
//	}
//
//	if ((listItem) || (addressBox->GetText()[0] != 0))
//	{
//		joinButton->Enable();
//	}
//	else
//	{
//		joinButton->Disable();
//	}
//
//	GameWindow::Move();
//}

void JoinGameWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == joinButton)
		{
			NetworkAddress address;

			TheGame->ExitCurrentGame();
			MainWindow::ReturnToGame();

			TheGame->LoadWorld("world/inout");
			TheMessageMgr->BeginMultiplayerGame(false);

			TheNetworkMgr->SetPortNumber(0);

			address = MessageMgr::StringToAddress(addressBox->GetText());

			address.SetPort(kGamePort);
			
			delete this;
			TheMessageMgr->Connect(address);
		} /*
		if ((widget == joinButton) || (widget == gameList))
		{
			NetworkAddress	address;

			TheGame->ExitCurrentGame();
			MainWindow::ReturnToGame();
			TheMessageMgr->BeginMultiplayerGame(false);

			const Widget *listItem = gameList->GetFirstSelectedListItem();
			if (listItem)
			{
				address = static_cast<const GameWidget *>(listItem)->gameAddress;
			}
			else
			{
				address = MessageMgr::StringToAddress(addressBox->GetText());
				if (address.GetPort() == 0)
				{
					address.SetPort(kGamePort);
				}
			}

			delete this;

			TheMessageMgr->Connect(address);
			//ConnectWindow::Open(address);
			TheGame->LoadWorld("world/inout");
		} */
		else if (widget == cancelButton)
		{
			//CallCompletionProc();
			delete this;
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == addressBox)
		{
			//gameList->UnselectAllListItems();
		}
		/*else if (widget == gameList)
		{
			const Widget *listItem = gameList->GetFirstSelectedListItem();
			if (listItem)
			{
				previewImage->SetTexture(0, static_cast<const GameWidget *>(listItem)->worldName);
			}
			else
			{
				previewImage->SetTexture(0, "C4/black");
			}
		}*/
	}
}
//
//void JoinGameWindow::ReceiveServerInfo(const NetworkAddress& address, const ServerInfoMessage *message)
//{
//	GameWidget *gameWidget = static_cast<GameWidget *>(gameList->GetFirstListItem());
//	while (gameWidget)
//	{
//		if (gameWidget->gameAddress == address)
//		{
//			gameWidget->aliveCount = kServerAliveCount;
//			return;
//		}
//
//		gameWidget = static_cast<GameWidget *>(gameWidget->Next());
//	}
//
//	gameList->AppendListItem(new GameWidget(address, message));
//}


MainWindow::MainWindow() :
GameWindow("StartMenu"),// (TheWorldMgr->GetWorld()) ? "game/Main2" : "game/Main1"),
Singleton<MainWindow>(TheMainWindow)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::Open(void)
{
	if (TheMainWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheMainWindow);
	}
	else
	{
		TheGame->AddWindow(new MainWindow);
	}
}

void MainWindow::ReturnToGame(void)
{
	//TheGame->StopTitleMusic();

	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		world->SetWorldFlags(world->GetWorldFlags() & ~kWorldPaused);
	}

	TheTimeMgr->SetWorldTimeMultiplier(1.0F);
}

/*
void MainWindow::SinglePlayerComplete(StartWindow *window, void *cookie)
{
	Open();
}

void MainWindow::LoadSavedGameComplete(FilePicker *picker, void *cookie)
{
	TheGame->RestoreSinglePlayerGame(picker->GetResourceName());
	ReturnToGame();
}

void MainWindow::SaveCurrentGameComplete(FilePicker *picker, void *cookie)
{
	TheGame->SaveSinglePlayerGame(picker->GetResourceName());
	delete static_cast<MainWindow *>(cookie);
	ReturnToGame();
}
*/
//void MainWindow::HostGameComplete(StartWindow *window, void *cookie)
//{
//	Open();
//}
//
//void MainWindow::JoinGameComplete(JoinGameWindow *window, void *cookie)
//{
//	Open();
//}

/*
void MainWindow::PlayerSettingsComplete(PlayerSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::ControlSettingsComplete(ControlSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::GraphicsSettingsComplete(GraphicsSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::AudioSettingsComplete(AudioSettingsWindow *window, void *cookie)
{
	Open();
}
*/

void MainWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	/*
	newButton = static_cast<TextButtonWidget *>(FindWidget("New"));
	loadButton = static_cast<TextButtonWidget *>(FindWidget("Load"));
	saveButton = static_cast<TextButtonWidget *>(FindWidget("Save"));
	exitButton = static_cast<TextButtonWidget *>(FindWidget("Exit"));
*/
	hostButton = static_cast<TextButtonWidget *>(FindWidget("Host"));
	joinButton = static_cast<TextButtonWidget *>(FindWidget("Join"));

	/*
	playerButton = static_cast<TextButtonWidget *>(FindWidget("Player"));
	controlsButton = static_cast<TextButtonWidget *>(FindWidget("Controls"));
	graphicsButton = static_cast<TextButtonWidget *>(FindWidget("Graphics"));
	audioButton = static_cast<TextButtonWidget *>(FindWidget("Audio"));

	creditsButton = static_cast<TextButtonWidget *>(FindWidget("Credits"));
	quitButton = static_cast<TextButtonWidget *>(FindWidget("Quit"));
*/

	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		if (!TheMessageMgr->Multiplayer())
		{
			//saveButton->Enable();

			world->SetWorldFlags(world->GetWorldFlags() | kWorldPaused);
			TheTimeMgr->SetWorldTimeMultiplier(0.0F);
		}
	}
	else
	{
		//TheGame->StartTitleMusic();
		//TheInterfaceMgr->GetStrip()->Show();
	}
}

bool MainWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEscape)
		{
			delete this;
			ReturnToGame();

			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}

void MainWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		/*if (widget == newButton)
		{
			SinglePlayerWindow::Open();
			TheSinglePlayerWindow->SetCompletionProc(&SinglePlayerComplete);
			delete this;
		}
		else if (widget == loadButton)
		{
			const StringTable *table = TheGame->GetStringTable();
			const char *title = table->GetString(StringID('LOAD'));

			FilePicker *picker = new FilePicker('GAME', title, TheResourceMgr->GetSaveCatalog(), SaveResource::GetDescriptor(), nullptr, kFilePickerEnableRootDirectory | kFilePickerStayInsideRootDirectory);
			picker->SetCompletionProc(&LoadSavedGameComplete);
			AddSubwindow(picker);
		}
		else if (widget == saveButton)
		{
			const StringTable *table = TheGame->GetStringTable();
			const char *title = table->GetString(StringID('SAVE'));

			FilePicker *picker = new FilePicker('GAME', title, TheResourceMgr->GetSaveCatalog(), SaveResource::GetDescriptor(), nullptr, kFilePickerSave | kFilePickerEnableRootDirectory | kFilePickerStayInsideRootDirectory);
			picker->SetCompletionProc(&SaveCurrentGameComplete, this);
			AddSubwindow(picker);
		}
		else*/ if (widget == hostButton)
		{
		//TheEngine->Report("Hosting Test");
			//HostGameWindow::Open();
			//TheHostGameWindow->SetCompletionProc(&HostGameComplete);
			ResourceName name;
			Text::ReadString("world/inout", name, kMaxResourceNameLength);
			TheGame->HostMultiplayerGame(name, 0);
			delete this;
		}
		else if (widget == joinButton)
		{
			JoinGameWindow::Open();
			//TheJoinGameWindow->SetCompletionProc(&JoinGameComplete);
			delete this;
		}
		/*else if (widget == playerButton)
		{
			PlayerSettingsWindow::Open();
			ThePlayerSettingsWindow->SetCompletionProc(&PlayerSettingsComplete);
			delete this;
		}
		else if (widget == controlsButton)
		{
			ControlSettingsWindow::Open();
			TheControlSettingsWindow->SetCompletionProc(&ControlSettingsComplete);
			delete this;
		}
		else if (widget == graphicsButton)
		{
			GraphicsSettingsWindow::Open();
			TheGraphicsSettingsWindow->SetCompletionProc(&GraphicsSettingsComplete);
			delete this;
		}
		else if (widget == audioButton)
		{
			AudioSettingsWindow::Open();
			TheAudioSettingsWindow->SetCompletionProc(&AudioSettingsComplete);
			delete this;
		}
		else if (widget == exitButton)
		{
			TheGame->ExitCurrentGame();
			delete this;
			Open();
		}
		else if (widget == creditsButton)
		{
			delete this;
			CreditsWindow::Open();
		}
		else if (widget == quitButton)
		{
			TheEngine->Quit();
		}*/
	}




}
