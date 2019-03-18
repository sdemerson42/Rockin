void Meteor_main(ScriptComponent @p)
{
	while(true)
	{
		auto vec = p.position();
		
		if (vec.x < -50 or vec.x > 850 or vec.y < -50 or vec.y > 650)
		{
			auto newp = p.spawn("Meteor");
			if (newp !is null)
			{
				auto px = p.randomRange(50, 751);
				int py = -40;
				auto mx = p.randomRange(1, 3);
				auto my = p.randomRange(1, 3);
				if (px > 400)
					mx *= -1; 
				newp.setPosition(px, py);
				newp.setMomentum(mx, my);
			}
			p.despawn();
		}

		p.suspend(200);
	}
}

void Meteor_onCollision(ScriptComponent @p, Entity @collider)
{
	if (collider.hasTag("Laser"))
	{
		p.playSound("Boom", 20.0f, false, 1);
		auto pos = p.position();

		auto d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(-2, -2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(2, 2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(-2, 2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(2, -2);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(0, 2.8);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(0, -2.8);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(2.8, 0);
		}
		@d = p.spawn("Debris");
		if (d !is null)
		{
			
			d.setPosition(pos.x + 16, pos.y + 16);
			d.setMomentum(-2.8, 0);
		}

		auto masterHandle = p.getScriptByTag("Master");
		if (masterHandle !is null) Master_incScore(masterHandle);

		p.despawn();
	}
	
	if (collider.hasTag("Meteor"))
	{
		p.playSound("Boom", 5.0f, false, 1);
	}
}
