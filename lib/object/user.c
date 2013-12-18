inherit F_DBASE;


void create()
{
	enable_commands();
	add_action("cmd_hook", "", 1);
}

object logon()
{
	//call_out("login_timeout", 60);
	//this_object()->set_temp("login_flag", 0);
	LOGIN_D->logon(this_object());
}

int cmd_hook(string arg)
{
	string verb = query_verb();
	return CMD_D->do_cmd(this_object(), verb, arg);
}

void login_timeout()
{
	if (0 == this_object()->query_temp("login_flag"))
		destruct(this_object());
}

void set_name(string name)
{
	set("name", name);
}

string get_name()
{
	return query("name");
}

void net_dead()
{
	LOGIN_D->tell_users(sprintf("%s已经下线\n", get_name()));
}

string query_save_file()
{
	return USER_SAVE_PATH + query("name");
}

int save()
{
	return save_object(query_save_file());
}

