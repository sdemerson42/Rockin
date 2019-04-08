ScriptComponent @P1Col = null;

void P1_main(ScriptComponent @p)
{
	p.log("P1 OK");
	float speed = 5.0f;
	bool talkState = false;
	ScriptComponent @dl = null;
	auto master = p.getScriptByTag("Master");

	p.setReg("portal", 0);

	while(true)
	{
		auto input = p.input();

		if (!talkState and input.a == 1 and P1Col !is null)
		{
			if (p.getReg("portal") == 0)
			{
				p.setMomentum(0.0, 0.0);
				@dl = p.spawn("Dialogue");
				ShowDialogue(dl, P1Col.getString(P1Col.getReg("node")));
				NPC_updateNode(P1Col);
				p.playSound("Talk", 20.0f, false, 1);
				talkState = true;
			}
			else
			{
				Portal_execute(P1Col);
			}
		}
		else if (talkState and input.a == 1)
		{
			dl.despawn();
			talkState = false;
		}

		if (!talkState)
		{
			p.setMomentum(input.stickLeftX * speed, input.stickLeftY * speed);
		}

		auto momentum = p.momentum();
		if (momentum.x == 0.0f and momentum.y == 0.0f) p.playAnimation("default");
		else p.playAnimation("walk");
		
		auto pos = p.position();
		p.setViewCenter(pos.x + 16.0, pos.y + 16.0);
		
		// Reset state

		@P1Col = null;

		p.suspend();
	}
}


void P1_onCollision(ScriptComponent @p, Entity @e)
{
	if (e.hasTag("NPC") or e.hasTag("Portal"))
	{
		@P1Col = e.scriptComponent();
		if (e.hasTag("Portal")) p.setReg("portal", 1);
		else p.setReg("portal", 0);
	}
}