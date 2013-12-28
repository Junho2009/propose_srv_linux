inherit F_DBASE;
inherit F_SAVE;


// 按10天算，总数用int存储
nosave int MAX_COUNT_PER_MIN = 100000; // 每分钟的总产出上限
nosave int MAX_NUM_PER_USER_PER_MIN = 100; // 每个用户每分钟的产出上限
nosave int RESET_DELAY_MIN = 5; // 重置可赠花数量的时间间隔（分）


// 协议头
nosave int PROTO_HEAD_RESET_FLOWER = 111101;


nosave int last_reset_time = 0;

nosave int cur_user_num = 0;


private void resetUsersNum(int call_out_flag);


void create()
{
    //
}

void registerUser(object user)
{
    return; //testing
    ++cur_user_num;
    if (1 == cur_user_num)
    {
        resetUsersNum(0);
        call_out("resetUsersNum", RESET_DELAY_MIN*1, 1);
    }
}

private void resetUsersNum(int call_out_flag)
{
    string content = sprintf("%d;%d\n", PROTO_HEAD_RESET_FLOWER, MAX_NUM_PER_USER_PER_MIN*RESET_DELAY_MIN);
    LOGIN_D->tell_users(content);

    if (1 == call_out_flag)
        call_out("resetUsersNum", RESET_DELAY_MIN*1, 1);
}