string get_root_uid()
{
   return ROOT_UID;
}

string get_bb_uid()
{
   return BACKBONE_UID;
}

object connect(int port)
{
    object ob;
    ob = new(USER_OB);
    return ob;
}

string creator_file(string filename)
{
    return ROOT_UID;
}

int valid_write(string file, mixed user, string func)
{
	return 1;
}