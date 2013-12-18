nosave mapping users = ([]);

nosave int cur_user_id = 0;
nosave string* free_user_id = ({});


void tell_users(string str)
{
	mixed *value_list = values(users);
	mixed value;

	foreach(value in value_list)
	{
		tell_object(value, str);
	}
}

void logon(object ob)
{
	string user_name;

	user_name = sprintf("user%d", ++cur_user_id);
	ob->set_name(user_name);
	users[user_name] = ob;

	tell_users(user_name+"进入了系统，大家热烈欢迎！\n");

	FLOWER_D->registerUser(ob);
}