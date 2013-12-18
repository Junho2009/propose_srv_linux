int main(object user, string arg)
{
    if (0 == user)
        return 0;

	LOGIN_D->tell_users(sprintf("%s: %s\n", user->get_name(), arg));
	return 1;
}