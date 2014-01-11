inherit F_DBASE;
inherit F_SAVE;


nosave mapping online_users = ([]); // 当前在线的用户。key: 用户ID；value: 用户对象
nosave mapping registered_user_dic = ([]); // 已注册过的用户。key: 用户名，value: 1

nosave int new_user_id = 0;
nosave int* free_userid_list = ({});

nosave int SAVE_INTERVAL_TIME = 59;
nosave int last_save_time = 0;


// 协议头
nosave int PROTO_HEAD_LOGIN = 10001;


int pv = 0; // 总访问数
int user_count = 0; // 总注册人数


string query_save_file();

void create()
{
    //
}

private void try_to_save()
{
    int cur_time = time();

    if (cur_time - last_save_time >= SAVE_INTERVAL_TIME)
    {
        remove_call_out("save");
        last_save_time = cur_time;
        save();
    }
    else
    {
        remove_call_out("save");
        call_out("save", last_save_time + SAVE_INTERVAL_TIME - cur_time);
    }
}

void tell_users(string str)
{
	mixed *value_list = values(online_users);
	mixed value;

	foreach(value in value_list)
	{
        if (0 != value)
		  tell_object(value, str);
	}
}

void logon(object ob)
{
	string user_id;
	int id;

	if (0 == sizeof(free_userid_list))
	{
		id = ++new_user_id;
	}
	else
	{
		id = free_userid_list[0];
		free_userid_list -= ({free_userid_list[0]});
	}

	user_id = sprintf("user%d", id);
	ob->set_id(user_id);
	online_users[user_id] = ob;

    ++pv;
    try_to_save();

	//tell_users(user_id+"进入了系统，大家热烈欢迎！\n");
}

void logout(object ob)
{
	int user_id = 0;

	map_delete(online_users, ob->get_id());

	sscanf(ob->get_id(), "user%d", user_id);
	free_userid_list += ({user_id});
}

private void handle_user_login(object user, string name)
{
	string user_id;
	string login_content = "";
	int login_flag = 0;

	user_id = user->get_id();
	if (0 == online_users[user_id])
	{
		login_flag = 0;
	}
	else
	{
		login_flag = 1;
		user->set_name(name);
	}

    if (0 == registered_user_dic[name])
    {
        registered_user_dic[name] = user;
        ++user_count;
        try_to_save();
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

string query_save_file()
{
    return COMMON_SAVE_PATH + "login";
}