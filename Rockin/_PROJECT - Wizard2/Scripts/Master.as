ScriptComponent @gMaster;

void Master_main(ScriptComponent @p)
{
	@gMaster = p;
	p.setSleep(true);
}