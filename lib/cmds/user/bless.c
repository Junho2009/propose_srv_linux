int main(object user, string arg)
{
    if (0 == user)
        return 0;
    return BLESS_D->handle_protos(user, arg);
}