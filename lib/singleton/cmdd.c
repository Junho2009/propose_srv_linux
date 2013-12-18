int do_cmd(object me,string verb,string arg)
{
    object cmd_obj = load_object(CMD_USER_PATH + verb);
    if (0 != cmd_obj)
        return cmd_obj->main(me, arg);
    else
        return 0;
}