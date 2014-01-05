nosave mapping users = ([]);

nosave int cur_user_id = 0;
nosave int* free_userid_list = ({});

// 协议头
nosave int PROTO_HEAD_LOGIN = 10001;


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
	string user_id;
	int id;

	if (0 == sizeof(free_userid_list))
	{
		id = ++cur_user_id;
	}
	else
	{
		id = free_userid_list[0];
		free_userid_list -= ({free_userid_list[0]});
	}

	user_id = sprintf("user%d", id);
	ob->set_id(user_id);
	users[user_id] = ob;

	tell_users(user_id+"进入了系统，大家热烈欢迎！\n");
}

void logout(object ob)
{
	int user_id = 0;

	map_delete(users, ob->get_id());

	sscanf(ob->get_id(), "user%d", user_id);
	free_userid_list += ({user_id});
}

private void handle_user_login(object user, string name)
{
	string user_id;
	string login_content = "";
	int login_flag = 0;

	user_id = user->get_id();
	if (0 == users[user_id])
	{
		login_flag = 0;
	}
	else
	{
		login_flag = 1;
		user->set_name(name);
	}

	login_content = sprintf("%d;%d;%s\n", PROTO_HEAD_LOGIN, login_flag, name);
	tell_object(user, login_content);
}

int handle_protos(object user, string proto)
{
	int sn = 0;
    string content = "";
    int handleFlag = 0;

    string name = "";

    if (0 == proto)
        return handleFlag;

    sscanf(proto, "%d;%s", sn, content);

    if (PROTO_HEAD_LOGIN == sn)
    {
    	sscanf(content, "%s", name);
    	handle_user_login(user, name);
    	handleFlag = 1;
    }

    return handleFlag;
}