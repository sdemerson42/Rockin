void MenuLogic_main(ScriptComponent @p)
{
	p.log("MenuLogic: OK");
	int score = ScoreState_getScore(gScoreState);
	p.setTextString("   Rockin\n\nHigh Score: " + score);
	while(true)
	{
		auto i = p.input();
		if (1 == i.start)
		{
			p.changeScene("Scene1");
		}
		p.suspend();
	}
}

void MenuLogic_onCollision(ScriptComponent @p, Entity @collider, PhysicsComponent @c)
{
	
}