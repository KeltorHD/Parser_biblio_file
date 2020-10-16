#include <iostream>
#include <string>
#include <fstream>
#include <vector>

struct Author
{
	std::string surname;
	std::string name_pathronymic;
};

struct Publication
{
	std::string type;
	std::string title;
	std::string year_pub;
	std::string pub_house;
	std::vector<Author> authors;

};

int main()
{
	system("chcp 65001>nul");

	//system("wget -P ./html -O i.html http://www.google.com --no-check-certificate");

	std::ifstream ifs("need_to_parse.txt");

	//std::string site{ "https://elibrary.ru/item.asp?id=" };
	std::string input{};
	std::getline(ifs, input);
	std::getline(ifs, input);

	size_t counter{0};
	while (std::getline(ifs, input) && counter < 1)
	{
		//std::cout << counter << ": " << input << std::endl;

		std::string num{};
		if (input.size() > 8)
		{
			for (size_t i = 0; i < 8; i++)
			{
				if (input[i] >= '0' && input[i] <= '9')
				{
					num.append(1, input[i]);
				}
			}
			if (num.size() == 8)
			{
				std::vector<std::string> arr;
				std::string delim(",");
				size_t prev = 0;
				size_t next;
				size_t delta = delim.length();

				while ((next = input.find(delim, prev)) != std::string::npos) 
				{
					arr.push_back(input.substr(prev, next - prev));
					prev = next + delta;
				}
				arr.push_back(input.substr(prev));
				int i{};
				for (i = arr.size() - 1; i >= 0; i--)
				{
					if (arr[i].length())
						break;
				}
				arr.erase(arr.begin() + i + 1, arr.end());
				for (size_t i = 0; i < arr.size(); i++)
				{
					if (arr[i][0] == '\"')
					{
						std::string tmp{ arr[i] };
						int j = i + 1;
						for (; j < arr.size(); j++)
						{
							tmp += arr[j];
							if (arr[j][arr[j].size() - 1] == '\"')
							{
								break;
							}
						}
						arr.erase(arr.begin() + i, arr.begin() + j + 1);
						arr.insert(arr.begin() + i, tmp);
					}
				}
				for (auto& i : arr)
				{
					std::cout << i << std::endl;
				}
			}
		}


		counter++;
	}

	ifs.close();

	int i{};
	std::cin >> i;

	return bool(std::cout);
}