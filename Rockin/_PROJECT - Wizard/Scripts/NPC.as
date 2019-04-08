void NPC_main(ScriptComponent @p)
{
	p.log("NPC OK");
	p.setRenderFrame(p.getReg("renderX"), p.getReg("renderY"), 32.0, 32.0);
	p.setReg("node", 0);
	int animCounter = 0;
	int frame = 0;
	
	if (p.getReg("animate") == 0)
	{
		p.setSleep(true);
	}

	while(true)
	{
		++animCounter;
		if (animCounter == 30)
		{
			animCounter = 0;
			if (frame == 0)
			{
				frame = 1;
				p.setRenderFrame(p.getReg("renderX") + 32.0f, p.getReg("renderY"), 32.0, 32.0);
			}
			else
			{
				frame = 0;
				p.setRenderFrame(p.getReg("renderX"), p.getReg("renderY"), 32.0, 32.0);
			}

		}
		p.suspend();
	}
}

void NPC_updateNode(ScriptComponent @p)
{
	p.modReg("node", 1);
	if (p.getReg("node") == p.getReg("nodeTotal")) p.setReg("node", 0);
}

void NPC_onCollision(ScriptComponent @p, Entity @e)
{

}