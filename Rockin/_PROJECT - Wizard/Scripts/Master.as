ScriptComponent @masterSelf;

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
			auto p1 = p.spawn("P1");
			p1.setPosition(p.getReg("destX"), p.getReg("destY"));
			p.setReg("action", 0);
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
		masterSelf.changeScene(scene);
	}
}

void Master_onCollision(ScriptComponent @p, Entity @collider)
{
	
}