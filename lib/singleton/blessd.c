inherit F_DBASE;
inherit F_SAVE;


// 协议头
nosave int PROTO_HEAD_RECV_BLESS = 131401;


int count = 0;
string *blesses = ({});


string query_save_file();
void blesses_broadcast(object* bless_list);


void create()
{
    if (0 != restore())
        count = sizeof(blesses);
    else
        count = 0;
    write(sprintf("载入祝福数据，总条目：%d\n", count));
}

int add_bless(object sender, string content)
{
    string author_name;
    string msg;
    object bless;
    string bless_save_str;
    object tmp_bless;

    sscanf(content, "%s|%s", author_name, msg);
    bless = new(BLESS_OB);
    bless->init(author_name, msg, time());

    bless_save_str = bless->get_save_str();
    blesses += ({bless_save_str});
    ++count;
    save();

    tell_object(sender, "当前祝福数："+count+"\n");
    LOGIN_D->tell_users(sprintf("%s发来祝福：%s\n", bless->author_name(), bless->msg()));
    blesses_broadcast(({bless}));

    /*
    tmp_bless = new(BLESS_OB);
    write("目前收到的祝福：\n");
    foreach(bless_save_str in blesses)
    {
        tmp_bless->load_from_savestr(bless_save_str);
        write(sprintf("%s发来祝福：%s\n", tmp_bless->author_name(), tmp_bless->msg()));
    }
    */

    return 1;
}

void blesses_broadcast(object* bless_list)
{
    object bless;
    string broadcast_strlist = "";

    foreach (bless in bless_list)
    {
        broadcast_strlist += sprintf("%d|%s|%s|%d\n", PROTO_HEAD_RECV_BLESS, bless->author_name(), bless->msg(), bless->send_time());
    }
    LOGIN_D->tell_users(broadcast_strlist);
}

string query_save_file()
{
    return COMMON_SAVE_PATH + "blesses";
}

