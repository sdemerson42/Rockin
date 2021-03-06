ScriptComponent @gMaster;

void Master_main(ScriptComponent @p)
{
	@gMaster = p;

	p.log("Master OK");
	p.setReg("score", 0);
	p.setReg("endCounter", 0);
	auto p1Handle = p.spawn("P1");
	p1Handle.setPosition(390.0, 568.0);

	auto clouds = p.spawn("Clouds");
	clouds.setPosition(0.0, 0.0);
	@clouds = p.spawn("Clouds");
	clouds.setPosition(0.0, -600.0);
	
	while(true)
	{
		p.suspend(50);
		auto x = p.randomRange(0, 761);
		float y = -35;
		auto mx = p.randomRange(-3, 4);
		auto my = p.randomRange(1, 4);
		if (x > 400) mx *= -1;
		string tag = "SmallMeteor";
		if (p.randomRange(1,3) == 1) tag = "Meteor";
		auto handle = p.spawn(tag);
		if (handle !is null)
		{
			handle.setPosition(x,y);
			handle.setMomentum(mx, my);
		}

		if (p.getReg("endCounter") > 0)
		{
			p.modReg("endCounter", 1);
			if (p.getReg("endCounter") == 5)
			{
				auto hiScore = ScoreState_getScore(gScoreState);
				auto score = p.getReg("score");
				if (score > hiScore)
				{
					ScoreState_setScore(gScoreState, score);
				}
				p.changeScene("SceneMenu");
			}
		}
	}
}


void Master_onCollision(ScriptComponent @me, Entity @collider, PhysicsComponent @c)
{
	
}

void Master_incScore(ScriptComponent @p)
{
	p.modReg("score", 1);
	p.setTextString("Score: " + p.getReg("score"));
}

void Master_explosion(ScriptComponent @p)
{
	auto pos = p.position();
	auto sz = p.spriteSize();
	auto x = pos.x + sz.x / 2.0;
	auto y = pos.y + sz.y / 2.0;

		auto d = p.spawn("Debris");
		if (d !is null)
		{
			d.setPosition(x, y);
			d.setMomentum(-2, -2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(2, 2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(-2, 2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(2, -2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(0, 2.8);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(0, -2.8);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(2.8, 0);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(x, y);
			d.setMomentum(-2.8, 0);
		}
		p.playSound("Boom", 20.0f, false, 1);
}

void Master_playerDead(ScriptComponent @p)
{
	p.setReg("endCounter", 1);
	p.log("PLAYER_DEAD");
}