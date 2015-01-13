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


#ifndef SimpleCharInterface_h
#define SimpleCharInterface_h


#include "C4World.h"
#include "C4FilePicker.h"
#include "SimpleCharMultiplayer.h"

namespace C4
{

	class GameWindow : public Window, public ListElement<GameWindow>
	{
	protected:

		GameWindow(const char *panelName);

	public:

		virtual ~GameWindow();
	};
	
	//class StartWindow : public GameWindow, public Completable<StartWindow>
	//{
	//protected:

	//	const char				*stringTableName;

	//	PushButtonWidget		*startButton;
	//	PushButtonWidget		*cancelButton;

	//	PushButtonWidget		*hostButton;
	//	PushButtonWidget		*joinButton;

	//	ListWidget				*worldList;

	//	StringTable				*worldTable;
	//	const Widget			*worldSelection;

	//	StartWindow(const char *panelName, const char *stringName);

	//	void UpdatePreview(void);

	//public:

	//	~StartWindow();

	//	const char *GetSelectedWorldName(unsigned_int32 *stringID = nullptr) const;

	//	void Preprocess(void) override;
	//	void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	//};
	//

	//class HostGameWindow : public StartWindow, public Singleton<HostGameWindow>
	//{
	//private:

	//	EditTextWidget		*gameNameText;
	//	EditTextWidget		*maxPlayersText;
	//	CheckWidget			*playHereBox;

	//	HostGameWindow();

	//public:

	//	~HostGameWindow();

	//	static void Open(void);

	//	void Preprocess(void) override;
	//	void Move(void) override;

	//	void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	//};

	class JoinGameWindow : public GameWindow, public Singleton<JoinGameWindow>//public Completable<JoinGameWindow>, public Singleton<JoinGameWindow>
	{
	private:

		class GameWidget : public Widget
		{
		public:

			int32				aliveCount;
			NetworkAddress		gameAddress;
			ResourceName		worldName;

			TextWidget			nameText;
			TextWidget			playerText;
			TextWidget			worldText;

			//GameWidget(const NetworkAddress& address, const ServerInfoMessage *message);
			//~GameWidget();
		};

		PushButtonWidget		*joinButton;
		PushButtonWidget		*cancelButton;

		EditTextWidget			*addressBox;
		ListWidget				*gameList;
		ImageWidget				*previewImage;

		int32					queryTime;

		JoinGameWindow();

		//static bool AddressFilter(unsigned_int32 code);

	public:

		~JoinGameWindow();

		static void Open(void);

		void Preprocess(void) override;
		//void Move(void) override;

		void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

		//void ReceiveServerInfo(const NetworkAddress& address, const ServerInfoMessage *message);
	};
	
	class MainWindow : public GameWindow, public Singleton<MainWindow>
	{
	private:

		TextButtonWidget		*hostButton;
		TextButtonWidget		*joinButton;

		MainWindow();

		/*static void SinglePlayerComplete(StartWindow *window, void *cookie);
		static void LoadSavedGameComplete(FilePicker *picker, void *cookie);
		static void SaveCurrentGameComplete(FilePicker *picker, void *cookie);
		*///static void HostGameComplete(StartWindow *window, void *cookie);
		//static void JoinGameComplete(JoinGameWindow *window, void *cookie);
		/*static void PlayerSettingsComplete(PlayerSettingsWindow *window, void *cookie);
		static void ControlSettingsComplete(ControlSettingsWindow *window, void *cookie);
		static void GraphicsSettingsComplete(GraphicsSettingsWindow *window, void *cookie);
		static void AudioSettingsComplete(AudioSettingsWindow *window, void *cookie);
*/
	public:

		~MainWindow();

		static void Open(void);

		static void ReturnToGame(void);

		void Preprocess(void) override;

		bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
		void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};



	//extern StartWindow *TheStartWindow;
	//extern HostGameWindow	*TheHostGameWindow;
	extern JoinGameWindow *TheJoinGameWindow;
	extern MainWindow *TheMainWindow;
}

#endif

// ZYVRPLN
