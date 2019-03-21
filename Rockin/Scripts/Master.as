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