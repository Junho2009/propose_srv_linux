nosave int PROTO_HEAD_NOTIFY = 40001; // 弹出消息对话框

void notify_user(object user, string msg)
{
    string content = sprintf("%d;%s\n", PROTO_HEAD_NOTIFY, msg);
    tell_object(user, content);
}