int save()
{
    string file;

    if (stringp(file = this_object()->query_save_file()))
        return save_object(file,1);
    else
        return 0;
}

int restore()
{
    string file;

    if (stringp(file = this_object()->query_save_file()))
        return restore_object(file);
    else
        return 0;
}