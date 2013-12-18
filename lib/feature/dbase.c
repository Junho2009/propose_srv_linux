mapping dbase = ([]);
nosave mapping tmp_dbase = ([]); // 临时数据


mixed set(mixed prop, mixed value)
{
	return dbase[prop] = value;
}

mixed query(mixed prop)
{
	return dbase[prop];
}

void delete(mixed prop)
{
	map_delete(dbase, prop);
}


mixed set_temp(mixed prop, mixed value)
{
	return tmp_dbase[prop] = value;
}

mixed query_temp(mixed prop)
{
	return tmp_dbase[prop];
}

void delete_temp(mixed prop)
{
	map_delete(tmp_dbase, prop);
}

