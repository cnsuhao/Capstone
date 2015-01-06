#ifndef SimpleCharMultiplayer_h
#define SimpleCharMultiplayer_h

#include "C4World.h"
#include "C4Input.h"
#include "C4Application.h"
#include "C4Interface.h"
#include "C4Character.h"
#include "C4Engine.h"
#include "C4Configuration.h"
#include "C4Logo.h"
#include "C4Markings.h"
#include "C4Adjusters.h"

namespace C4
{
	enum
	{
		kGameProtocol = 0x00000017,
		kGamePort = 28327
	};

	class SoldierController;

	enum 
	{
		kMessageServerInfo = kMessageBaseCount,
		kMessageSpawn,
		kMessageRequest,
		kMessageMovementBegin,
		kMessageMovementEnd,
		kMessageOrientation
	};

	class GamePlayer : public Player
	{
	public:

		GamePlayer(PlayerKey key);
		~GamePlayer();

		SoldierController *GetController() const
		{
			return cont;
		}

		void SetController(SoldierController *controller)
		{
			cont = controller;
		}

	private:

		SoldierController *cont;
	};

	class RequestMessage : public Message
	{
	public:

		RequestMessage();
		~RequestMessage();

		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);
	};

	class SpawnMessage : public Message
	{
	public:

		SpawnMessage();
		SpawnMessage(PlayerKey playerkey, int32 controllerIndex, Point3D location);
		~SpawnMessage();

		bool HandleMessage(Player *sender) const;

		void Compress(Compressor &data) const;
		bool Decompress(Decompressor &data);

		Point3D GetLocation() const
		{
			return loc;
		}

		PlayerKey GetPlayerKey() const
		{
			return key;
		}

		long GetControllerIndex() const
		{
			return contIndex;
		}
	private:

		Point3D loc;
		int32 contIndex;
		PlayerKey key;
	};

	class ClientMovementMessage : public Message
	{
		friend class Game;

	private:

		unsigned long	movementFlag;

		ClientMovementMessage(MessageType type);

	public:

		ClientMovementMessage(MessageType type, unsigned long flag);
		~ClientMovementMessage();

		long GetMovementFlag(void) const
		{
			return (movementFlag);
		}

		void Compress(Compressor& data) const;
		bool Decompress(Decompressor& data);

		bool HandleMessage(Player *sender) const;
	};

	class ClientOrientationMessage : public Message
	{
	private:

		float orientation;

	public:

		ClientOrientationMessage(void);
		ClientOrientationMessage(float _orientation);
		~ClientOrientationMessage(void);

		void Compress(Compressor& data) const;
		bool Decompress(Decompressor& data);

		bool HandleMessage(Player *sender) const;
	};
}


#endif