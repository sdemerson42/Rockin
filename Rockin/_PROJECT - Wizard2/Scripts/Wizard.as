void Wizard_main(ScriptComponent @p)
{
	Master_wizardInit(gMaster, p);
	
	if (gMaster.getReg("destx") != 0)
	{
		p.setPosition(gMaster.getReg("destx"), gMaster.getReg("desty"));
	}
	p.setMainFunction("mainState");
}

void Wizard_setSpell(ScriptComponent @p, string spellName)
{
	p.setReg("spell", -1);

	if (spellName == "fireball")
	{
		p.setReg("spell", 0);
		p.setReg("spellCool", 30);
		p.setReg("spellCoolCounter", 0);
		p.setReg("spellSpeed", 8);	
	}
}

void Wizard_mainState(ScriptComponent @p)
{
	while(true)
	{
		auto pos = p.position();
		p.setViewCenter(pos.x, pos.y);
		// Controls

		auto input = p.input();
		Wizard_move(p, input);
		Wizard_shoot(p, input);

		p.suspend();
	}
	
	
}

void Wizard_move(ScriptComponent @p, const InputEvent &input)
{
	float speed = 5.0f;
	// Movement

	float moveX = 0;
	float moveY = 0;
	
	if (input.stickLeftX < -0.1 or input.stickLeftX > 0.1)
	{
		moveX = input.stickLeftX;	
	} 
	if (input.stickLeftY < -0.1 or input.stickLeftY > 0.1)
	{
		moveY = input.stickLeftY;	
	}

	if (moveX == 0 and moveY == 0)
	{
		p.playAnimation("idle");
	}
	else
	{
		float mag = sqrt(moveX **2 + moveY ** 2);
		moveX /= mag;
		moveY /= mag;
		moveX *= speed;
		moveY *= speed;
		p.playAnimation("walking");
	}
	p.setMomentum(moveX, moveY);
}

void Wizard_shoot(ScriptComponent @p, const InputEvent &input)
{
	int spellIndex = p.getReg("spell");
	if (spellIndex == -1) return;

	auto coolCounter = p.getReg("spellCoolCounter");
	auto counterTarget = p.getReg("spellCool");
	if (coolCounter < counterTarget)
	{
		p.modReg("spellCoolCounter", 1);
		return;
	}

	float shootX = 0;
	float shootY = 0;
	
	if (input.stickRightX < -0.1 or input.stickRightX > 0.1)
	{
		shootX = input.stickRightX;	
	} 
	if (input.stickRightY < -0.1 or input.stickRightY > 0.1)
	{
		shootY = input.stickRightY;	
	}
	if (shootX == 0 and shootY == 0) return;
	
	float speed = float(p.getReg("spellSpeed"));
	float mag = sqrt(shootX **2 + shootY ** 2);
	shootX = shootX / mag * speed;
	shootY = shootY / mag * speed;
	
	// Fireball

	if (spellIndex == 0)
	{
		auto handle = p.forceSpawn("Fireball", "main");
		if (handle !is null)
		{
			auto pos = p.position();
			handle.setPosition(pos.x + 8.0, pos.y + 8.0);
			handle.setMomentum(shootX, shootY);
		}
	}

	p.setReg("spellCoolCounter", 0);
	
}