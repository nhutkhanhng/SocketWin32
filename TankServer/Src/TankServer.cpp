#include <TankServerPCH.h>

TankServer::TankServer() :
	mTankControlType(ETCT_Human),
	mTimeOfNextShot(0.f),
	mTimeBetweenShots(0.2f)
{}

void TankServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void TankServer::Update()
{
	Tank::Update();

	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();
	float oldRotation = GetRotation();

	ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
	if (client)
	{

		MoveList& moveList = client->GetUnprocessedMoveList();
		for (const Move& unprocessedMove : moveList)
		{
			const InputState& currentState = unprocessedMove.GetInputState();
			float deltaTime = unprocessedMove.GetDeltaTime();
			ProcessInput(deltaTime, currentState);


			std::cout << "SimulateMovement" << currentState.GetDesiredHorizontalDelta() << currentState.GetDesiredVerticalDelta() << std::endl;



			SimulateMovement(deltaTime);
		}

		moveList.Clear();
	}

	HandleShooting();

	if (!TankMath::Is2DVectorEqual(oldLocation, GetLocation()) ||
		!TankMath::Is2DVectorEqual(oldVelocity, GetVelocity()) ||
		oldRotation != GetRotation())
	{
		NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ECRS_Pose);
	}
}

void TankServer::HandleShooting()
{
	float time = Timing::sInstance.GetFrameStartTime();
	if (mIsShooting && Timing::sInstance.GetFrameStartTime() > mTimeOfNextShot)
	{
		//not exact, but okay
		mTimeOfNextShot = time + mTimeBetweenShots;

		//fire!
		BulletPtr bullet = std::static_pointer_cast<Bullet>(GameObjectRegistry::sInstance->CreateGameObject('BULT'));
		bullet->InitFromShooter(this);
	}
}

void TankServer::TakeDamage(int inDamagingPlayerId)
{
	mHealth--;
	if (mHealth <= 0.f)
	{
		//score one for damaging player...
		ScoreBoardManager::sInstance->IncScore(inDamagingPlayerId, 1);

		//and you want to die
		SetDoesWantToDie(true);

		//tell the client proxy to make you a new cat
		ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
		if (clientProxy)
		{
			clientProxy->HandleTankDied();
		}
	}

	//tell the world our health dropped
	NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ECRS_Health);
}
