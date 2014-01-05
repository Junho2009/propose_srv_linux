nosave int PROTO_HEAD_MSG = 40001;

void notify_user(object user, string msg)
{
    string content = sprintf("%d;%s\n", PROTO_HEAD_MSG, msg);
    tell_object(user, content);
}