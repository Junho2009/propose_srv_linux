inherit F_DBASE;
inherit F_SAVE;


// 按10天算，总数用int存储
nosave float MAX_COUNT_PER_MIN = 100000; // 每分钟的总产出上限
nosave float MAX_NUM_PER_USER_PER_MIN = 500; // 每个用户每分钟的产出上限
nosave int RESET_DELAY_SEC = 300; // 重置可赠花数量的时间间隔（秒）
nosave int SHOW_EFFECT_NEED_NUM = 36; // 每次显示特效所需新增的赠花数
nosave int SHOW_NOTICE_NEED_NUM_PER_USER = 18; // 公告某个用户已送花信息所需的新增赠花数


// 协议头
nosave int PROTO_HEAD_SENT_INFO = 111101; // 赠花信息
nosave int PROTO_HEAD_SEND_LIM_INFO = 111102; // 赠花限额
nosave int PROTO_HEAD_SEND = 111103; // 赠送鲜花
nosave int PROTO_HEAD_USER_SENT_NOTICE = 111104; // 公告某个用户的赠花信息
nosave int PROTO_HEAD_SHOW_EFFECT = 111199; // 显示鲜花特效


nosave int last_reset_time = 0; // 上次广播重置的时间
nosave mapping send_limit_info = ([]); // 可赠送鲜花数的限制信息，保存当前仍可赠送的数量，-1表示当前已用完赠花数，0表示当前会话未曾赠送过鲜花


int sent_total = 0; // 已赠送的总鲜花数

string *user_sent_info_savedata = ({}); // 每个用户的累计赠花数（保存数据）
nosave mapping user_sent_info = ([]); // 每个用户的累计赠花数
nosave mapping user_sent_info_save_indices = ([]); // key: user_name, value: 对应用户的数据在user_sent_info_savedata中的索引+1

nosave int total_remain_now = 0; // 当前全服赠花剩余次数


string query_save_file();
private void rebuild_user_sent_info();
private void reset_users_num(int call_out_flag);

void create()
{
    if (0 != restore())
        rebuild_user_sent_info();

    call_out("start_resetnum_looptask", 10);
}



private void rebuild_user_sent_info()
{
    string user_name = "";
    int sent_num = 0;

    string savestr = "";
    int idx = 0;
    int savedata_len = sizeof(user_sent_info_savedata);

    for (idx = 0; idx < savedata_len; ++idx)
    {
        savestr = user_sent_info_savedata[idx];
        sscanf(savestr, "%s;%d", user_name, sent_num);
        user_sent_info[user_name] = sent_num;
        user_sent_info_save_indices[user_name] = idx+1;
    }

    write(sprintf("载入赠花数据，总条目：%d\n", savedata_len));
}

private string gen_reset_protostr(int duration, int total)
{
    string str = sprintf("%d;%d;%d\n", PROTO_HEAD_SEND_LIM_INFO, duration, total);
    return str;
}

private void start_resetnum_looptask()
{
    reset_users_num(0);
    call_out("reset_users_num", RESET_DELAY_SEC, 1);
}

private void reset_users_num(int call_out_flag)
{
    string content = "";
    int total = RESET_DELAY_SEC / 60.0 * MAX_NUM_PER_USER_PER_MIN;

    last_reset_time = time();
    total_remain_now = RESET_DELAY_SEC / 60.0 * MAX_COUNT_PER_MIN;

    content = gen_reset_protostr(RESET_DELAY_SEC, total);
    LOGIN_D->tell_users(content);

    if (1 == call_out_flag)
    {
        remove_call_out("reset_users_num");
        call_out("reset_users_num", RESET_DELAY_SEC, 1);
    }
}



private void query_sent_info(object user)
{
    string sent_info = "";

    int self_sent_num = 0;
    string user_name = user->get_name();

    if (0 == user->is_logined())
    {
        MSG_D->notify_user(user, "未登录，不能请求赠花信息");
        return;
    }

    if (0 != user_sent_info[user_name])
    {
        self_sent_num = user_sent_info[user_name];
    }
    else
    {
        self_sent_num = 0;
        user_sent_info[user_name] = 0;
    }

    sent_info = sprintf("%d;%d;%d\n", PROTO_HEAD_SENT_INFO, self_sent_num, sent_total);
    tell_object(user, sent_info);
}

private void query_sent_lim_info(object user)
{
    string reset_info = "";

    int duration = 0;
    int cur_remain_num = 0;

    if (0 == user->is_logined())
    {
        MSG_D->notify_user(user, "未登录，不能请求赠花限额信息");
        return;
    }

    cur_remain_num = send_limit_info[user];
    if (-1 == cur_remain_num)
    { // 已用完限额
        duration = 0;
        cur_remain_num = 0;
    }
    else
    { // 初始化 或 中途请求
        duration = RESET_DELAY_SEC - (time() - last_reset_time);

        cur_remain_num = duration / 60.0 * MAX_NUM_PER_USER_PER_MIN;
        send_limit_info[user] = cur_remain_num;
    }

    reset_info = gen_reset_protostr(duration, cur_remain_num);

    tell_object(user, reset_info);
}

private void send_flower(object user, int num)
{
    string sent_info = "";

    string tell_users_info = "";
    string sent_total_info = "";
    string user_sent_notice_info = "";
    string show_effect_info = "";

    int remain_num = 0;
    int user_sent_num = 0;
    int user_name = user->get_name();
    int savedata_idx = -1;
    string savedata = "";

    if (0 == user->is_logined())
    {
        MSG_D->notify_user(user, "未登录，不能赠花");
        return;
    }

    remain_num = send_limit_info[user];
    if (-1 == remain_num || remain_num < num)
    {
        tell_object(user, "您赠花的数量已经超过当前时间段的上限了，开挂了吧？\n");
        return;
    }

    if (total_remain_now < num)
        return; // 已超出本时间段内的本服产出上限

    total_remain_now -= num;

    send_limit_info[user] = remain_num - num;
    if (0 == send_limit_info[user])
        send_limit_info[user] = -1;

    user_sent_num = user_sent_info[user_name] = user_sent_info[user_name] + num;
    sent_total += num;

    savedata = sprintf("%s;%d", user_name, user_sent_info[user_name]);
    if (0 == user_sent_info_save_indices[user_name])
    {
        user_sent_info_savedata += ({savedata});
        user_sent_info_save_indices[user_name] = sizeof(user_sent_info_savedata);
    }
    else
    {
        savedata_idx = user_sent_info_save_indices[user_name] - 1;
        if (savedata_idx >= 0)
            user_sent_info_savedata[savedata_idx] = savedata;
    }
    save();

    sent_info = sprintf("%d;%d;%d\n", PROTO_HEAD_SENT_INFO, user_sent_info[user_name], sent_total);
    tell_object(user, sent_info);

    // 通知所有用户当前的赠花总数
    sent_total_info = sprintf("%d;%d;%d\n", PROTO_HEAD_SENT_INFO, -1, sent_total);
    tell_users_info += sent_total_info;

    // 条件满足的话，公告本用户的赠花信息
    if (0 == user_sent_num % SHOW_NOTICE_NEED_NUM_PER_USER && user_sent_num > 0)
    {
        user_sent_notice_info = sprintf("%d;%s;%d\n", PROTO_HEAD_USER_SENT_NOTICE, user_name, user_sent_info[user_name]);
        tell_users_info += user_sent_notice_info;
    }

    // 满足条件的话，显示鲜花特效
    if (0 == sent_total % SHOW_EFFECT_NEED_NUM && sent_total > 0)
    {
        show_effect_info = sprintf("%d;1\n", PROTO_HEAD_SHOW_EFFECT);
        tell_users_info += show_effect_info;
    }

    LOGIN_D->tell_users(tell_users_info);
}

int handle_protos(object user, string proto)
{
    int sn = 0;
    string content = "";
    int handleFlag = 0;

    int send_num = 0;

    if (0 == proto)
        return handleFlag;

    sscanf(proto, "%d;%s", sn, content);

    if (PROTO_HEAD_SENT_INFO == sn)
    {
        query_sent_info(user);
        handleFlag = 1;
    }
    else if (PROTO_HEAD_SEND_LIM_INFO == sn)
    {
        query_sent_lim_info(user);
        handleFlag = 1;
    }
    else if (PROTO_HEAD_SEND == sn)
    {
        sscanf(content, "%d", send_num);
        send_flower(user, send_num);
        handleFlag = 1;
    }

    return handleFlag;
}

string query_save_file()
{
    return COMMON_SAVE_PATH + "flowers";
}