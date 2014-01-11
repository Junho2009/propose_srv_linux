int main(object user, string arg)
{
    object obj = find_object(arg);
    if (0 != obj)
    {
        destruct(obj);
        load_object(arg);
    }
}