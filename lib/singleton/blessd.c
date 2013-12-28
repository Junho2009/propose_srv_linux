inherit F_DBASE;
inherit F_SAVE;


// 协议编号
nosave int PROTO_SN_BLESS_INFO = 131401; // 祝福信息
nosave int PROTO_SN_ADD_BLESS = 131402; // 添加祝福
nosave int PROTO_SN_BLESS_LIST = 131403; // 请求祝福列表

nosave float BLESS_NUM_PER_PAGE = 20.0;


int count = 0;
int total_page = 0;
string *blesses = ({});


string query_save_file();
void blesses_broadcast(object* bless_list);


void create()
{
    if (0 != restore())
        count = sizeof(blesses);
    else
        count = 0;
    total_page = ceil(count / BLESS_NUM_PER_PAGE);
    write(sprintf("载入祝福数据，总条目：%d\n", count));
}

void add_bless(object user, string content)
{
    string author_name;
    string msg;
    object bless;
    string bless_save_str;
    object tmp_bless;

    sscanf(content, "%s;%s", author_name, msg);
    bless = new(BLESS_OB);
    bless->init(author_name, msg, time());

    bless_save_str = bless->get_save_str();
    blesses += ({bless_save_str});

    ++count;
    total_page = ceil(count / BLESS_NUM_PER_PAGE);

    save();

    LOGIN_D->tell_users(bless_save_str);
}

void query_bless_info(object user)
{
    string info = sprintf("%d;%d;%d\n", PROTO_SN_BLESS_INFO, count, total_page);
    tell_object(user, info);
}

void send_blesses(object user, int page)
{
    string proto = "";
    string content = "";
    int i = 0;
    int begin_idx = 0;
    string bless_save_str = "";
    object bless = new(BLESS_OB);

    if (page < 1 || page > total_page)
        return;

    begin_idx = (page-1) * BLESS_NUM_PER_PAGE;
    for (i = begin_idx; i < begin_idx+BLESS_NUM_PER_PAGE && i < count; ++i)
    {
        if ("" != content)
            content += "#";

        bless_save_str = blesses[i];
        bless->load_from_savestr(bless_save_str);
        content += sprintf("author_name:%s,msg:%s,time:%d", bless->author_name(), bless->msg(), bless->send_time());
    }

    proto = sprintf("%d;%s\n", PROTO_SN_BLESS_LIST, content);
    tell_object(user, proto);
}

void blesses_broadcast(object* bless_list)
{
    object bless;
    string broadcast_strlist = "";

    foreach (bless in bless_list)
    {
        broadcast_strlist += sprintf("%d;%s;%s;%d\n", 1314, bless->author_name(), bless->msg(), bless->send_time());
    }
    LOGIN_D->tell_users(broadcast_strlist);
}

string query_save_file()
{
    return COMMON_SAVE_PATH + "blesses";
}

int handle_protos(object user, string proto)
{
    int sn = 0;
    string content = "";
    int handleFlag = 0;

    int page = 0;

    if (0 == proto)
        return handleFlag;

    sscanf(proto, "%d;%s", sn, content);

    if (PROTO_SN_ADD_BLESS == sn)
    {
        add_bless(user, content);
        handleFlag = 1;
    }
    else if (PROTO_SN_BLESS_INFO == sn)
    {
        query_bless_info(user);
        handleFlag = 1;
    }
    else if (PROTO_SN_BLESS_LIST == sn)
    {
        sscanf(content, "%d", page);
        send_blesses(user, page);
        handleFlag = 1;
    }

    return handleFlag;
}