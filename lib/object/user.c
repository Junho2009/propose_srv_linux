inherit F_DBASE;


int cmd_hook(string arg);
private void login_timeout();

void set_id(string id);
string get_id();
void set_name(string name);
string get_name();


void create()
{
	set_name("");
	enable_commands();
	add_action("cmd_hook", "", 1);
}

object logon()
{
	call_out("login_timeout", 60);
	//this_object()->set_temp("login_flag", 0);
	LOGIN_D->logon(this_object());
}

int cmd_hook(string arg)
{
	string verb = query_verb();
	return CMD_D->do_cmd(this_object(), verb, arg);
}

private void login_timeout()
{
	if ("" == get_name())
	{
		MSG_D->notify_user(this_object(), "登录超时，请刷新重连");
		destruct(this_object());
	}
}

void set_id(string id)
{
	set("id", id);
}

string get_id()
{
	return query("id");
}

void set_name(string name)
{
	set("name", name);
}

string get_name()
{
	return query("name");
}

int is_logined()
{
	int flag = 0;
	if ("" != get_name())
		flag = 1;
	return flag;
}

void net_dead()
{
	LOGIN_D->logout(this_object());
	//LOGIN_D->tell_users(sprintf("%s已经下线\n", get_name()));
}

string query_save_file()
{
	return USER_SAVE_PATH + query("name");
}

int save()
{
	return save_object(query_save_file());
}

