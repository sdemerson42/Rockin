ScriptComponent @gMaster;

void Master_main(ScriptComponent @p)
{
	@gMaster = p;

	p.setReg("wizardSpell", -1);
	
	p.setSleep(true);
}

void Master_wizardInit(ScriptComponent @p, ScriptComponent @wiz)
{
	string spell = "none";
	int spellIndex = p.getReg("wizardSpell");
	
	if (0 == spellIndex) spell = "fireball";

	Wizard_setSpell(wiz, spell);	
}
