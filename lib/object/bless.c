inherit F_DBASE;


int init(string author_name, string msg, int send_time)
{
    set("author_name", author_name);
    set("msg", msg);
    set("send_time", send_time);
}

string author_name()
{
    return query("author_name");
}

string msg()
{
    return query("msg");
}

int send_time()
{
    return query("send_time");
}

string get_save_str()
{
    return sprintf("%s;%s;%d", author_name(), msg(), send_time());
}

void load_from_savestr(string save_str)
{
    string author_name;
    string msg;
    int send_time;

    sscanf(save_str, "%s;%s;%d", author_name, msg, send_time);

    set("author_name", author_name);
    set("msg", msg);
    set("send_time", send_time);
}