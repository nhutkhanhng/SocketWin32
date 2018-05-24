enum ETankControlType
{
	ETCT_Human,
	ETCT_AI
};

class TankServer : public Tank
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new TankServer()); }
	virtual void HandleDying() override;
	virtual void Update() override;
	void SetTankControlType(ETankControlType inTankControlType) { mTankControlType = inTankControlType; }
	void TakeDamage(int inDamagingPlayerId);

protected:
	TankServer();

private:
	void HandleShooting();
	ETankControlType	mTankControlType;
	float		mTimeOfNextShot;
	float		mTimeBetweenShots;

};