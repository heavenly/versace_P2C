#include "friends_list.h"
#include "json.h"

namespace modified_friends_list
{
	std::vector<CSteamID> friends = { };
	void save_friends_to_file()
	{
		Json::Value friends_list_as_json;
		friends_list_as_json["size"] = (int)friends.size();
		for(int i = 0; i < friends.size(); i++)
		{
			friends_list_as_json[std::to_string(i)] = friends.at(i).ConvertToUint64();
		}

		std::string str_json = friends_list_as_json.toStyledString();
		FILE* file = fopen("C:\\VER$ACE\\friends_list\\friends_list.json", "w+");
		if (file)
		{
			fwrite(str_json.c_str(), 1, str_json.length(), file);
			fclose(file);
		}
	}

	void load_friends_from_file()
	{
		FILE* infile = fopen("C:\\VER$ACE\\friends_list\\friends_list.json", "r");

		if (!infile)
			return;

		fseek(infile, 0, SEEK_END);
		const long filesize = ftell(infile);
		char* buf = new char[filesize + 1];
		fseek(infile, 0, SEEK_SET);
		fread(buf, 1, filesize, infile);
		fclose(infile);

		buf[filesize] = '\0';
		std::stringstream ss;
		ss.str(buf);
		delete[] buf;

		Json::Value friends_json;
		ss >> friends_json;

		for(int i = 0; i < friends_json["size"].asInt(); i++)
		{
			friends.emplace_back(friends_json[std::to_string(i)].asUInt64());
		}
	}


};