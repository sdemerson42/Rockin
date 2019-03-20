void Master_main(ScriptComponent @p)
{
	p.setReg("score", 0);
	auto p1Handle = p.spawn("P1");
	p1Handle.setPosition(390.0, 568.0);

	while(true)
	{
		p.suspend(90);
		auto x = p.randomRange(0, 761);
		float y = -50;
		auto mx = p.randomRange(1, 4);
		auto my = p.randomRange(-3, 4);
		if (x > 400) mx *= -1;
		auto handle = p.spawn("Meteor");
		if (handle !is null)
		{
			handle.setPosition(x,y);
			handle.setMomentum(mx, my);
		}
	}
}


void Master_onCollision(ScriptComponent @p, Entity @collider)
{
	
}

void Master_incScore(ScriptComponent @p)
{
	p.modReg("score", 1);
	p.log("Score: " + p.getReg("score"));
}