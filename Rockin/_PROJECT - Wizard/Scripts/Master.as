ScriptComponent @masterSelf;
ScriptComponent @masterBlack;

void Master_main(ScriptComponent @p)
{
	p.log("Master OK");
	@masterSelf = p;
	p.playMusic("Assets/Sounds/Castle.wav", 10.0f, true);
	p.setReg("action", 0);

	p.spawn("P1");
	
	while(true)
	{
		if (p.getReg("action") == 1)
		{
			// Fade out
			p.modReg("blackAlpha", 10);
			if (p.getReg("blackAlpha") >= 255)
			{
				p.setReg("action", 2);
				masterSelf.changeScene(p.getString(0));
			}
			else masterBlack.setRenderColor(255, 255, 255, p.getReg("blackAlpha"));
		}

		else if (p.getReg("action") == 2)
		{
			auto p1 = p.spawn("P1");
			p1.setPosition(p.getReg("destX"), p.getReg("destY"));
			p.setReg("blackAlpha", 255);
			@masterBlack = p.spawn("Black");
			p.setReg("action", 3);
		}

		else if (p.getReg("action") == 3)
		{
			// Fade in
			p.modReg("blackAlpha", -10);
			if (p.getReg("blackAlpha") <= 0)
			{
				p.setReg("action", 0);
				masterBlack.despawn();
				auto p1 = p.getScriptByTag("P1");
				p1.setReg("locked", 0);
			}
			else masterBlack.setRenderColor(255, 255, 255, p.getReg("blackAlpha"));
		}
	
		p.suspend();
	}
}

void Master_changeScene(string scene, int ty, float x, float y)
{
	if (ty == 0)
	{
		masterSelf.setReg("destX", x);
		masterSelf.setReg("destY", y);
		masterSelf.setReg("action", 1);
		@masterBlack = masterSelf.spawn("Black");
		masterBlack.setRenderColor(255, 255, 255, 0);
		masterSelf.setReg("blackAlpha", 0);
		masterSelf.setString(0, scene);
		
	}
}

void Master_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)
{
	
}