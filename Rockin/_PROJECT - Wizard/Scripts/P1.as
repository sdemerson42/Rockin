void P1_main(ScriptComponent @p)
{
	p.log("P1 OK");
	float speed = 5.0f;
	auto master = p.getScriptByTag("Master");

	p.setReg("portal", 0);
	p.setReg("locked", 0);
	p.setReg("talk", 0);

	while(true)
	{
		// No action if locked

		if (p.getReg("locked") == 1)
		{
			p.setMomentum(0.0, 0.0);
			p.suspend();
			continue;
		}

		auto input = p.input();

		if (p.getReg("talk") == 0 and input.a == 1 and p.getScript("target") !is null)
		{
			if (p.getReg("portal") == 0)
			{
				P1_onTalk(p);
			}
			else
			{
				P1_onPortal(p);
			}
		}
		else if (p.getReg("talk") == 1 and input.a == 1)
		{
			auto dl = p.getScript("dialogue");
			dl.despawn();
			p.setReg("talk", 0);
		}

		if (p.getReg("talk") == 0 and p.getReg("locked") == 0)
		{
			p.setMomentum(input.stickLeftX * speed, input.stickLeftY * speed);
		}

		auto momentum = p.momentum();
		if (momentum.x == 0.0f and momentum.y == 0.0f) p.playAnimation("default");
		else p.playAnimation("walk");
		
		auto pos = p.position();
		p.setViewCenter(pos.x + 16.0, pos.y + 16.0);
		
		// Reset state

		p.setScript("target", null);
	
		p.suspend();
	}
}

void P1_onTalk(ScriptComponent @p)
{
	p.setMomentum(0.0, 0.0);
	auto dl = p.spawn("Dialogue");
	auto target = p.getScript("target");
	ShowDialogue(dl, target.getString(target.getReg("node")));
	NPC_updateNode(target);
	p.playSound("Talk", 20.0f, false, 1);
	p.setReg("talk", 1);
	p.setScript("dialogue", dl);
}

void P1_onPortal(ScriptComponent @p)
{
	p.setReg("locked", 1);
	Portal_execute(p.getScript("target"));
}


void P1_onCollision(ScriptComponent @p, Entity @e, PhysicsComponent @c)
{
	if (e.hasTag("NPC") or e.hasTag("Portal"))
	{
		p.setScript("target", e.scriptComponent());
		if (e.hasTag("Portal")) p.setReg("portal", 1);
		else p.setReg("portal", 0);
	}
}