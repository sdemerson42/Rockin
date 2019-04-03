ScriptComponent @P1Col = null;

void P1_main(ScriptComponent @p)
{
	p.log("P1 OK");
	float speed = 5.0f;
	bool fireState = false;
	bool talkState = false;
	ScriptComponent @dl = null;

	while(true)
	{
		auto input = p.input();

		if (input.fire)
		{
			if (!fireState and P1Col !is null)
			{
				fireState = true;
				if (!talkState)
				{
					p.setMomentum(0.0, 0.0);
					@dl = p.spawn("Dialogue");
					ShowDialogue(dl, P1Col.getString(P1Col.getReg("node")));
					NPC_updateNode(P1Col);
					talkState = true;
				}
				else
				{
					dl.despawn();
					talkState = false;
				}
			}
		}

		if (!input.fire)
		{
			fireState = false;
		}

		if (!talkState)
		{
			p.setMomentum(input.stickX * speed, input.stickY * speed);
		}
		
		auto pos = p.position();
		p.setViewCenter(pos.x + 16.0, pos.y + 16.0);
		
		// Reset state

		@P1Col = null;

		p.suspend();
	}
}


void P1_onCollision(ScriptComponent @p, Entity @e)
{
	if (e.hasTag("NPC")) @P1Col = e.scriptComponent();
}